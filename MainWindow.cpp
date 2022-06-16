#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QTimer>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , rows(8)
    , columns(10)
    , UI(new Ui::MainWindow)
    , aiPromptWindow(new AIPromptWindow())
{
    UI->setupUi(this);
    setWindowTitle("Connect 4");
    setFixedSize(size());
    createUIBoard();
    disableBoard();
    connect(this,&MainWindow::boardUpdated,this,&MainWindow::onBoardUpdated);
    connect(this,&MainWindow::connect4,this,&MainWindow::onConnect4);
    connect(this,&MainWindow::playerWon,this,&MainWindow::onPlayerWon);
    connect(aiPromptWindow,&AIPromptWindow::humanColorSelection,this,&MainWindow::onColorPerferenceSelected);
}

MainWindow::~MainWindow()
{
    delete UI;
}

// private methods
void MainWindow::AImove()
{
    if (gameOver) { return; };
    int column = AI::monteCarloSim(board, AIcolor, 5000);
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

bool MainWindow::checkBottom(int row, int column, std::string& color)
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

bool MainWindow::checkLeftRight(int row, int column, std::string& color)
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

bool MainWindow::checkDiagonal(int row, int column, std::string& color)
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

void MainWindow::createUIBoard()
{
    // delete existing UI board
    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < columns; ++j){
            QLayoutItem* button = UI->boardGrid->itemAtPosition(i,j);
            if (button){
                QWidget* widget = button->widget();
                if (widget){
                    UI->boardGrid->removeWidget(widget);
                    delete widget;
                }
            }
        }
    }

    // get new dimensions
    std::pair<int,int> newDimensions = getNewBoardSize();
    rows = newDimensions.first;
    columns = newDimensions.second;
    if (rows < minRows || rows > maxRows || columns < minColumns || columns > maxColumns) { rows = defaultRows; columns = defaultColumns; };
    board = std::vector(rows,std::vector<std::string>(columns));

    // create new UI board
    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < columns; ++j){
            CustomPushButton* button = new CustomPushButton(i,j);
            connect(button,&CustomPushButton::gridPosition,this,&MainWindow::onCustomPushButtonClicked);
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            button->setIconSize(QSize(80,80));
            UI->boardGrid->addWidget(button,i,j);
            UI->boardGrid->setHorizontalSpacing(0);
            UI->boardGrid->setVerticalSpacing(0);
        }
    }
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
    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < columns; ++j){
            QPushButton* button = static_cast<QPushButton*>(UI->boardGrid->itemAtPosition(i,j)->widget());
            if (board[i][j] == "") { button->setDisabled(true); };
        }
    }
}

void MainWindow::enableBoard()
{
    for (int i = 0; i < rows; ++i){
        for (int j = 0; j < columns; ++j){
            QPushButton* button = static_cast<QPushButton*>(UI->boardGrid->itemAtPosition(i,j)->widget());
            button->setEnabled(true);
        }
    }
}

std::pair<int, int> MainWindow::getNewBoardSize() const
{
    int index = UI->boardSizeComboBox->currentIndex();
    switch (index){
        case(0):
            return std::pair{6,7};
        case(1):
            return std::pair{7,8};
        case(2):
            return std::pair{7,9};
        case(3):
            return std::pair{8,8};
        case(4):
            return std::pair{8,9};
        case(5):
            return std::pair{8,10};
    }
}

void MainWindow::reset()
{
    gameOver = false;
    AI = false;
    AIcolor = "";
    createUIBoard();
    board = std::vector(rows,std::vector<std::string>(columns));
    filledSlots = 0;
    enableBoard();
    color = "RED";
    UI->playerTurnLineEdit->setText("Player RED's turn");
}

// private slots
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

void MainWindow::onColorPerferenceSelected(std::string color)
{
    reset();
    AIcolor = (color == "BLACK") ? "RED" : "BLACK";
    AI = true;
    if (AIcolor == this->color) { AImove(); };
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

void MainWindow::onCustomPushButtonClicked(int row, int column)
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
        QTimer::singleShot(RNG::randomInt(100,150), this, &MainWindow::AImove);
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
        messageBox.setText(QString("Player " + QString::fromUtf8(color) + ((color == AIcolor) ? " (A.I.) " : "") + " has won!"));
    }
    messageBox.exec();
}

void MainWindow::on_newGamePushButton_clicked()
{
    reset();
}

void MainWindow::on_newGameAIPushButton_clicked()
{
    aiPromptWindow->exec();
}

