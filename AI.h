#pragma once
#include <QObject>
#include <vector>
#include <string>
#include <optional>
#include "Utility.h"

class AI : public QObject{
    Q_OBJECT
    using Board = std::vector<std::vector<std::string>>;
public:
     AI();

     std::string getColor() const;
     bool isActive() const;
     int nextMove() const;
     int nextMove(int sims) const;
     AI& linkBoard(Board* board);
     AI& setActive(bool active);
     AI& setColor(std::string color);

private:
     Board* board;
     std::string color;
     bool active;

     std::string alternate(std::string color) const;
     bool checkBottom(int row, int column, const Board& board) const;
     bool checkLeftRight(int row, int column, const Board& board) const;
     bool checkDiagonal(int row, int column, const Board& board) const;
     bool checkWin(int row, int column, const Board& board) const;
     std::optional<int> determineInsertRow(int row, int column, const Board& board) const;
     std::vector<int> getPossibleMoves() const;
     int monteCarloSim() const;
     int monteCarloSim(int sims) const;
     std::vector<int>& randomize(std::vector<int>& vector) const;
};
