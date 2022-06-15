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

int AI::monteCarloSim(std::vector<std::vector<std::string>>& board, std::string AIcolor, int sims)
{
    int rows = board.size();
    int columns = board[0].size();
    std::vector<long long> winProbability(columns);
    std::vector<std::vector<std::string>> simBoard;
    std::vector<int> availableMoves;

    // find all possible remaining moves
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            if (board[row][column] == "") {
                availableMoves.push_back(column);
            }
        }
    };

    if (availableMoves.size() == rows * columns) { return RNG::randomInt(0, columns - 1); };

    for (int i = 0; i < sims; ++i) {
        simBoard = board;
        std::string currentColor = AIcolor;

        // copy vector of available moves for each sim
        std::vector<int> availableColumns = availableMoves;

        // first move by AI
        int index = RNG::randomInt(0, availableColumns.size() - 1);
        int column = availableColumns[index];
        int trueRow = AI::determineInsertPosition(0, column, simBoard).value();
        simBoard[trueRow][column] = currentColor;
        availableColumns.erase(availableColumns.begin() + index);
        if (AI::checkWin(trueRow,column,simBoard)) { return column; };
        currentColor = (currentColor == "BLACK") ? "RED" : "BLACK";

        long long penalty = 1000000;
        long long moves = 0;
        while (availableColumns.size() > 0) {
            // remaining moves based on available positions
            int index2 = RNG::randomInt(0, availableColumns.size() - 1);
            int column2 = availableColumns[index2];
            int trueRow2 = AI::determineInsertPosition(0, column2, simBoard).value();
            simBoard[trueRow2][column2] = currentColor;
            availableColumns.erase(availableColumns.begin() + index2);
            if (AI::checkWin(trueRow2,column2,simBoard) && currentColor == AIcolor) { winProbability[column] += penalty; break; };
            if (AI::checkWin(trueRow2,column2,simBoard) && currentColor != AIcolor) { winProbability[column] -= penalty; break; }
            currentColor = (currentColor == "BLACK") ? "RED" : "BLACK";
            ++moves;
            penalty = (moves % 2 == 0) ? std::pow(penalty, 0.96) : penalty;
        }

    }

    // select move with highest win probability
    int bestMoveColumn = 0;
    for (int column = 0; column < winProbability.size(); ++column) {
        if (winProbability[column] == 0) { continue; };
        if (winProbability[column] >= winProbability[bestMoveColumn]){
            bestMoveColumn = column;
        }
    };

    // incase there were no victories in all simulations, pick a random available move
    if (winProbability[bestMoveColumn] == 0) {
        bestMoveColumn = availableMoves[RNG::randomInt(0, availableMoves.size() - 1)];
    }

    return bestMoveColumn;
}


int RNG::randomInt(int min, int max) {
    std::uniform_int_distribution<int> range(min, max);
    return range(generator);
}
