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
// ===========================================================
const BLOCK_OCCUPIED = 6;
const BLOCK_OCCUPIED_2 = 7;
const UNKNOWN_DIRECTION = 0;

const MOVE_X = [0, -1, 0, 1, 0];
const MOVE_Y = [0, 0, -1, 0, 1];

const INF = 400;
const START_DEPTH_LEVEL = 12;
const MAX_TIME = 2000;
const UNKNOWN_HEUR_VAL = 0;

const STOP_SEARCH_VAL = 256;

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

function maxAreaBasedOnRedBlack(numRed, numBlack, isRedInit, isRedEnd){
  if (isRedEnd === undefined){
    if (isRedInit && numRed > numBlack){
      return numBlack * 2 + 1;
    } else if (isRedInit === false && numBlack > numRed){
      return numRed * 2 + 1;
    } else {
      return Math.min(numRed, numBlack) * 2;
    }  
  } else {
    if (isRedInit && isRedEnd){
      return Math.min(2 * numRed - 1, 2 * numBlack + 1);
    } else if (isRedInit === false && isRedEnd === false){
      return Math.min(2 * numBlack - 1, 2 * numRed + 1);
    } else {
      return Math.min(numRed, numBlack) * 2;
    }
  }  
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

function print(board, filePath){
  var string = "";
  for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
    for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
      if (filePath === undefined){
        string += "\'";
        if (board[idRow][idCol] < 10 && board[idRow][idCol] >= 0){
         string += "   ";
        } else if (board[idRow][idCol] < 100 && board[idRow][idCol] > -10){
          string += "  ";
        } else if (board[idRow][idCol] < 1000 && board[idRow][idCol] > -100){
          string += " ";
        }
        string += board[idRow][idCol] +  "\'";
      } else {
        string += board[idRow][idCol] +  " ";  
      }
      
    }
    string += "\n";
  }
  string += "\n";
  if (filePath === undefined){
    //console.log(string);
  } else {
    var fs = require('fs');
    fs.appendFileSync(filePath, string);  
  }
}

function findCutVertices(board, myPosition){
  var myId = board[myPosition.x][myPosition.y];
  board[myPosition.x][myPosition.y] = BLOCK_EMPTY;
  
  var label = new Array();
  var lowReach = new Array();
  var cutVertices = new Array();
  var count = 0;
  for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
    label[idRow] = new Array();
    lowReach[idRow] = new Array();
    cutVertices[idRow] = new Array();
    for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
      label[idRow][idCol] = INF;
      cutVertices[idRow][idCol] = 0;
    }
  }

  var dfsCutVertices = function(board, pos){
    ++ count;
    label[pos.x][pos.y] = count;
    lowReach[pos.x][pos.y] = INF;
    cutVertices[pos.x][pos.y] = 0;
    for (var direction = 1; direction <= 4; ++ direction){
      var newPos = moveDirection(pos, direction);
      if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
        if (label[newPos.x][newPos.y] === INF){
          dfsCutVertices(board, newPos);
          if (lowReach[newPos.x][newPos.y] >= label[pos.x][pos.y]){
            cutVertices[pos.x][pos.y] = 1;
          }
          lowReach[pos.x][pos.y] = Math.min(lowReach[pos.x][pos.y], lowReach[newPos.x][newPos.y]);
        } else {
          lowReach[pos.x][pos.y] = Math.min(lowReach[pos.x][pos.y], label[newPos.x][newPos.y]);
        }          
      }
    }    
  }


  dfsCutVertices(board, myPosition);  

  board[myPosition.x][myPosition.y] = myId;

  return cutVertices;
}


