#include "MainWindow.h"
#include "UI_MainWindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , UI(new Ui::MainWindow)
    , rows(6)
    , columns(7)
    , filledSlots(0)
    , color("RED")
    , gameOver(false)
{
    UI->setupUi(this);
    this->setWindowTitle("Connect 4");
    initBoard(rows,columns);
    disableBoard();
    connect(this,SIGNAL(boardUpdated(int,int,std::string)),this,SLOT(onBoardUpdated(int,int,std::string)));
    connect(this,SIGNAL(connect4(int,int,int,int)),this,SLOT(onConnect4(int,int,int,int)));
    connect(this,SIGNAL(playerWon(std::string)),this,SLOT(onPlayerWon(std::string)));
}

MainWindow::~MainWindow()
{
    delete UI;
}

bool MainWindow::checkBottom(int row, int column)
{
    std::string color = board[row][column];
    int consecutive = 1;
    int endRow = row;
    // check bottom
    while ((endRow + 1) < rows && board[endRow + 1][column] == color){
        ++consecutive;
        ++endRow;
    }
    if (consecutive >= 4) { emit connect4(row, column, endRow, column); return true; };

    return false;
}

bool MainWindow::checkLeftRight(int row, int column)
{
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
    if (consecutive >= 4) { emit connect4(row, startColumn, row, endColumn); return true; };

    return false;
}

bool MainWindow::checkDiagonal(int row, int column)
{
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
    if (consecutive >= 4) { emit connect4(startRow, startColumn, endRow, endColumn); return true; };

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
    if (consecutive >= 4) { emit connect4(endRow, startColumn, startRow, endColumn); return true; };

    return false;
}

// private methods
std::optional<int> MainWindow::determineInsertPosition(int row, int column) const
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

void MainWindow::disableBoard()
{
    for (int i = 0; i < UI->boardGrid->rowCount(); ++i){
        for (int j = 0; j < UI->boardGrid->columnCount(); ++j){
            QPushButton* button = static_cast<QPushButton*>(UI->boardGrid->itemAtPosition(i,j)->widget());
            button->blockSignals(true);
            button->setDisabled(true);
        }
    }
}

void MainWindow::enableBoard()
{
    for (int i = 0; i < UI->boardGrid->rowCount(); ++i){
        for (int j = 0; j < UI->boardGrid->columnCount(); ++j){
            QPushButton* button = static_cast<QPushButton*>(UI->boardGrid->itemAtPosition(i,j)->widget());
            button->blockSignals(false);
            button->setEnabled(true);
        }
    }
}

void MainWindow::initBoard(int rows, int columns)
{
    board = std::vector(rows,std::vector<std::string>(columns));

    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < columns; ++j){
            CustomPushButton* button = new CustomPushButton(i,j);
            connect(button,SIGNAL(gridPosition(int,int)),this,SLOT(buttonClicked(int,int)));
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            button->setIconSize(QSize(80,80));
            UI->boardGrid->addWidget(button,i,j);
            UI->boardGrid->setHorizontalSpacing(0);
            UI->boardGrid->setVerticalSpacing(0);
        }
    }
}

int MainWindow::monteCarloSim(int sims)
{
    std::vector<int> winOccurences(board[0].size());
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

    // sim random first move and fill board randomly until full. Run dijkstra to determine if there is a win.
    for (int i = 0; i < sims; ++i) {
        simBoard = board;
        std::string currentColor = this->color;

        // copy vector of available moves for each sim
        std::vector<int> availableColumns = availableMoves;

        // first move by AI
        int index = RNG::randomInt(0, availableColumns.size() - 1);
        int column = availableColumns[index];
        int trueRow = AI::determineInsertPosition(0, column, simBoard).value();
        simBoard[trueRow][column] = currentColor;
        availableColumns.erase(availableColumns.begin() + index);
        if (AI::checkWin(trueRow,column,simBoard)) { winOccurences[column]++; continue; };
        currentColor = (currentColor == "BLACK") ? "RED" : "BLACK";

        while (availableColumns.size() > 0) {
            // remaining moves based on available positions
            int index2 = RNG::randomInt(0, availableColumns.size() - 1);
            int column2 = availableColumns[index2];
            int trueRow2 = AI::determineInsertPosition(0, column2, simBoard).value();
            simBoard[trueRow2][column2] = currentColor;
            availableColumns.erase(availableColumns.begin() + index);
            if (AI::checkWin(trueRow,column,simBoard) && currentColor == "BLACK") { winOccurences[column]++; continue; };
            currentColor = (currentColor == "BLACK") ? "RED" : "BLACK";
        }
    }

    // determine best move by win occurence of each first move
    int bestMoveColumn = 0;
    for (int column = 0; column < winOccurences.size(); ++column) {
        if (winOccurences[column] >= winOccurences[bestMoveColumn]){
            bestMoveColumn = column;
        }
    };

    // incase there were no victories in all simulations, pick a random available move
    if (winOccurences[bestMoveColumn] == 0) {
        bestMoveColumn = availableMoves[RNG::randomInt(0, availableMoves.size() - 1)];
    }

    return bestMoveColumn;
}

