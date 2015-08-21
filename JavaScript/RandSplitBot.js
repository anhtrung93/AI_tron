// ==================== HOW TO RUN THIS =====================
// Call:
// "node Client.js -h [host] -p [port] -k [key]"
//
// If no argument given, it'll be 127.0.0.1:3011
// key is a secret string that authenticate the bot identity
// it is not required when testing
// ===========================================================

// Get the host and port from argurment
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
// Player 1 have value 0, and player 2 have value 1, but you probably
// don't care about that anyway.
//
// That's pretty much about it. Now, let's start coding.
// ===========================================================

var fs = require('fs');

const BLOCK_OCCUPIED = 6;

const MOVE_X = [0, -1, 0, 1, 0];
const MOVE_Y = [0, 0, -1, 0, 1];

const INF = 400;

///////////////////////////////////////////////Support Function////////////////////////////////////////////////
function inMatrix(pos){
  if (pos.x >= 0 && pos.x < MAP_SIZE){
    if (pos.y >= 0 && pos.y < MAP_SIZE){
      return true;
    }
  }
  return false;
}

function restoreBoard(board){
  for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
    for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
      if (board[idRow][idCol] === BLOCK_OCCUPIED){
        board[idRow][idCol] = BLOCK_EMPTY;
      }
    }
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
  return 0;//UNKNOWN_DIRECTION
}

function isSplit(board, myPos, enemyPos){
  var myID = board[myPos.x][myPos.y];
  var stack = [];
  for (var direction = 1; direction <= 4; ++ direction){
    var newPos = moveDirection(myPos, direction);
    if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
      stack.push(newPos);
    }
  }
  var result = true;
  while (stack.length > 0 && result !== false){
    var pos = stack.pop();

    board[pos.x][pos.y] = BLOCK_OCCUPIED;
    for (var direction = 1; direction <= 4; ++ direction){
      var newPos = moveDirection(pos, direction);
      if (inMatrix(newPos)){
        if (newPos.x === enemyPos.x && newPos.y === enemyPos.y){
          result = false;
          break;
        } else if (board[newPos.x][newPos.y] === BLOCK_EMPTY){
          stack.push(newPos);
        }
      }
    }
  }
  restoreBoard(board);

  board[myPos.x][myPos.y] = myID;  
  return result;
};

function print(board){
  var string = "";
  for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
    for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
      string += "\'";
      if (board[idRow][idCol] < 10){
       string += "  ";
      } else if (board[idRow][idCol] < 100){
        string += " ";
      }
      string += board[idRow][idCol] +  "\'";
    }
    string += "\n";
  }
  console.log(string);
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

function compareConnection(a, b){
  if (a[1] < b[1]){
    return -1;
  } else if (a[1] > b[1]){
    return 1;
  } else {
    return 0;
   }
}

function compareDiffDist(a, b){
  if (a[1] < b[1]){
    return -1;
  } else if (a[1] > b[1]){
    return 1;
  } else {
    if (Math.random() < 0.5){
      return -1;
    } else {
      return 1;
    }
  } 
}


///////////////////////////////////////////////FindMaxLengthInStatic////////////////////////////////////////////////

function heurEstLongest(board, myPos, connection){
  var myID = board[myPos.x][myPos.y];

  var nextMoves = [];
  for (var direction = 1; direction <= 4; ++ direction){
    var newPos = moveDirection(myPos, direction);
    if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
      -- connection[newPos.x][newPos.y];
      nextMoves.push([direction, connection[newPos.x][newPos.y]]);
    }
  }
  
  nextMoves.sort(compareConnection);

//  console.log(nextMoves);
  
  board[myPos.x][myPos.y] = BLOCK_OCCUPIED;
  
  var result = 0;
  for (var idMove = 0; idMove < nextMoves.length; ++ idMove){
    var direction = nextMoves[idMove][0];
    
    var newPos = moveDirection(myPos, direction);
    if (board[newPos.x][newPos.y] === BLOCK_EMPTY){
      result = Math.max(result, heurEstLongest(board, newPos, connection) + 1);
    }  
  }
  
  return result;
}

const MAX_DEPTH_LEVEL = 10;

