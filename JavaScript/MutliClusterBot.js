// ==================== HOW TO RUN THIS =====================
// Call:
// "node Client.js -h [host] -p [port] -k [key]"
//
// If no argument given, it'll be 127.0.0.1:3011
// key is a secret string that authenticate the bot identity
// it is not required when testing
// ===========================================================

// Get the host and port from argurment
var VERSION = 1;

var host = "127.0.0.1";
var port = 3011;
var key = 0;
for (var i=0; i<process.argv.length; i++) {
	if (process.argv[i] == "-h") {
		host = process.argv[i + 1];
	}
	else if (process.argv[i] == "-p") {
		port = process.argv[i + 1];
	}
	else if (process.argv[i] == "-k") {
		key = process.argv[i + 1];
	}
}
if (host == null) host = "127.0.0.1";
if (port == null) port = 3011;
if (key == null) key = 0;






// ================== BEHIND THE SCENE STUFF =================
// Game definition
var GAMESTATE_WAIT_FOR_PLAYER = 0;
var GAMESTATE_COMMENCING = 1;
var GAMESTATE_END = 2;

var COMMAND_SEND_KEY = 1;
var COMMAND_SEND_INDEX = 2;
var COMMAND_SEND_DIRECTION = 3;
var COMMAND_SEND_STAGE = 4;

var TURN_PLAYER_1 = 1;
var TURN_PLAYER_2 = 2;

var BLOCK_EMPTY = 0;
var BLOCK_PLAYER_1 = 1;
var BLOCK_PLAYER_1_TRAIL = 2;
var BLOCK_PLAYER_2 = 3;
var BLOCK_PLAYER_2_TRAIL = 4;
var BLOCK_OBSTACLE = 5;

var DIRECTION_LEFT = 1;
var DIRECTION_UP = 2;
var DIRECTION_RIGHT = 3;
var DIRECTION_DOWN = 4;

var turn = TURN_PLAYER_1;
var gameState = GAMESTATE_WAIT_FOR_PLAYER;

var MAP_SIZE = 11;



var map = new Array();
var winner = null;
var index = 0;

// These are friendly variable for user only
var myPosition = new Position(0, 0);
var enemyPosition = new Position(0, 0);
var board = new Array();
for (var i=0; i<MAP_SIZE; i++) {
	board[i] = new Array();
	for (var j=0; j<MAP_SIZE; j++) {
		board[i][j] = 0;
	}
}


// Position object
function Position(x, y) {
	this.x = x;
	this.y = y;
}

// When receive a packet from server
function OnUpdatePacket(data, offset) {
	// Update all variable
	var i = offset;
	gameState = data[i].charCodeAt(0); i ++;
	turn = data[i].charCodeAt(0); i ++;
	winner = data[i].charCodeAt(0); i ++;
	for (var j=0; j<MAP_SIZE * MAP_SIZE; j++) {
		map[j] = data[i].charCodeAt(0); i ++;
	}
	
	// If it's player turn, notify them to get their input
	if (gameState == GAMESTATE_COMMENCING && turn == index) {
		ConvertVariable();
		MyTurn();
	}
	else {
		// Do something while waiting for your opponent
	}
}

// Player need to give a command here
function Command(dir) {
	if (gameState == GAMESTATE_COMMENCING && turn == index) {
		var data = "";
		data += String.fromCharCode(COMMAND_SEND_DIRECTION);
		data += String.fromCharCode(dir);
		Send (data);
	}
}

// Helper
function ConvertCoord (x, y) {
	return y * MAP_SIZE + x;
}
function ConvertVariable () {
	for (var i=0; i<MAP_SIZE; i++) {
		board[i] = new Array();
		for (var j=0; j<MAP_SIZE; j++) {
			board[i][j] = map[ConvertCoord(i, j)];
			
			if (board[i][j] == BLOCK_PLAYER_1) {
				if (index == TURN_PLAYER_1) {
					myPosition.x = i;
					myPosition.y = j;
				}
				else {
					enemyPosition.x = i;
					enemyPosition.y = j;
				}
			}
			else if (board[i][j] == BLOCK_PLAYER_2) {
				if (index == TURN_PLAYER_2) {
					myPosition.x = i;
					myPosition.y = j;
				}
				else {
					enemyPosition.x = i;
					enemyPosition.y = j;
				}
			}
		}
	}
}


