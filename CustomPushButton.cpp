#include "CustomPushButton.h"

CustomPushButton::CustomPushButton(int row, int column, QWidget* parent) : row(row), column(column), QPushButton(parent)
{
    if (row < 0 || column < 0) { throw std::out_of_range("Invalid grid index: [" + std::to_string(row) + "][" + std::to_string(column) + "] for push button"); };
    connect(this,SIGNAL(clicked(bool)),this,SLOT(customClicked(bool)));
}

int CustomPushButton::getRow() const {
    return row;
}

int CustomPushButton::getColumn() const {
    return column;
}

void CustomPushButton::customClicked(bool)
{
    emit gridPosition(getRow(),getColumn());
}
