#pragma once
#include <QPushButton>

class CustomPushButton : public QPushButton
{
    Q_OBJECT
public:
    CustomPushButton(int row, int column, QWidget* parent = nullptr);
    int getRow();
    int getColumn();

private:
    int row;
    int column;

public slots:
    void customClicked(bool);

signals:
    void gridPosition(int row, int column);
};

