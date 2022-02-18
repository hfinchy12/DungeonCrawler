#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::string;

/**
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */


char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    ifstream fin(fileName);
    if(!fin.is_open()){
        //FILE NOT OPEN
        cout << "FILE NOT OPEN" << endl;
        return nullptr;
    }
    // assuming correct file
    fin >> maxRow;
    if(fin.fail()){return nullptr;}
    fin >> maxCol;
    if(fin.fail()){return nullptr;}

    int totalSpots = maxRow * maxCol;
    if(totalSpots <= 1){
        return nullptr;
    }
    char** map = createMap(maxRow, maxCol);
    char spot;

    fin >> player.row;
    if(fin.fail()){return nullptr;}
    fin >> player.col;
    if(fin.fail()) {return nullptr;}

    if(player.col >= maxCol || player.row >= maxRow){
        return nullptr;
    } else if(player.col < 0 || player.row < 0){
        return nullptr;
    }

    for(int row = 0; row < maxRow; row++){
        for(int col = 0; col < maxCol; col++){
            fin >> spot;
            if(fin.fail()){
                return nullptr;
            }
            if(row == player.row && col == player.col){
                map[row][col] = TILE_PLAYER;
            } else if(spot == TILE_TREASURE){
                map[row][col] = TILE_TREASURE;    
            } else if(spot == TILE_PILLAR){
                map[row][col] = TILE_PILLAR;    
            } else if(spot == TILE_OPEN){
                map[row][col] = TILE_OPEN;    
            } else if(spot == TILE_AMULET){
                map[row][col] = TILE_AMULET;    
            } else if(spot == TILE_MONSTER){
                map[row][col] = TILE_MONSTER;    
            } else if(spot == TILE_DOOR){
                map[row][col] = TILE_DOOR;    
            } else if(spot == TILE_EXIT){
                map[row][col] = TILE_EXIT;    
            } else{
                // error, invalid map character
                return nullptr;
            }
        }
    }
    fin >> spot;
    if(!fin.eof()){
        // error
        return nullptr;
    }

    bool hasDoor = false;
    bool hasExit = false;
    for(int row = 0; row < maxRow; row++){ // checks for correct number of doors
        for(int col = 0; col < maxCol; col++){
                if(map[row][col] == TILE_EXIT){
                    if(hasExit == false){
                        hasExit = true;
                    } 
                } else if(map[row][col] == TILE_DOOR){
                    if(hasDoor == false){
                        hasDoor = true;
                    } 
                }
        }
    }
    if(hasDoor == false && hasExit == false){
        return nullptr;
    }
    return map;
}