// Engine
var socketStatus = 0;
var SOCKET_STATUS_ONLINE = 1;
var SOCKET_STATUS_OFFLINE = 0;


// Start new connection to server
var ws;
try {
	ws = require("./NodeWS");
}
catch (e) {
	ws = require("./../NodeWS");
}

var socket = ws.connect ("ws://" + host + ":" + port, [], function () {
	socketStatus = SOCKET_STATUS_ONLINE;
	
	// Send your key (even if you don't have one)
	var data = "";
	data += String.fromCharCode(COMMAND_SEND_KEY);
	data += String.fromCharCode(key);
	data += String.fromCharCode(VERSION);
	Send (data);
});
socket.on("text", function (data) {
	var command = data[0].charCodeAt(0);
	if (command == COMMAND_SEND_INDEX) {
		// Server send you your index, update it
		index = data[1].charCodeAt(0);
	}
	else if (command == COMMAND_SEND_STAGE) {
		OnUpdatePacket(data, 1);
	}
});
socket.on("error", function (code, reason) {
	socketStatus = SOCKET_STATUS_OFFLINE;
});

// Send data through socket
function Send(data) {
	if (socketStatus == SOCKET_STATUS_ONLINE) {
		socket.sendText(data);
	}
}
// ===========================================================













//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//                                    GAME RULES                                    //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////
// - Game board is an array of MAP_SIZExMAP_SIZE blocks                             //
// - 2 players starts at 2 corners of the game board                                //
// - Each player will take turn to move                                             //
// - Player can only move left/right/up/down and stay inside the game board         //
// - The game is over when one of 2 players cannot make a valid move                //
// - In a competitive match:                                                        //
//   + A player will lose if they cannot connect to server within 10 seconds        //
//   + A player will lose if they don't make a valid move within 3 seconds          //
//////////////////////////////////////////////////////////////////////////////////////

// ===================== PLAYER'S PART =======================
// Do not modify the code above, you won't be able to 'hack',
// all data sent to server is double checked there.
// Further more, if you cause any damage to the server or
// wrong match result, you'll be disqualified right away.
//
// When it's your turn, function "MyTurn" function will be called.
// To make a move, you must call function "Command" with input is
// the direction you want to move. The list of the argument here:
// - DIRECTION_LEFT
// - DIRECTION_UP
// - DIRECTION_RIGHT
// - DIRECTION_DOWN
//
// To give the decision, you must certainly consider the current
// board state. You can use the following variables:
// * Your position:
// - myPosition.x
// - myPosition.y
// * Your opponent position:
// - enemyPosition.x
// - enemyPosition.y
// * Board:
// - board[x][y]
// "board" is a 2D array, which will define board status.
// Square with value 0 means empty. Anything other than 0 is 
// where you cannot move to.
// The full list of variable is:
// - BLOCK_EMPTY = 0;
// - BLOCK_PLAYER_1 = 1;
// - BLOCK_PLAYER_1_TRAIL = 2; Square player 1 went through before
// - BLOCK_PLAYER_2 = 3;
// - BLOCK_PLAYER_2_TRAIL = 4; Square player 2 went through before
// - BLOCK_OBSTACLE = 5;
// Which player you are? You can know it from variable "index"
// Player 1 have value 1, and player 2 have value 2, but you probably
// don't care about that anyway.
//
// That's pretty much about it. Now, let's start coding.
// ===========================================================const BLOCK_OCCUPIED = 6;
const UNKNOWN_DIRECTION = 0;

const MOVE_X = [0, -1, 0, 1, 0];
const MOVE_Y = [0, 0, -1, 0, 1];

const INF = 400;

const START_DEPTH_LEVEL = 12;

const MAX_TIME = 2000;

const UNKNOWN_HEUR_VAL = 0;

