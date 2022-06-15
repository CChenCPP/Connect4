#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , UI(new Ui::MainWindow)
    , rows(6)
    , columns(7)
    , filledSlots(0)
    , color("RED")
    , gameOver(false)
    , AI(false)
    , aiPromptWindow(new AIPromptWindow())
{
    UI->setupUi(this);
    setWindowTitle("Connect 4");
    setFixedSize(size());
    initBoard(rows,columns);
    disableBoard();
    connect(this,SIGNAL(boardUpdated(int,int,std::string)),this,SLOT(onBoardUpdated(int,int,std::string)));
    connect(this,SIGNAL(connect4(int,int,int,int,std::string)),this,SLOT(onConnect4(int,int,int,int,std::string)));
    connect(this,SIGNAL(playerWon(std::string)),this,SLOT(onPlayerWon(std::string)));
    connect(aiPromptWindow,SIGNAL(humanColorSelection(std::string)),this,SLOT(onColorPerferenceSelected(std::string)));
}

MainWindow::~MainWindow()
{
    delete UI;
}

// private methods
void MainWindow::AImove()
{
    if (gameOver) { return; };
    int column = AI::monteCarloSim(board, AIcolor, 100000);
    std::optional<int> trueRow = determineInsertPosition(0,column);
    CustomPushButton* button = static_cast<CustomPushButton*>(UI->boardGrid->itemAtPosition(trueRow.value(),column)->widget());
    QPixmap icon((AIcolor == "BLACK") ? ":/Resources/Images/black_dot.png" : ":/Resources/Images/red_dot.png");
    button->setIcon(icon);
    board[trueRow.value()][column] = AIcolor;
    ++filledSlots;
    emit boardUpdated(trueRow.value(),column, AIcolor);
    if (!gameOver) {
        color = (color == "BLACK") ? "RED" : "BLACK";
        UI->playerTurnLineEdit->setText("Player " + QString::fromUtf8(color) + "'s turn"); };
}

bool MainWindow::checkBottom(int row, int column, std::string color)
{
    int consecutive = 1;
    int endRow = row;
    // check bottom
    while ((endRow + 1) < rows && board[endRow + 1][column] == color){
        ++consecutive;
        ++endRow;
    }
    if (consecutive >= 4) { emit connect4(row, column, endRow, column, color); return true; };

    return false;
}

bool MainWindow::checkLeftRight(int row, int column, std::string color)
{
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
    if (consecutive >= 4) { emit connect4(row, startColumn, row, endColumn, color); return true; };

    return false;
}

bool MainWindow::checkDiagonal(int row, int column, std::string color)
{
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
    if (consecutive >= 4) { emit connect4(startRow, startColumn, endRow, endColumn, color); return true; };

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
    if (consecutive >= 4) { emit connect4(endRow, startColumn, startRow, endColumn, color); return true; };

    return false;
}

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
            if (board[i][j] == "") { button->setDisabled(true); };
        }
    }
}

void MainWindow::enableBoard()
{
    for (int i = 0; i < UI->boardGrid->rowCount(); ++i){
        for (int j = 0; j < UI->boardGrid->columnCount(); ++j){
            QPushButton* button = static_cast<QPushButton*>(UI->boardGrid->itemAtPosition(i,j)->widget());
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

void MainWindow::reset()
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

    if (AI && !gameOver) {
        disableBoard();
        UI->playerTurnLineEdit->setText("A.I. calculating move...");
        QTimer::singleShot(RNG::randomInt(200,400), this, [&](){AImove(); });
    }
}

void MainWindow::onColorPerferenceSelected(std::string color)
{
    AIcolor = (color == "BLACK") ? "RED" : "BLACK";
}

void MainWindow::onConnect4(int rowStart, int columnStart, int rowEnd, int columnEnd, std::string color)
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
    bool winB = checkBottom(row, column, color);
    bool winLR = checkLeftRight(row, column, color);
    bool winDiag = checkDiagonal(row, column, color);
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
        UI->playerTurnLineEdit->setText("Draw game");
        messageBox.setWindowTitle("Draw game");
        messageBox.setText(QString("DRAW GAME"));
    }
    else {
        UI->playerTurnLineEdit->setText("Player " + QString::fromUtf8(color) + " victory!");
        messageBox.setWindowTitle("Victory!");
        messageBox.setText(QString("Player " + QString::fromUtf8(color) + " has won!"));
    }
    messageBox.exec();
}

void MainWindow::on_newGamePushButton_clicked()
{
    AI = false;
    reset();
}

void MainWindow::on_newGameAIPushButton_clicked()
{
    aiPromptWindow->exec();
    AI = true;
    reset();
    if (AIcolor == color) { AImove(); };
}

