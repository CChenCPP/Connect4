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
    using Board = std::vector<std::vector<std::string>>;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    static constexpr uint8_t minRows = 4;
    static constexpr uint8_t minColumns = 4;
    static constexpr uint8_t defaultRows = 6;
    static constexpr uint8_t defaultColumns = 7;
    static constexpr uint8_t maxRows = 10;
    static constexpr uint8_t maxColumns = 15;
    int rows;
    int columns;
    Board board;
    int filledSlots;
    bool gameOver;
    std::string color;
    AI ai;
    Ui::MainWindow* UI;
    AIPromptWindow* aiPromptWindow;

    void AImove();
    std::string alternate(std::string color) const;
    bool boardIsFull() const;
    bool checkBottom(int row, int column, std::string& color);
    bool checkLeftRight(int row, int column, std::string& color);
    bool checkDiagonal(int row, int column, std::string& color);
    void createBoard();
    void createUIBoard();
    void deleteUIBoard();
    std::optional<int> determineInsertRow(int row, int column) const;
    void disableBoard();
    void enableBoard();
    std::pair<int,int> getNewBoardSize() const;
    void reset();

private slots:
    void onBoardUpdated(int row,int column, std::string color);
    void onConnect4(int rowStart, int columnStart, int rowEnd, int columnEnd, std::string color);
    void onCustomPushButtonClicked(int row, int column);
    void onPlayerColorPreferenceSelected(std::string color);
    void onPlayerWon(std::string color);
    void on_newGamePushButton_clicked();
    void on_newGameAIPushButton_clicked();

signals:
    void boardUpdated(int row, int column, std::string color);
    void connect4(int rowStart, int columnStart, int rowEnd, int columnEnd, std::string color);
    void playerWon(std::string color);
};