// private slots
void MainWindow::buttonClicked(int row, int column)
{
    if (gameOver) { return; };
    std::optional<int> trueRow = determineInsertPosition(row,column);
    if (!trueRow){ return; };

    disableBoard();
    CustomPushButton* button = static_cast<CustomPushButton*>(UI->boardGrid->itemAtPosition(trueRow.value(),column)->widget());
    if (color == "RED"){
        QPixmap icon(":/Resources/Images/red_dot.png");
        button->setIcon(icon);
        board[trueRow.value()][column] = "RED";
    }
    else if (color == "BLACK"){
        QPixmap icon(":/Resources/Images/black_dot.png");
        button->setIcon(icon);
        board[trueRow.value()][column] = "BLACK";
    }

    ++filledSlots;
    emit boardUpdated(trueRow.value(),column, color);
    color = (color == "BLACK") ? "RED" : "BLACK";
    UI->playerTurnLineEdit->setText("Player " + QString::fromUtf8(color) + "'s turn");
}

void MainWindow::onConnect4(int rowStart, int columnStart, int rowEnd, int columnEnd)
{
    // highlights victory cells
    int rowS = rowStart;
    int columnS = columnStart;
    CustomPushButton* button = static_cast<CustomPushButton*>(UI->boardGrid->itemAtPosition(rowS,columnS)->widget());
    QPixmap icon((color == "RED") ? ":/Resources/Images/red_dot_win.png" : ":/Resources/Images/black_dot_win.png");
    button->setIcon(icon);
    while (rowS != rowEnd || columnS != columnEnd){
        rowS = (rowEnd - rowStart > 0) ? std::min<int>(rowS + 1, rowEnd) : std::max<int>(rowS - 1, rowEnd);
        columnS = std::min<int>(columnS + 1, columnEnd);
        button = static_cast<CustomPushButton*>(UI->boardGrid->itemAtPosition(rowS,columnS)->widget());
        icon = QPixmap((color == "RED") ? ":/Resources/Images/red_dot_win.png" : ":/Resources/Images/black_dot_win.png");
        button->setIcon(icon);
    }
}

void MainWindow::onBoardUpdated(int row, int column, std::string color)
{
    bool winB = checkBottom(row,column);
    bool winLR = checkLeftRight(row,column);
    bool winDiag = checkDiagonal(row,column);
    enableBoard();
    if (winB || winLR || winDiag){
        emit playerWon(color);
        return;
    }
    if (filledSlots >= rows * columns){
        emit playerWon("DRAW");
        return;
    }
}

void MainWindow::onPlayerWon(std::string color)
{
    gameOver = true;
    QMessageBox messageBox;
    if (color == "DRAW"){
        messageBox.setWindowTitle("Draw game");
        messageBox.setText(QString("DRAW GAME"));
    }
    else {
        messageBox.setWindowTitle("Victory!");
        messageBox.setText(QString("Player " + QString::fromUtf8(color) + " has won!"));
    }
    messageBox.exec();
}

void MainWindow::on_newGamePushButton_clicked()
{
    gameOver = false;
    board.clear();
    board = std::vector(rows,std::vector<std::string>(columns));
    filledSlots = 0;
    enableBoard();
    color = "RED";
    UI->playerTurnLineEdit->setText("Player RED's turn");
    for (int i = 0; i < UI->boardGrid->rowCount(); ++i){
        for (int j = 0; j < UI->boardGrid->columnCount(); ++j){
            QPushButton* button = static_cast<QPushButton*>(UI->boardGrid->itemAtPosition(i,j)->widget());
            button->setIcon(QIcon());
        }
    }
}