function getUpperLongest(board, myPosition){
  var myID = board[myPosition.x][myPosition.y];
  board[myPosition.x][myPosition.y] = BLOCK_EMPTY;

  //print(board);
  
  var label = new Array();
  var lowReach = new Array();
  var cutVertices = new Array();
  var count = 0;
  for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
    label[idRow] = new Array();
    lowReach[idRow] = new Array();
    cutVertices[idRow] = new Array();
    for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
      label[idRow][idCol] = INF;
      cutVertices[idRow][idCol] = 0;
    }
  }

  function dfsUpperLongest(board, pos){
    ++ count;
    label[pos.x][pos.y] = count;
    lowReach[pos.x][pos.y] = INF;
    cutVertices[pos.x][pos.y] = 0;

    var numRed = 0, numBlack = 0;
    var isRed = ((pos.x + pos.y) % 2 === 0) ? true : false;
    if (isRed){
      numRed = 1; numBlack = 0;
    } else {
      numRed = 0; numBlack = 1;
    }
    var result = new Array();
    result.push(isRed);
    result.push(numRed);
    result.push(numBlack);
    result.push(new Array());

    var compareInfo = [];
    for (var direction = 1; direction <= 4; ++ direction){
      var newPos = moveDirection(pos, direction);
      if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
        if (label[newPos.x][newPos.y] === INF){
          compareInfo.push([direction, dfsUpperLongest(board, newPos)]);
          if (lowReach[newPos.x][newPos.y] >= label[pos.x][pos.y]){
            cutVertices[pos.x][pos.y] = 1;
          }
          lowReach[pos.x][pos.y] = Math.min(lowReach[pos.x][pos.y], lowReach[newPos.x][newPos.y]);
        } else {
          lowReach[pos.x][pos.y] = Math.min(lowReach[pos.x][pos.y], label[newPos.x][newPos.y]);
        }
      }
    }

    if (cutVertices[pos.x][pos.y] === 1){
      result[3].push([isRed, 1]);
    }
    //console.log("label " + label[pos.x][pos.y]);

    for (var id = 0; id < compareInfo.length; ++ id){
      var newPos = moveDirection(pos, compareInfo[id][0]);
      if (cutVertices[pos.x][pos.y] === 0 || lowReach[newPos.x][newPos.y] < label[pos.x][pos.y]){        
        result[1] += compareInfo[id][1][1];
        result[2] += compareInfo[id][1][2];
        result[3] = result[3].concat(compareInfo[id][1][3]);  
      } else {// when pos is cutVertices
        var numRedTemp = ((isRed) ? 1 : 0) + compareInfo[id][1][1];
        var numBlackTemp = ((isRed === false) ? 1 : 0) + compareInfo[id][1][2];
        result[3][0][1] = Math.max(maxAreaBasedOnRedBlack(numRedTemp, numBlackTemp, isRed), result[3][0][1]);

        var nextCuts = compareInfo[id][1][3];
        //console.log("nextCuts = " );
        //console.log(nextCuts);
        for (var idCut = 0; idCut < nextCuts.length; ++ idCut){          
          var tempArea = maxAreaBasedOnRedBlack(numRedTemp, numBlackTemp, isRed, nextCuts[idCut][0]) - 1 + nextCuts[idCut][1];
          result[3][0][1] = Math.max(tempArea, result[3][0][1]);
        }
      }
    }

    //console.log(result);

    return result;
  }

  var tempResult = dfsUpperLongest(board, myPosition);
  //print(label);
  board[myPosition.x][myPosition.y] = myID;

  if (tempResult[3][0] === undefined){
    return 1;
  }
  return tempResult[3][0][1];
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
      if (inMatrix(newPos) && dist[newPos.x][newPos.y] === INF){
        if (board[newPos.x][newPos.y] === BLOCK_EMPTY){//empty
          dist[newPos.x][newPos.y] = dist[curPos.x][curPos.y] + 1;
          queue.push(newPos);
        } else if (board[newPos.x][newPos.y] === BLOCK_PLAYER_2 + BLOCK_PLAYER_1 - board[pos.x][pos.y]){//enemy
          dist[newPos.x][newPos.y] = dist[curPos.x][curPos.y] + 1;
        }
      }
    }
  }

  return dist;
}

function compareMinSecond(a, b){
  if (a[1] < b[1]){
    return -1;
  } else if (a[1] > b[1]){
    return 1;
  } else {
    return 0;
   }
}

function compareMaxSecond(a, b){
  if (a[1] > b[1]){
    return -1;
  } else if (a[1] < b[1]){
    return 1;
  } else {
    return 0;
   }
}