///////////////////////////////////////////////Support Function////////////////////////////////////////////////
function inMatrix(pos){
  if (pos.x >= 0 && pos.x < MAP_SIZE){
    if (pos.y >= 0 && pos.y < MAP_SIZE){
      return true;
    }
  }
  return false;
}

function restoreBoard(board, restoreList){
  for (var idList = 0; idList < restoreList.length; ++ idList){
    board[restoreList[idList].x][restoreList[idList].y] = BLOCK_EMPTY;
  }
}

function moveDirection(pos, direction){
  var newX = pos.x + MOVE_X[direction];
  var newY = pos.y + MOVE_Y[direction];
  return new Position(newX, newY);
}

function whichDirection(pos, nextPos){
  for (var direction = 1; direction <= 4; ++ direction){
    if (nextPos.x - pos.x === MOVE_X[direction]){
      if (nextPos.y - pos.y === MOVE_Y[direction]){
        return direction;
      }      
    }
  }
  return UNKNOWN_DIRECTION;
}

function calMahattanDist(pos, otherPos){
  return Math.abs(pos.x - otherPos.x) + Math.abs(pos.y - otherPos.y);
}

function calSqrEuclideDist(pos, otherPos){
  return Math.pow((pos.x - otherPos.x), 2) + Math.pow((pos.y - otherPos.y),2);
}

function isSplit(board, myPos, enemyPos){
  var myID = board[myPos.x][myPos.y];
  var stack = [];
  var restoreList = new Array();

  for (var direction = 1; direction <= 4; ++ direction){
    var newPos = moveDirection(myPos, direction);
    if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
      board[newPos.x][newPos.y] = BLOCK_OCCUPIED;
      stack.push(newPos);
      restoreList.push(newPos);
    }
  }

  var result = true;
  while (stack.length > 0 && result !== false){
    var pos = stack.pop();
    
    for (var direction = 1; direction <= 4; ++ direction){
      var newPos = moveDirection(pos, direction);
      if (inMatrix(newPos)){
        if (newPos.x === enemyPos.x && newPos.y === enemyPos.y){
          result = false;
          break;
        } else if (board[newPos.x][newPos.y] === BLOCK_EMPTY){
          board[newPos.x][newPos.y] = BLOCK_OCCUPIED;
          stack.push(newPos);
          restoreList.push(newPos);
        }
      }
    }
  }

  restoreBoard(board, restoreList);
  board[myPos.x][myPos.y] = myID;

  return result;
};

function print(board){
  var string = "";
  for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
    for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
      string += "\'";
      if (board[idRow][idCol] < 10 && board[idRow][idCol] >= 0){
       string += "   ";
      } else if (board[idRow][idCol] < 100 && board[idRow][idCol] > -10){
        string += "  ";
      } else if (board[idRow][idCol] < 1000 && board[idRow][idCol] > -100){
        string += " ";
      }
      string += board[idRow][idCol] +  "\'";
    }
    string += "\n";
  }
  //console.error(string);
}

function getArea(board, pos){
  var result = 0;
  var stack = [];
  var restoreList = new Array();
  var myID = board[pos.x][pos.y];
  board[pos.x][pos.y] = BLOCK_OCCUPIED;
  stack.push(pos);
  restoreList.push(pos);

  while (stack.length > 0){
    var curPos = stack.pop();
    ++ result;
    for (var direction = 1; direction <= 4; ++ direction){
      var newPos = moveDirection(curPos, direction);
      if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
        board[newPos.x][newPos.y] = BLOCK_OCCUPIED;
        stack.push(newPos);
        restoreList.push(newPos);
      }
    }
  }
  restoreBoard(board, restoreList);
  board[pos.x][pos.y] = myID;

  return result;
}

function calConnection(board){
  var numCon = new Array();
  for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
    numCon[idRow] = new Array();
    for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
      if (board[idRow][idCol] !== BLOCK_EMPTY){
        numCon[idRow][idCol] = 0;
      } else {
        numCon[idRow][idCol] = 0;
        for (var direction = 1; direction <= 4; ++direction){
          var newPos = moveDirection(new Position(idRow, idCol), direction);
          if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
            ++ numCon[idRow][idCol];
          }
        }    
      }      
    }
  }

  return numCon;    
}