function dlsEstLongest(board, myPos, depthLvl){
  if (depthLvl >= MAX_DEPTH_LEVEL){
    var myID = board[myPos.x][myPos.y];
    board[myPos.x][myPos.y] = BLOCK_EMPTY;
    var temp = heurEstLongest(board, myPos, calConnection(board));
    restoreBoard(board);
    board[myPos.x][myPos.y] = myID;
    return temp;
  }
  
  var myID = board[myPos.x][myPos.y];
  ++ board[myPos.x][myPos.y];

  var result = 0;
  var directionToMove = 0;
  for (var direction = 1; direction <= 4; ++ direction){
    var newPos = moveDirection(myPos, direction);
    if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
      board[newPos.x][newPos.y] = myID;
      var temp = dlsEstLongest(board, newPos, depthLvl + 1);
      if (temp + 1 > result){
        result = temp + 1;
        directionToMove = direction;
      }
      board[newPos.x][newPos.y] = BLOCK_EMPTY;
    }
  }
  -- board[myPos.x][myPos.y];

  if (depthLvl === 0){
    return [result, directionToMove];
  } else {
    return result;
  }
}


function findMaxAfterSplit(board, pos){ 
  var tempPos = new Position(0, 0);
  var longestEst = dlsEstLongest(board, pos, 0);
  console.log("estimate longest = " + longestEst[0]);
  console.log("move = " + longestEst[1]);
  return longestEst[1];
}

///////////////////////////////////////////////FindMaxLengthInDynamic////////////////////////////////////////////////

function calMinDist(board, pos, dist){
  for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
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
}

function moveBeforeSplit(board, myPos, enemyPos){
  var dist1 = new Array();
  var dist2 = new Array();
  for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
    dist1[idRow] = new Array();
    dist2[idRow] = new Array();
  }
  //print(board);
  calMinDist(board, myPos, dist1);
  calMinDist(board, enemyPos, dist2);
    
  var string = "";
  for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
    for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
      string += "\'";
      var val = dist2[idRow][idCol] - dist1[idRow][idCol];
      if (val < 10 && val >= 0){
        string += "  ";
      } else if (val < INF / 2 && val > -10){
        string += " ";
      }
      string += val +  "\'";
    }
    string += "\n";
  }
  console.log(string);    

  var nextMoves = [];    
  for (var direction = 1; direction <= 4; ++ direction){
    var newPos = moveDirection(myPos, direction);

    if (inMatrix(newPos)){
      if (board[newPos.x][newPos.y] === BLOCK_EMPTY){
        var diffDist = dist2[newPos.x][newPos.y] - dist1[newPos.x][newPos.y];
        nextMoves.push([direction, diffDist]);
      }      
    }  
  }

  nextMoves.sort(compareDiffDist);

  console.log(nextMoves);

  if (nextMoves[nextMoves.length - 1][1] >= 100){
    var myID = board[myPos.x][myPos.y];
    board[myPos.x][myPos.y] = BLOCK_OBSTACLE;
    var newPos = moveDirection(myPos, nextMoves[nextMoves.length - 1][0]);
    var myResult = dlsEstLongest(board, newPos, 0);
    var enemyResult = dlsEstLongest(board, enemyPos, 0);
    board[myPos.x][myPos.y] = myID;
    console.log("myResult = " + myResult[0] + " enemyResult = " + enemyResult[0]);
    if (myResult >= enemyResult){      
      return nextMoves[nextMoves.length - 1][0];
    }
  }
    
  return nextMoves[0][0];
}

var fs = require('fs');
var isSplitStatus = false;

function MyTurn(){
//    var string = 0;
//    for (var idRow = 0; idRow < MAP_SIZE; ++ idRow){
//  for (var idCol = 0; idCol < MAP_SIZE; ++ idCol){
//      string += board[idRow][idCol] + " ";
//  }
//  string += "\n";
//    }
//    fs.writeFileSync("test.inp", string);

  var startTime = new Date().getTime();

  //Only 1 move possible, nothing to decide
  var suitableDir = [];
  for (var direction = 1; direction <= 4; ++ direction){
    var newPos = moveDirection(myPosition, direction);
    if (inMatrix(newPos) && board[newPos.x][newPos.y] === BLOCK_EMPTY){
      suitableDir.push(direction);
    }
  }
  if (suitableDir.length !== 1){
    if (isSplitStatus || isSplit(board, myPosition, enemyPosition)){
      isSplitStatus = true;
      cmd = findMaxAfterSplit(board, myPosition);
    } else {
      cmd = moveBeforeSplit(board, myPosition, enemyPosition);
    }
  } else {
    cmd = suitableDir[0];
  }  
    
  console.log("time = " + (new Date().getTime() - startTime) + "ms");
  Command(cmd);
}