function compareMinSecondAndThird(a, b){
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

function compareMaxAll(a, b){
  id = 1;
  while (true){
    if (a[id] !== undefined && b[id] != undefined){
      if (a[id] > b[id]){
        return -1;
      } else if (a[id] < b[id]){
        return 1;
      }
    } else if (a[id] !== undefined && b[id] === undefined){
      return -1;
    } else if (b[id] !== undefined && a[id] === undefined){
      return 1;
    } else {
      return 0;
    }
    ++ id;
  }
}

function compareMinAll(a, b){
  id = 1;
  while (true){
    if (a[id] !== undefined && b[id] != undefined){
      if (a[id] < b[id]){
        return -1;
      } else if (a[id] > b[id]){
        return 1;
      }
    } else if (a[id] !== undefined && b[id] === undefined){
      return -1;
    } else if (b[id] !== undefined && a[id] === undefined){
      return 1;
    } else {
      return 0;
    }
    ++ id;
  }
}

///////////////////////////////////////////////FindMaxLengthInStatic////////////////////////////////////////////////
//NOTE: O(n^2)
function heurEstLongest2(board, myPos, isFirst, restoreList){
  var myID;
  if (isFirst === true){
    myID = board[myPos.x][myPos.y];
    board[myPos.x][myPos.y] = BLOCK_EMPTY;
    restoreList = new Array();
  }

  board[myPos.x][myPos.y] = BLOCK_OCCUPIED;
  restoreList.push(myPos);

  var nextMoves = [];
  for (var direction = 1; direction <= 4; ++ direction){
    var newPos = moveDirection(myPos, direction);
    if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
      nextMoves.push([direction, getUpperLongest(board, newPos)]);
    }
  }
  
  nextMoves.sort(compareMaxAll);
  if (nextMoves.length > 1 && nextMoves[1][1] === nextMoves[0][1]){
    for (var idMove = 0; idMove < nextMoves.length; ++ idMove){
      if (nextMoves[idMove][1] === nextMoves[0][1]){
        var newPos = moveDirection(myPos, nextMoves[idMove][0]);
        nextMoves[idMove][2] = heurEstLongest(board, newPos, calConnection(board), true);
        if (nextMoves[idMove][2] === nextMoves[idMove][1]){
          break;
        }
      } else {
        break;
      }
    }
    nextMoves.sort(compareMaxAll);
    if (nextMoves[0][1] > nextMoves[0][2]){
      for (var idMove = 0; idMove < nextMoves.length; ++ idMove){
        if (nextMoves[idMove][1] === nextMoves[0][1]){
          var newPos = moveDirection(myPos, nextMoves[idMove][0]);
          nextMoves[idMove][2] = heurEstLongest2(board, newPos, true);
          if (nextMoves[idMove][2] === nextMoves[idMove][1]){
            break;
          }
        } else {
          break;
        }
      }
      nextMoves.sort(compareMaxAll);
    }
  }
  //console.log(nextMoves);
  
  var result = 1;
  if (nextMoves.length >= 1){
    var direction = nextMoves[0][0];
    var newPos = moveDirection(myPos, direction);
    if (nextMoves[0][2] !== undefined){
      result = nextMoves[0][2] + 1;
    } else {
      result = heurEstLongest2(board, newPos, false, restoreList) + 1;
    }  
  }  

  if (isFirst === true){
    //console.error(restoreList);
    //print(board);
    restoreBoard(board, restoreList);
    board[myPos.x][myPos.y] = myID;
  }
  
  return result;
}

//NOTE: O(n)
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
  
  nextMoves.sort(compareMinSecond);

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

function dlsEstLongest(board, myPos, depthLvl, depthLimitLvl, depthToCut){
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
      var temp = dlsEstLongest(board, newPos, depthLvl + 1, depthLimitLvl, (depthToCut !== undefined) ? (depthToCut - 1) : undefined);
      board[newPos.x][newPos.y] = BLOCK_EMPTY;
      if (temp + 1 > result){
        result = temp + 1;
        directionToMove = direction;
        if (depthToCut !== undefined && result >= depthToCut){
          //console.log("cut at depth " + depthLvl);
          break;
        }
      }
    }
  }
  -- board[myPos.x][myPos.y];//make it not trail

  if (depthLvl === 0){
    return [result, directionToMove];
  } else {
    return result;
  }
}

