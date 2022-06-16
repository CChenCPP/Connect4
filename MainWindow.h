#pragma once
#include <QMainWindow>
#include <CustomPushButton.h>
#include <AI.h>
#include <AIPromptWindow.h>

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
    static constexpr int minRows = 4;
    static constexpr int minColumns = 4;
    static constexpr int defaultRows = 6;
    static constexpr int defaultColumns = 7;
    static constexpr int maxRows = 8;
    static constexpr int maxColumns = 10;
    int rows;
    int columns;
    std::vector<std::vector<std::string>> board;
    int filledSlots;
    bool gameOver;
    std::string color;
    std::string AIcolor;
    bool AI;
    Ui::MainWindow* UI;
    AIPromptWindow* aiPromptWindow;
    void AImove();
    bool checkBottom(int row, int column, std::string& color);
    bool checkLeftRight(int row, int column, std::string& color);
    bool checkDiagonal(int row, int column, std::string& color);
    void createUIBoard();
    std::optional<int> determineInsertPosition(int row, int column) const;
    void disableBoard();
    void enableBoard();
    std::pair<int,int> getNewBoardSize() const;
    void reset();

private slots:
    void onBoardUpdated(int row,int column, std::string color);
    void onColorPerferenceSelected(std::string color);
    void onConnect4(int rowStart, int columnStart, int rowEnd, int columnEnd, std::string color);
    void onCustomPushButtonClicked(int row, int column);
    void onPlayerWon(std::string color);
    void on_newGamePushButton_clicked();
    void on_newGameAIPushButton_clicked();

signals:
    void boardUpdated(int row, int column, std::string color);
    void connect4(int rowStart, int columnStart, int rowEnd, int columnEnd, std::string color);
    void playerWon(std::string color);
};