function calMinDistToAll(board, pos){
  var dist = new Array();
  for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
    dist[idRow] = new Array();
    for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
      dist[idRow][idCol] = INF;
    }
  }
    
  var queue = [];
  queue.push(pos);

  dist[pos.x][pos.y] = 0;  
  while (queue.length > 0){
    var curPos = queue.shift();
    for (var direction = 1; direction <= 4; ++ direction){
      var newPos = moveDirection(curPos, direction);
      if (inMatrix(newPos)){
        if (board[newPos.x][newPos.y] === BLOCK_EMPTY && dist[newPos.x][newPos.y] === INF){
          dist[newPos.x][newPos.y] = dist[curPos.x][curPos.y] + 1;
          queue.push(newPos);
        }
      }
    }
  }

  return dist;
}

function compareConnection(a, b){
  if (a[1] < b[1]){
    return -1;
  } else if (a[1] > b[1]){
    return 1;
  } else {
    return 0;
   }
}

function compareVirMove(a, b){
  if (a[1] < b[1]){
    return -1;
  } else if (a[1] > b[1]){
    return 1;
  } else if (a[2] < b[2]){
    return -1;    
  } else if (a[2] > b[2]){
    return 1;
  } else {
    return 0;
  }
}


///////////////////////////////////////////////FindMaxLengthInStatic////////////////////////////////////////////////

function heurEstLongest(board, myPos, connection, isFirst, restoreList){
  var myID;
  if (isFirst === true){
    myID = board[myPos.x][myPos.y];
    board[myPos.x][myPos.y] = BLOCK_EMPTY;
    restoreList = new Array();
  }

  var nextMoves = [];
  for (var direction = 1; direction <= 4; ++ direction){
    var newPos = moveDirection(myPos, direction);
    if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
      -- connection[newPos.x][newPos.y];
      nextMoves.push([direction, connection[newPos.x][newPos.y]]);
    }
  }
  
  nextMoves.sort(compareConnection);

  board[myPos.x][myPos.y] = BLOCK_OCCUPIED;
  restoreList.push(myPos);
  
  var result = 1;
  for (var idMove = 0; idMove < nextMoves.length; ++ idMove){
    var direction = nextMoves[idMove][0];
    
    var newPos = moveDirection(myPos, direction);
    if (board[newPos.x][newPos.y] === BLOCK_EMPTY){
      result = Math.max(result, heurEstLongest(board, newPos, connection, false, restoreList) + 1);
    }  
  }

  if (isFirst === true){
    //console.error(restoreList);
    restoreBoard(board, restoreList);
    board[myPos.x][myPos.y] = myID;
  }
  
  return result;
}

function dlsEstLongest(board, myPos, depthLvl, depthLimitLvl){
  if (depthLvl >= depthLimitLvl){
    return heurEstLongest(board, myPos, calConnection(board), true);
  }
  
  var myID = board[myPos.x][myPos.y];
  ++ board[myPos.x][myPos.y];//Make it trail

  var result = 1;
  var directionToMove = UNKNOWN_DIRECTION;
  for (var direction = 1; direction <= 4; ++ direction){
    var newPos = moveDirection(myPos, direction);
    if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
      board[newPos.x][newPos.y] = myID;
      var temp = dlsEstLongest(board, newPos, depthLvl + 1, depthLimitLvl);
      if (temp + 1 > result){
        result = temp + 1;
        directionToMove = direction;
      }
      board[newPos.x][newPos.y] = BLOCK_EMPTY;
    }
  }
  -- board[myPos.x][myPos.y];//make it not trail

  if (depthLvl === 0){
    return [result, directionToMove];
  } else {
    return result;
  }
}

///////////////////////////////////////////////FindMaxLengthInDynamic////////////////////////////////////////////////

