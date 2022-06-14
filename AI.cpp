#include "AI.h"

bool AI::checkBottom(int row,int column, std::vector<std::vector<std::string>>& board){
    int rows = board.size();
    std::string color = board[row][column];
    int consecutive = 1;
    int endRow = row;
    // check bottom
    while ((endRow + 1) < rows && board[endRow + 1][column] == color){
        ++consecutive;
        ++endRow;
    }
    if (consecutive >= 4) { return true; };

    return false;
}

bool AI::checkLeftRight(int row,int column, std::vector<std::vector<std::string>>& board){
    int columns = board[0].size();
    std::string color = board[row][column];
    int consecutive = 1;
    int startColumn = column;
    int endColumn = column;
    // check left
    while ((startColumn - 1) >= 0 && board[row][startColumn - 1] == color){
        ++consecutive;
        --startColumn;
    }
    // check right
    while ((endColumn + 1) < columns && board[row][endColumn + 1] == color){
        ++consecutive;
        ++endColumn;
    }
    if (consecutive >= 4) { return true; };

    return false;
}

bool AI::checkDiagonal(int row,int column, std::vector<std::vector<std::string>>& board){
    int rows = board.size();
    int columns = board[0].size();
    std::string color = board[row][column];

    int consecutive = 1;
    int startRow = row;
    int endRow = row;
    int startColumn = column;
    int endColumn = column;
    // check top left to bottom right diagonal
    while ((endRow + 1) < rows && (endColumn + 1) < columns && board[endRow + 1][endColumn + 1] == color){
        ++consecutive;
        ++endRow;
        ++endColumn;
    }
    while ((startRow - 1) >= 0 && (startColumn - 1) >= 0 && board[startRow - 1][startColumn - 1] == color){
        ++consecutive;
        --startRow;
        --startColumn;
    }
    if (consecutive >= 4) { return true; };

    consecutive = 1;
    startRow = row;
    endRow = row;
    startColumn = column;
    endColumn = column;
    // check bottom right to top left diagonal
    while ((endRow + 1) < rows && (startColumn - 1) >= 0 && board[endRow + 1][startColumn - 1] == color){
        ++consecutive;
        ++endRow;
        --startColumn;
    }
    while ((startRow - 1) >= 0 && (endColumn + 1) < columns && board[startRow - 1][endColumn + 1] == color){
        ++consecutive;
        --startRow;
        ++endColumn;
    }
    if (consecutive >= 4) { return true; };

    return false;
}

bool AI::checkWin(int row, int column, std::vector<std::vector<std::string>>& board)
{
    bool winB = checkBottom(row, column, board);
    bool winLR = checkLeftRight(row, column, board);
    bool winDiag = checkDiagonal(row, column, board);
    return winB || winLR || winDiag;
}

std::optional<int> AI::determineInsertPosition(int row, int column, std::vector<std::vector<std::string>>& board){
    if (board[row][column] != ""){
        return {};
    }
    int trueRow = row;
    while (trueRow + 1 < board.size() && board[trueRow + 1][column] == "") {
        ++trueRow;
    }
    return trueRow;
}

int RNG::randomInt(int min, int max) {
    std::uniform_int_distribution<int> range(min, max);
    return range(generator);
}
