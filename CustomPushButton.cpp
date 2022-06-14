#include "CustomPushButton.h"

CustomPushButton::CustomPushButton(int row, int column, QWidget* parent) : row(row), column(column), QPushButton(parent)
{
    connect(this,SIGNAL(clicked(bool)),this,SLOT(customClicked(bool)));
}

int CustomPushButton::getRow(){
    return row;
}

int CustomPushButton::getColumn(){
    return column;
}

void CustomPushButton::customClicked(bool)
{
    emit gridPosition(getRow(),getColumn());
}