//TODO
function heurEstForNonSplit(board, myPos, enemyPos, isMaxPlayer){
  var myVirPos, enemyVirPos;
  myVirPos = new Position(myPos.x, myPos.y);
  enemyVirPos = new Position(enemyPos.x, enemyPos.y);

  var myDist, enemyDist = calMinDistToAll(board, enemyVirPos);
  var restoreList = new Array();

  var trace = [[myPos.x, myPos.y]];
  while (enemyDist[myVirPos.x][myVirPos.y] > 0){
    var virMoves = [];
    for (var direction = 1; direction <= 4; ++ direction){
      var newPos = moveDirection(myVirPos, direction);
      if (inMatrix(newPos) && enemyDist[newPos.x][newPos.y] < enemyDist[myVirPos.x][myVirPos.y]){
        virMoves.push([newPos, calMahattanDist(newPos, enemyVirPos), calSqrEuclideDist(newPos, enemyVirPos)]);
      }
    }
    //console.error(virMoves);
    virMoves.sort(compareVirMove);
    myVirPos.x = virMoves[0][0].x;
    myVirPos.y = virMoves[0][0].y;
    trace.push([myVirPos.x, myVirPos.y]);
    if (myVirPos.x != enemyVirPos.x || myVirPos.y != enemyVirPos.y){
      board[myVirPos.x][myVirPos.y] = BLOCK_OCCUPIED;
      restoreList.push(new Position(myVirPos.x, myVirPos.y));
    } else {
      break;
    }    
  }

  if (isMaxPlayer === true){
    var myInTrace = Math.floor((trace.length - 1) / 2);
    //console.error("myInTrace = " + myInTrace + " trace.length = " + trace.length);
    myVirPos.x = trace[myInTrace][0];
    myVirPos.y = trace[myInTrace][1]
    enemyVirPos.x = trace[myInTrace + 1][0];
    enemyVirPos.y = trace[myInTrace + 1][1];
  } else {
    var enemyInTrace = Math.floor(trace.length / 2);
    //console.error("enemyInTrace = " + enemyInTrace);
    myVirPos.x = trace[enemyInTrace - 1][0];
    myVirPos.y = trace[enemyInTrace - 1][1];
    enemyVirPos.x = trace[enemyInTrace][0];
    enemyVirPos.y = trace[enemyInTrace][1];
  }

  myDist = calMinDistToAll(board, myVirPos);
  enemyDist = calMinDistToAll(board, enemyVirPos);

  var diff = 0;
//  var diffDist = new Array();
  for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
//    diffDist[idRow] = new Array();
    for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
      if (myDist[idRow][idCol] > enemyDist[idRow][idCol]){
        -- diff;
      } else if (myDist[idRow][idCol] < enemyDist[idRow][idCol]){
        ++ diff;
      }
//      diffDist[idRow][idCol] = myDist[idRow][idCol] - enemyDist[idRow][idCol];
    }
  }

  //print(diffDist);
  //console.error("heurEstForNonSplit = " + diff);
  //console.log(restoreList);
  restoreBoard(board, restoreList);

  return diff;
}

function heurEstForSplit(board, myPos, enemyPos, depthLvl, depthLimitLvl){
  var isMaxPlayer = (depthLvl % 2 === 0) ? true : false;

  //print(board);
  //console.error("myPos = " + myPos.x + ", " + myPos.y);
  //console.error("enemyPos = " + enemyPos.x + ", " + enemyPos.y);
  var diff = 0;
  var connection = calConnection(board);
  var enemyHeurLen = heurEstLongest(board, enemyPos, connection, true);
  var myHeurLen = heurEstLongest(board, myPos, connection, true);

  if (depthLimitLvl - depthLvl - 1 === 0){
    diff = myHeurLen - enemyHeurLen;
  } else {
    var myArea = getArea(board, myPos);
    if (myArea < enemyHeurLen || (myArea === enemyHeurLen && isMaxPlayer === true)){
      diff = myArea - enemyHeurLen;
      diff -= INF / 4;
      //console.error("myArea = " + myArea + " enemyHeurLen = " + enemyHeurLen + " diff = " + diff);
    } else {
      var enemyArea = getArea(board, enemyPos);
      if (enemyArea < myHeurLen || (enemyArea === myHeurLen && isMaxPlayer === false)){
        diff = myHeurLen - enemyArea;
        diff += INF / 4;
        //console.error("enemyArea = " + enemyArea + " myHeurLen = " + myHeurLen + " diff = " + diff);
      } else {
        var myResult = dlsEstLongest(board, myPos, 0, depthLimitLvl - depthLvl - 1);
        var enemyResult = dlsEstLongest(board, enemyPos, 0, depthLimitLvl - depthLvl - 1);
        diff = myResult[0] - enemyResult[0];
        //console.error("myResult = " + myResult[0] + " enemyResult = " + enemyResult[0] + " diff = " + diff);      
      }  
    }
    
  }

  if (diff === 0) {
    diff = ((isMaxPlayer === true) ? -1 : 1);
  }
  //console.log("heurEstForSplit = " + (diff + (INF / 2) * ((diff > 0) ? 1 : -1)));
  return diff + (INF / 2) * ((diff > 0) ? 1 : -1);  
}

