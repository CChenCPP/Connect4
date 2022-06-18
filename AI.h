#pragma once
#include <vector>
#include <string>
#include <optional>
#include "Utility.h"

namespace AI{
     bool checkBottom(int row, int column, const std::vector<std::vector<std::string>>& board);
     bool checkLeftRight(int row, int column, const std::vector<std::vector<std::string>>& board);
     bool checkDiagonal(int row, int column, const std::vector<std::vector<std::string>>& board);
     bool checkWin(int row, int column, const std::vector<std::vector<std::string>>& board);
     std::optional<int> determineInsertPosition(int row, int column, const std::vector<std::vector<std::string>>& board);
     int monteCarloSim(const std::vector<std::vector<std::string>>& board, const std::string& AIcolor);
     int monteCarloSim(const std::vector<std::vector<std::string>>& board, const std::string& AIcolor, int sims);
}
