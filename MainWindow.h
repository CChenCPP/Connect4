#pragma once
#include <QMainWindow>
#include <CustomPushButton.h>
#include <AI.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    int rows;
    int columns;
    int filledSlots;
    std::vector<std::vector<std::string>> board;
    std::string color;
    bool gameOver;
    Ui::MainWindow* UI;

    bool checkBottom(int row,int column);
    bool checkLeftRight(int row,int column);
    bool checkDiagonal(int row,int column);
    std::optional<int> determineInsertPosition(int row, int column) const;
    void disableBoard();
    void enableBoard();
    void initBoard(int rows, int columns);
    int monteCarloSim(int sims);

private slots:
    void buttonClicked(int row, int column);
    void onConnect4(int rowStart, int columnStart, int rowEnd, int columnEnd);
    void onBoardUpdated(int row,int column, std::string color);
    void onPlayerWon(std::string color);
    void on_newGamePushButton_clicked();

    void on_newGameAIPushButton_clicked();

signals:
    void boardUpdated(int row, int column, std::string color);
    void connect4(int rowStart, int columnStart, int rowEnd, int columnEnd);
    void playerWon(std::string color);
};