// if positive = good for maxPlayer
function minimax(board, myPos, enemyPos, depthLvl, depthLimitLvl, alpha, beta){
  var isMaxPlayer = (depthLvl % 2 === 0)? true : false;

  if (isSplit(board, myPos, enemyPos)){
    return heurEstForSplit(board, myPos, enemyPos, depthLvl, depthLimitLvl);
  } else if (depthLvl >= depthLimitLvl){
    return heurEstForNonSplit(board, myPos, enemyPos, isMaxPlayer);
  }

  var posOfMovePlayer;
  var value;
  if (isMaxPlayer === true){
    posOfMovePlayer = new Position(myPos.x, myPos.y);
    value = -INF;
  } else {
    posOfMovePlayer = new Position(enemyPos.x , enemyPos.y);
    value = INF;
  }

  var directionToMove = [UNKNOWN_DIRECTION];
  for (var direction = 1; direction <= 4; ++ direction){
    var newPos = moveDirection(posOfMovePlayer, direction);
    if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
      board[newPos.x][newPos.y] = board[posOfMovePlayer.x][posOfMovePlayer.y];
      ++ board[posOfMovePlayer.x][posOfMovePlayer.y];//Make it trails
      if (isMaxPlayer === true){
        var temp = minimax(board, newPos, enemyPos, depthLvl + 1, depthLimitLvl, alpha, beta);
        if (value < temp){
          value = temp;
          directionToMove = [direction];
        } else if (depthLvl === 0 && value === temp){
          directionToMove.push(direction);
        }
        alpha = Math.max(alpha, value);
      } else {
        value = Math.min(value, minimax(board, myPos, newPos, depthLvl + 1, depthLimitLvl, alpha, beta));
        beta = Math.min(beta, value);        
      }
      -- board[posOfMovePlayer.x][posOfMovePlayer.y];//Make it normal
      board[newPos.x][newPos.y] = BLOCK_EMPTY;
      if (beta < alpha){
        break;
      }
    }
  }

  if (depthLvl === 0){
    return [value, directionToMove[(directionToMove.length * Math.random()) >> 0]];
  } else {
    return value;
  }
}
  
var isSplitStatus = false;

function getBestMoveInDepth(board, myPosition, enemyPosition, depthLimitLvl){
  if (isSplitStatus || isSplit(board, myPosition, enemyPosition)){
    isSplitStatus = true;
    var temp = dlsEstLongest(board, myPosition, 0, depthLimitLvl);
    //console.log("longest = " + temp[0]);
    return temp[1];
  } else {
    var temp = minimax(board, myPosition, enemyPosition, 0, depthLimitLvl, -INF, INF);
    //console.error("minimax value = " + temp[0]);      
    return temp[1];
  }
}

var cluster = require('cluster');
var numCPUs = require('os').cpus().length;

