#include "AIPromptWindow.h"
#include "ui_AIPromptWindow.h"

AIPromptWindow::AIPromptWindow(QWidget *parent) :
    QDialog(parent),
    UI(new Ui::AIPromptWindow)
{
    UI->setupUi(this);
    setWindowTitle("Pick a color");
    UI->redColorButton->setIcon(QPixmap(":/Resources/Images/red_dot.png"));
    UI->blackColorButton->setIcon(QPixmap(":/Resources/Images/black_dot.png"));
}

AIPromptWindow::~AIPromptWindow()
{
    delete UI;
}

void AIPromptWindow::on_redColorButton_clicked()
{
    emit humanColorSelection("RED");
    this->hide();
}


void AIPromptWindow::on_blackColorButton_clicked()
{
    emit humanColorSelection("BLACK");
    this->hide();
}

