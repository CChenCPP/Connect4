#include "AI.h"
#include <limits>
#include <iostream>

AI::AI() : active(false)
{
}

// public methods

std::string AI::getColor() const
{
    return color;
}

bool AI::isActive() const
{
    return active;
}

int AI::nextMove() const
{
    return monteCarloSim();
}

int AI::nextMove(int sims) const
{
    return monteCarloSim(sims);
}

AI& AI::linkBoard(Board* board)
{
    this->board = board;
    return *this;
}

AI& AI::setActive(bool active)
{
    this->active = active;
    return *this;
}

AI& AI::setColor(std::string color)
{
    this->color = color;
    return *this;
}

// private methods
std::string AI::alternate(std::string color) const {
    return (color == "BLACK") ? "RED" : "BLACK";
}

bool AI::checkBottom(int row, int column, const Board& board) const
{
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

bool AI::checkLeftRight(int row, int column, const Board& board) const
{
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

bool AI::checkDiagonal(int row, int column, const Board& board) const
{
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

bool AI::checkWin(int row, int column, const Board& board) const
{
    bool winB = checkBottom(row, column, board);
    bool winLR = checkLeftRight(row, column, board);
    bool winDiag = checkDiagonal(row, column, board);
    return winB || winLR || winDiag;
}

std::optional<int> AI::determineInsertRow(int row, int column, const Board& board) const
{
    if (board[row][column] != ""){
        return {};
    }
    int trueRow = row;
    while (trueRow + 1 < board.size() && board[trueRow + 1][column] == "") {
        ++trueRow;
    }
    return trueRow;
}

std::vector<int> AI::getPossibleMoves() const
{
    int rows = board->size();
    int columns = (*board)[0].size();

    std::vector<int> allPossibleMoves;
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            if ((*board)[row][column] == "") {
                allPossibleMoves.push_back(column);
            }
        }
    };

    return allPossibleMoves;
}

int AI::monteCarloSim() const
{
    int size = board->size() * (*board)[0].size();
    int sims = std::log10(size) * std::pow(std::numeric_limits<int>::max(), 0.6) / size;
    return monteCarloSim(sims);
}

int AI::monteCarloSim(int sims) const
{
    int rows = board->size();
    int columns = (*board)[0].size();

    std::vector<int> allPossibleMoves = getPossibleMoves();
    if (allPossibleMoves.size() == rows * columns) { return RNG::randomNum(0, columns - 1); };

    std::vector<int64_t> movePenalty(columns);
    for (int i = 0; i < sims; ++i) {
        std::string currentColor = color;
        Board simBoard = *board;
        std::vector<int> possibleMoves = randomize(allPossibleMoves);

        // first move by AI
        int firstMoveColumn = possibleMoves.back();
        int firstMoveRow = determineInsertRow(0, firstMoveColumn, simBoard).value();
        simBoard[firstMoveRow][firstMoveColumn] = currentColor;
        possibleMoves.pop_back();
        if (checkWin(firstMoveRow, firstMoveColumn, simBoard)) { return firstMoveColumn; };
        currentColor = alternate(currentColor);

        // sim remaining moves based on available positions
        int64_t penalty = 1000000;
        while (possibleMoves.size() > 0) {
            int column = possibleMoves.back();
            int row = determineInsertRow(0, column, simBoard).value();
            possibleMoves.pop_back();
            simBoard[row][column] = currentColor;
            if (checkWin(row,column,simBoard) && currentColor != color) {
                movePenalty[firstMoveColumn] -= penalty;
                break;
            }
            currentColor = alternate(currentColor);
            penalty = std::pow(penalty, 0.95);
        }
    }

    // select move with lowest penalty
    std::replace_if(movePenalty.begin(), movePenalty.end(), [&](int64_t& penalty){ return penalty == 0; }, std::numeric_limits<int64_t>::min());
    int bestMove = std::max_element(movePenalty.begin(), movePenalty.end()) - movePenalty.begin();

    // for debugging
    for (int i = 0; i < movePenalty.size(); ++i){
        std::cout << "Column: " << i << " Penalty: " << movePenalty[i] << std::endl;
    }

    return bestMove;
}

std::vector<int>& AI::randomize(std::vector<int>& vector) const {
    std::shuffle(vector.begin(), vector.end(), RNG::generator);
    return vector;
}