//////////////////////////////////////////////FindMaxLengthInDynamic////////////////////////////////////////////////

//TODO
function heurEstForNonSplit(board, myPos, enemyPos, isMaxPlayer){
  var distFromMe = calMinDistToAll(board, myPos);
  var distFromEnemy = calMinDistToAll(board, enemyPos);

  var myVoronoiBoard = new Array(), enemyVoronoiBoard = new Array();
  for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
    myVoronoiBoard[idRow] = new Array();
    enemyVoronoiBoard[idRow] = new Array();
    for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
      if (distFromMe[idRow][idCol] === distFromEnemy[idRow][idCol] && distFromMe[idRow][idCol] === INF){
        myVoronoiBoard[idRow][idCol] = BLOCK_OBSTACLE;
        enemyVoronoiBoard[idRow][idCol] = BLOCK_OBSTACLE;
      } else if (distFromMe[idRow][idCol] <= distFromEnemy[idRow][idCol]){
        enemyVoronoiBoard[idRow][idCol] = BLOCK_OBSTACLE;
        myVoronoiBoard[idRow][idCol] = BLOCK_EMPTY;
      } else if (distFromMe[idRow][idCol] > distFromEnemy[idRow][idCol]){
        myVoronoiBoard[idRow][idCol] = BLOCK_OBSTACLE;
        enemyVoronoiBoard[idRow][idCol] = BLOCK_EMPTY;
      }
    }
  }
  var tmp1 = getUpperLongest(myVoronoiBoard, myPos);
  var tmp2 = getUpperLongest(enemyVoronoiBoard, enemyPos);

  return tmp1 - tmp2;
}