function MyTurn(){
// var fs = require('fs');
//    var string = 0;
//    for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
//  for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
//      string += board[idRow][idCol] + " ";
//  }
//  string += "\n";
//    }
//    fs.writeFileSync("test.inp", string);

  if (cluster.isMaster){
    var killStartTime, startTime = new Date().getTime();

    var cmd = UNKNOWN_DIRECTION;

    //Only 1 move possible, nothing to decide
    var suitableDir = [];
    for (var direction = 1; direction <= 4; ++ direction){
      var newPos = moveDirection(myPosition, direction);
      if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
        suitableDir.push(direction);
      }
    }

    cmd = suitableDir[0];

    if (suitableDir.length === 1){
      Command(cmd);
    } else {
      console.log("numCPUs = " + numCPUs);
      console.log("Master start a worker");
      var worker = cluster.fork();

      var boardString = "";
      for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
        for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
          boardString += board[idRow][idCol] + " ";
        }
      }
      boardString += myPosition.x + " " + myPosition.y + " ";
      boardString += enemyPosition.x + " " + enemyPosition.y + " ";

      worker.send({info: boardString});

      worker.on('exit', function(code, signal) {
        if( signal ) {
          console.log("worker was killed by signal: "+signal);
        } else if( code !== 0 ) {
          console.log("worker exited with error code: "+code);
        } else {
          console.log("worker success!");
        }
        var endTime = new Date().getTime();
        console.log("Time : " + (endTime - startTime) + "ms");
        console.log("Kill time: " + (endTime - killStartTime) + "ms");
      });

      worker.on('message', function(msg) {
        if (msg.cmd){
          console.log("Master received a messge: " + msg.cmd);
          cmd = +(msg.cmd); 
        } else if (msg.finish){
          clearTimeout();
          worker.kill();
          Command(cmd);
        }
        
      });

      setTimeout(function(){
        killStartTime = new Date().getTime();
        worker.kill();
        Command(cmd);
      }, MAX_TIME);
    }
  }
} 

if (cluster.isWorker){
  console.log('I am worker #' + cluster.worker.id);

  var startTime = new Date().getTime();
  var depthLimitLvl = START_DEPTH_LEVEL;

  process.on('exit', function(worker, code, signal) {
    var endTime = new Date().getTime();
    console.log("final depth = " + depthLimitLvl);
    console.log("worker " + worker.process.pid + " died after " + (endTime - startTime) + "ms");  
  });

  process.on('message', function(msg){
    console.log("Worker received " + msg.info);

    var number = msg.info.split(' ');
    number = number.map(function(val){
      return +val;
    });

    var temp = "";
    for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
      for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
        board[idRow][idCol] = number[idRow * MAP_SIZE + idCol];
        temp += board[idRow][idCol] + " ";
      }
      temp += "\n";
    }
    myPosition.x = number[MAP_SIZE * MAP_SIZE];
    myPosition.y = number[MAP_SIZE * MAP_SIZE + 1];
    enemyPosition.x = number[MAP_SIZE * MAP_SIZE + 2];
    enemyPosition.y = number[MAP_SIZE * MAP_SIZE + 3];

    var newTime;
    var maxDepth = heurEstLongest(board, myPosition, calConnection(board), true);
    do {      
      var cmd = getBestMoveInDepth(board, myPosition, enemyPosition, depthLimitLvl);

      process.send({cmd: ("" + cmd)});
      ++ depthLimitLvl;
      newTime = new Date().getTime();
      //console.log("depth = " + depthLimitLvl + " time = " + (newTime - lastTime));
    } while (startTime + MAX_TIME > newTime && depthLimitLvl <= maxDepth);
    process.send({finish: "true"});
  }); 
  
}


exports.minimax = minimax;
exports.isSplit = isSplit;
exports.heurEstForNonSplit = heurEstForNonSplit
exports.heurEstForSplit = heurEstForSplit;
exports.heurEstLongest = heurEstLongest;
exports.dlsEstLongest = dlsEstLongest;
exports.inMatrix = inMatrix;
exports.restoreBoard = restoreBoard;
exports.compareVirMove = compareVirMove;
exports.Position = Position;
exports.compareConnection = compareConnection;
exports.calSqrEuclideDist = calSqrEuclideDist;
exports.calMahattanDist = calMahattanDist;
exports.calConnection = calConnection;
exports.calMinDistToAll = calMinDistToAll;
exports.print = print;
exports.moveDirection = moveDirection;
exports.whichDirection = whichDirection;
exports.getArea = getArea;
