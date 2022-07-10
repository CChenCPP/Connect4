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
    deleteUIBoard();
    createBoard();
    createUIBoard();
    disableBoard();
    connect(this,&MainWindow::boardUpdated,this,&MainWindow::onBoardUpdated);
    connect(this,&MainWindow::connect4,this,&MainWindow::onConnect4);
    connect(this,&MainWindow::playerWon,this,&MainWindow::onPlayerWon);
    connect(aiPromptWindow,&AIPromptWindow::humanColorSelection,this,&MainWindow::onPlayerColorPreferenceSelected);
}

MainWindow::~MainWindow()
{
    delete UI;
}

// private methods
void MainWindow::AImove()
{
    if (gameOver) { return; };
    int column;
    std::cout << Benchmark::timeFunctionMs([&](){ column = ai.nextMove();}) << "ms\n";
//    int column = ai.nextMove();
    std::optional<int> trueRow = determineInsertRow(0,column);
    CustomPushButton* button = static_cast<CustomPushButton*>(UI->boardGrid->itemAtPosition(trueRow.value(),column)->widget());
    QPixmap icon((ai.getColor() == "BLACK") ? ":/Resources/Images/black_dot.png" : ":/Resources/Images/red_dot.png");
    button->setIcon(icon);
    board[trueRow.value()][column] = ai.getColor();
    emit boardUpdated(trueRow.value(), column, ai.getColor());
    if (!gameOver) {
        color = alternate(color);
        UI->playerTurnLineEdit->setText("Player " + Parse::toQString(color) + "'s turn"); };
}

std::string MainWindow::alternate(std::string color) const
{
    return (color == "BLACK") ? "RED" : "BLACK";
}

bool MainWindow::boardIsFull() const
{
    return filledSlots == rows * columns;
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

void MainWindow::createBoard()
{
    std::pair<int,int> newDimensions = getNewBoardSize();
    rows = newDimensions.first;
    columns = newDimensions.second;
    if (rows < minRows || rows > maxRows || columns < minColumns || columns > maxColumns) { rows = defaultRows; columns = defaultColumns; };
    board = std::vector(rows,std::vector<std::string>(columns));
    filledSlots = 0;
}

void MainWindow::createUIBoard()
{
    this->setFixedSize(columns * 100 + 100, rows * 100);
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

void MainWindow::deleteUIBoard()
{
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
}

std::optional<int> MainWindow::determineInsertRow(int row, int column) const
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
        case(6):
           return std::pair{10,12};
        case(7):
           return std::pair{10,14};
        case(8):
           return std::pair{10,15};
    }
}

void MainWindow::reset()
{
    ai.setActive(false);
    gameOver = false;
    deleteUIBoard();
    createBoard();
    createUIBoard();
    enableBoard();
    color = "RED";
    UI->playerTurnLineEdit->setText("Player RED's turn");
}

// private slots
void MainWindow::onBoardUpdated(int row, int column, std::string color)
{
    ++filledSlots;
    bool winB = checkBottom(row, column, color);
    bool winLR = checkLeftRight(row, column, color);
    bool winDiag = checkDiagonal(row, column, color);
    enableBoard();
    if (winB || winLR || winDiag){
        emit playerWon(color);
        return;
    }
    if (boardIsFull()){
        emit playerWon("DRAW");
        return;
    }
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
    std::optional<int> trueRow = determineInsertRow(row,column);
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

    emit boardUpdated(trueRow.value(),column, color);
    color = alternate(color);
    UI->playerTurnLineEdit->setText("Player " + Parse::toQString(color) + "'s turn");

    if (ai.isActive() && !gameOver) {
        disableBoard();
        UI->playerTurnLineEdit->setText("AI calculating move...");
        QTimer::singleShot(RNG::randomNum(50,100), this, &MainWindow::AImove);
    }
}

void MainWindow::onPlayerColorPreferenceSelected(std::string color)
{
    reset();
    std::string aiColor = alternate(color);
    ai.setActive(true).linkBoard(&board).setColor(aiColor);
    if (ai.getColor() == this->color) { AImove(); };
}

void MainWindow::onPlayerWon(std::string color)
{
    gameOver = true;
    QMessageBox messageBox;
    if (color == "DRAW"){
        UI->playerTurnLineEdit->setText("Draw game");
        messageBox.setWindowTitle("Draw game");
        messageBox.setText(QString("Draw game"));
    }
    else {
        UI->playerTurnLineEdit->setText("Player " + Parse::toQString(color) + " victory!");
        messageBox.setWindowTitle((ai.isActive() && ai.getColor() == color) ? "You lost!" : "Victory!");
        messageBox.setText(QString("Player " + Parse::toQString(color) + ((ai.isActive() && color == ai.getColor()) ? " (AI) " : "") + " has won!"));
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

