#pragma once

#include <QDialog>

namespace Ui {
class AIPromptWindow;
}

class AIPromptWindow : public QDialog
{
    Q_OBJECT
public:
    explicit AIPromptWindow(QWidget *parent = nullptr);
    ~AIPromptWindow();

private:
    Ui::AIPromptWindow* UI;

signals:
    void humanColorSelection(std::string color);
private slots:
    void on_redColorButton_clicked();
    void on_blackColorButton_clicked();
};