function heurEstForSplit(board, myPos, enemyPos, depthLvl, depthLimitLvl){
  var isMaxPlayer = (depthLvl % 2 === 0) ? true : false;

  var diff = 0;
  var connection = calConnection(board);
  var enemyHeurLen = heurEstLongest(board, enemyPos, connection, true);
  var myArea = getUpperLongest(board, myPos);

  if (myArea < enemyHeurLen || (myArea === enemyHeurLen && isMaxPlayer === true)){
    diff = myArea - enemyHeurLen;
    diff -= INF / 4;
    //console.error("myArea = " + myArea + " enemyHeurLen = " + enemyHeurLen + " diff = " + diff);
  } else {
    var myHeurLen = heurEstLongest(board, myPos, connection, true);
    var enemyArea = getUpperLongest(board, enemyPos);
    if (enemyArea < myHeurLen || (enemyArea === myHeurLen && isMaxPlayer === false)){
      diff = myHeurLen - enemyArea;
      diff += INF / 4;
      //console.error("enemyArea = " + enemyArea + " myHeurLen = " + myHeurLen + " diff = " + diff);
    } else if (myHeurLen === myArea && enemyHeurLen === enemyArea){
      diff = myHeurLen - enemyHeurLen;
      diff += (diff > 0 || (diff == 0 && isMaxPlayer === false)) ? INF / 4 : -INF / 4;
    } else {//if (depthLimitLvl - depthLvl - 1 === 0){      
      myHeurLen = heurEstLongest2(board, myPos, true);
      //myHeurLen = Math.max(myHeurLen, dlsEstLongest(board, myPos, 0, depthLimitLvl - depthLvl - 1, Math.min(myArea, enemyArea + 1)));
      if (enemyArea < myHeurLen || (enemyArea === myHeurLen && isMaxPlayer === false)){
        diff = myHeurLen - enemyArea;
        diff += INF / 4;
        //console.error("enemyArea = " + enemyArea + " myHeurLen2 = " + myHeurLen + " diff = " + diff);
      } else {
        //enemyHeurLen = Math.max(enemyHeurLen, dlsEstLongest(board, enemyPos, 0, depthLimitLvl - depthLvl - 1, enemyArea));
        enemyHeurLen = heurEstLongest2(board, enemyPos, true);
        diff = myHeurLen - enemyHeurLen;
        if (myArea < enemyHeurLen || (myArea === enemyHeurLen && isMaxPlayer === true)){
          diff -= INF / 4;
          //console.error("myArea = " + myArea + " enemyHeurLen2 = " + enemyHeurLen + " diff = " + diff);
        } else if (myHeurLen === myArea && enemyHeurLen === enemyArea){
          diff += (diff > 0 || (diff == 0 && isMaxPlayer === false)) ? INF / 4 : -INF / 4;
        }
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
    //print(board);
    var temp = heurEstForNonSplit(board, myPos, enemyPos, isMaxPlayer);
    //console.log("depth = " + depthLvl + " value = " + temp);
    return temp
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
      if (beta < alpha || (isMaxPlayer && value >= STOP_SEARCH_VAL)){
        break;
      }
    }
  }

  //console.log("depth = " + depthLvl + " value = " + value);

  if (depthLvl === 0){
    return [value, directionToMove[(directionToMove.length * Math.random()) >> 0]];
  } else {
    return value;
  }
}

var isSplitStatus = false;
var cmd = UNKNOWN_DIRECTION
var lastDepthLimitLvl = 0;
var winForSure = false;

function getBestMoveInDepth(board, myPosition, enemyPosition, depthLimitLvl){
  if (isSplitStatus || isSplit(board, myPosition, enemyPosition)){
    if (isSplitStatus === false){
      isSplitStatus = true;
      depthLimitLvl = START_DEPTH_LEVEL;
      //print(board);
    }
    var upper = getUpperLongest(board, myPosition);
    var enemyUpper = getUpperLongest(board, enemyPosition);
    //TODO enable below line
    var temp = dlsEstLongest(board, myPosition, 0, depthLimitLvl, Math.min(upper, enemyUpper + 1));
    //console.log("upper = " + upper);
    //console.log("longest = " + temp[0]);
    //console.log("upper = " + upper);
    //TODO disable below lines
    /*var temp = dlsEstLongest(board, myPosition, 0, depthLimitLvl);
    var fs = require('fs');
    if (upper < temp[0]){
      print(board, 'boards.txt');      
      fs.appendFileSync('boards.txt', "upper2 failes \n");
    }
    var heurEst = heurEstLongest2(board, myPosition,true);
    if (heurEst < temp[0]){
      print(board, 'boards.txt');
      fs.appendFileSync('boards.txt', "heurEst22 failes \n");
    }*/
    //////////////////////////////////////
    return temp;
  } else {
    var temp = minimax(board, myPosition, enemyPosition, 0, depthLimitLvl, -INF, INF);
    //console.error("minimax value = " + temp[0]);
    //TODO
    if (temp[0] >= STOP_SEARCH_VAL){
      winForSure = true;
    }
    return temp;
  }
}

function MyTurn(){
  var startTime = new Date().getTime();

  //Only 1 move possible, nothing to decide
  var suitableDir = []
  for (var direction = 1; direction <= 4; ++ direction){
    var newPos = moveDirection(myPosition, direction);
    if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
      suitableDir.push(direction);
    }
  }

  cmd = suitableDir[0];

  if (suitableDir.length !== 1){
    var depthLimitLvl = Math.max(START_DEPTH_LEVEL, (isSplitStatus) ? (lastDepthLimitLvl - 1) :  (lastDepthLimitLvl - 2));
    winForSure = false;

    var lastTime, newTime = new Date().getTime();
    var maxDepth = heurEstLongest(board, myPosition, calConnection(board), true);
    do {      
      lastTime = newTime;
      cmd = getBestMoveInDepth(board, myPosition, enemyPosition, depthLimitLvl);
      ++ depthLimitLvl;
      newTime = new Date().getTime();
      //console.log("depth = " + (depthLimitLvl - 1)+ " time = " + (newTime - lastTime));
    } while (startTime + MAX_TIME - newTime > 3 * (newTime - lastTime) && depthLimitLvl <= maxDepth && winForSure === false);

    if (! isSplitStatus && Math.abs(cmd[0]) >= 200){
      //print(board, 'learn.txt');  
      var distFromMe = calMinDistToAll(board, myPosition);
      var distFromEnemy = calMinDistToAll(board, enemyPosition);
      var voronoiBoard = new Array();
      var myVoronoiBoard = new Array(), enemyVoronoiBoard = new Array();
      for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
        voronoiBoard[idRow] = new Array();
        myVoronoiBoard[idRow] = new Array();
        enemyVoronoiBoard[idRow] = new Array();
        for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
          if (distFromMe[idRow][idCol] === distFromEnemy[idRow][idCol] && distFromMe[idRow][idCol] === INF){
            voronoiBoard[idRow][idCol] = BLOCK_OBSTACLE;
            myVoronoiBoard[idRow][idCol] = BLOCK_OBSTACLE;
            enemyVoronoiBoard[idRow][idCol] = BLOCK_OBSTACLE;
          } else if (distFromMe[idRow][idCol] === 0 || distFromEnemy[idRow][idCol] === 0){
            voronoiBoard[idRow][idCol] = board[idRow][idCol];
            myVoronoiBoard[idRow][idCol] = board[idRow][idCol];
            enemyVoronoiBoard[idRow][idCol] = board[idRow][idCol];
          } else if (distFromMe[idRow][idCol] <= distFromEnemy[idRow][idCol]){
            voronoiBoard[idRow][idCol] = 7;
            enemyVoronoiBoard[idRow][idCol] = BLOCK_OBSTACLE;
            myVoronoiBoard[idRow][idCol] = BLOCK_EMPTY;
          } else if (distFromMe[idRow][idCol] > distFromEnemy[idRow][idCol]){
            voronoiBoard[idRow][idCol] = 9;
            myVoronoiBoard[idRow][idCol] = BLOCK_OBSTACLE;
            enemyVoronoiBoard[idRow][idCol] = BLOCK_EMPTY;
          }
        }
      }
      var tmp1 = findCutVertices(myVoronoiBoard, myPosition);
      //print(myVoronoiBoard, 'learn.txt');
      var tmp2 = findCutVertices(enemyVoronoiBoard, enemyPosition);
      //print(enemyVoronoiBoard, 'learn.txt');
      for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
        for (var idCol= 0; idCol < MAP_SIZE; ++ idCol){
          if (board[idRow][idCol] === BLOCK_EMPTY){
            if (tmp1[idRow][idCol] === 1){
              voronoiBoard[idRow][idCol] = 6;
            } else if (tmp2[idRow][idCol] === 1){
              voronoiBoard[idRow][idCol] = 8; 
            }
          }
        }
      }

      print(voronoiBoard, 'learn.txt');

      var fs = require('fs');
      fs.appendFileSync('learn.txt', "heurEstForNonSplit = " + heurEstForNonSplit(board, myPosition, enemyPosition, true) + "\n");
      fs.appendFileSync('learn.txt', "minmax value = " + cmd[0] + "\n");
    }

    lastDepthLimitLvl = depthLimitLvl - 1;
    //console.log("final depth = " + lastDepthLimitLvl);
    //console.error("time = " + (new Date().getTime() - startTime) + "ms");

    cmd = cmd[1];
  } else {
    lastDepthLimitLvl -= 2;
  }

  Command(cmd);
} 

exports.minimax = minimax;
exports.isSplit = isSplit;
exports.heurEstForNonSplit = heurEstForNonSplit
exports.heurEstForSplit = heurEstForSplit;
exports.heurEstLongest = heurEstLongest;
exports.heurEstLongest2 = heurEstLongest2;
exports.dlsEstLongest = dlsEstLongest;
exports.inMatrix = inMatrix;
exports.restoreBoard = restoreBoard;
exports.compareMinSecondAndThird = compareMinSecondAndThird;
exports.Position = Position;
exports.compareMinSecond = compareMinSecond;
exports.calConnection = calConnection;
exports.calMinDistToAll = calMinDistToAll;
exports.print = print;
exports.moveDirection = moveDirection;
exports.whichDirection = whichDirection;
exports.getUpperLongest = getUpperLongest;
exports.getBestMoveInDepth = getBestMoveInDepth;