/**
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    // constants for user's keyboard inputs
    // const char INPUT_QUIT     = 'q';    // quit command
    // const char INPUT_STAY     = 'e';    // no movement
    // const char MOVE_UP        = 'w';    // up movement
    // const char MOVE_LEFT      = 'a';    // left movement
    // const char MOVE_DOWN      = 's';    // down movement
    // const char MOVE_RIGHT     = 'd';    // right movement
    if(input == MOVE_UP){
        nextRow -= 1;
    } else if (input == MOVE_DOWN){
        nextRow += 1;
    } else if (input == MOVE_LEFT){
        nextCol -= 1;
    } else if(input == MOVE_RIGHT){
        nextCol += 1;
    } else if(input == INPUT_STAY){
        // nothing
    }
}

/**
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
    if(maxRow <= 0 || maxCol <= 0){
        return nullptr;
    } else if(maxRow > (INT32_MAX / maxCol)){
        return nullptr;
    } else if(maxCol > (INT32_MAX / maxRow)){
        return nullptr;
    }

    char** map = new char*[maxRow];
    for(int row = 0; row < maxRow; ++row){
        map[row] = new char[maxCol];
    }
    for(int row = 0; row < maxRow; row++){
        for(int col = 0; col < maxCol; col++){
            map[row][col] = TILE_OPEN;
        }
    }
    return map;
}

/**
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    if(map != nullptr){
        for(int i = 0; i < maxRow; ++i){
            delete[] map[i];
        }
        delete[] map;
    }
    maxRow = 0;
    map = nullptr;
}

/**
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    int originalRow = maxRow;
    int originalCol = maxCol;
    if(maxRow <= 0 || maxCol <= 0){
        return nullptr;
    } 
    if (map == nullptr){
        return nullptr;
    }

    maxRow *= 2;
    maxCol *= 2;
    
    if(maxRow > (INT32_MAX / maxCol)){
        deleteMap(map, originalRow);
        return nullptr;
    } 

    char** newMap = createMap(maxRow, maxCol); // this will be an empty map of twice the size

    for(int row = 0; row < originalRow; ++row){ 
        for(int col = 0; col < originalCol; ++col){ 
            newMap[row][col] = map[row][col];
        }
    }
    for(int row = 0; row < originalRow; ++row){ 
        for(int col = 0; col < originalCol; ++col){ 
            newMap[row+originalRow][col] = map[row][col];
            if(map[row][col] == TILE_PLAYER){
                newMap[row+originalRow][col] = TILE_OPEN;
            }
        }
    }
    for(int row = 0; row < originalRow; ++row){ // top right
        for(int col = 0; col < originalCol; ++col){ 
            newMap[row][col+originalCol] = map[row][col];
            if(map[row][col] == TILE_PLAYER){
                newMap[row][col+originalCol] = TILE_OPEN;
            }
        }
    }
    for(int row = 0; row < originalRow; ++row){ // bottom right
        for(int col = 0; col < originalCol; ++col){ 
            newMap[row+originalRow][col+originalCol] = map[row][col];
            if(map[row][col] == TILE_PLAYER){
                newMap[row+originalRow][col+originalCol] = TILE_OPEN;
            }
        }
    }

    deleteMap(map, originalRow);

    return newMap;
}

/**
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    // want to check if next move is out of bounds, then what tile they are moving onto
    // constants for movement status flags :
    // const int STATUS_STAY     = 0;      // flag indicating player has stayed still
    // const int STATUS_MOVE     = 1;      // flag indicating player has moved in a direction
    // const int STATUS_TREASURE = 2;      // flag indicating player has stepped onto the treasure
    // const int STATUS_AMULET   = 3;      // flag indicating player has stepped onto an amulet
    // const int STATUS_LEAVE    = 4;      // flag indicating player has left the current room
    // const int STATUS_ESCAPE   = 5;      // flag indicating player has gone through the dungeon exit
    int origRow = player.row;
    int origCol = player.col;

    if(nextRow >= maxRow || nextCol >= maxCol){
        return STATUS_STAY;
    } else if (nextRow < 0 || nextCol < 0){
        return STATUS_STAY;
    } else if(map[nextRow][nextCol] == TILE_MONSTER || map[nextRow][nextCol] == TILE_PILLAR){
        return STATUS_STAY;
    } else if(map[nextRow][nextCol] == TILE_EXIT && player.treasure == 0){
        return STATUS_STAY;
    } 
    if(map[nextRow][nextCol] == TILE_EXIT && player.treasure != 0){
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        map[origRow][origCol] = TILE_OPEN;
        return STATUS_ESCAPE;
    } else if(map[nextRow][nextCol] == TILE_DOOR){
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        map[origRow][origCol] = TILE_OPEN;
        return STATUS_LEAVE;
    }
    if(map[nextRow][nextCol] == TILE_TREASURE){
        player.treasure += 1;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        map[origRow][origCol] = TILE_OPEN;
        return STATUS_TREASURE;
    } else if(map[nextRow][nextCol] == TILE_AMULET){
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        map[origRow][origCol] = TILE_OPEN;
        return STATUS_AMULET;
    }
    if(map[nextRow][nextCol] == TILE_OPEN){
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        map[origRow][origCol] = TILE_OPEN;
        return STATUS_MOVE;
    }


    return 0;
}

/**
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    int pRow = player.row;
    int pCol = player.col;

    bool eaten = false;

    int upLength = pRow; 
    int downLength = (maxRow - 1) - pRow;
    int rightLength = (maxCol - 1) - pCol;
    int leftLength = pCol;
    // check above, the below, then right, then left

    // above
    for(int i = 1; i <= upLength; ++i){
        if(map[pRow-i][pCol] == TILE_MONSTER){
            // monster found
            map[pRow-i][pCol] = TILE_OPEN;
            map[pRow-i +1][pCol] = TILE_MONSTER;
            if(i == 1){
                eaten = true;
            }
        } else if(map[pRow-i][pCol] == TILE_PILLAR){
            break;
        }
    }

    // below
    for(int i = 1; i <= downLength; ++i){
        if(map[pRow+i][pCol] == TILE_MONSTER){
            // monster found
            map[pRow+i][pCol] = TILE_OPEN;
            map[pRow+i-1][pCol] = TILE_MONSTER;
            if(i == 1){
                eaten = true;
            }
        } else if(map[pRow+i][pCol] == TILE_PILLAR){
            break;
        }
    }

    // right
    for(int i = 1; i <= rightLength; ++i){
        if(map[pRow][pCol+i] == TILE_MONSTER){
            // monster found
            map[pRow][pCol+i] = TILE_OPEN;
            map[pRow][pCol+i - 1] = TILE_MONSTER;
            if(i == 1){
                eaten = true;
            }
        } else if(map[pRow][pCol+i] == TILE_PILLAR){
            break;
        }
    } 

    // left
    for(int i = 1; i <= leftLength; ++i){
        if(map[pRow][pCol-i] == TILE_MONSTER){
            // monster found
            map[pRow][pCol-i] = TILE_OPEN;
            map[pRow][pCol-i + 1] = TILE_MONSTER;
            if(i == 1){
                eaten = true;
            }
        } else if(map[pRow][pCol-i] == TILE_PILLAR){
            break;
        }
    }
    
    if(eaten){
        return true;
    }
    return false;
}
