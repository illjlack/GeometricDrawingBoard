#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_WorkWindow.h"

class WorkWindow : public QMainWindow
{
    Q_OBJECT

public:
    WorkWindow(QWidget *parent = nullptr);
    ~WorkWindow();

private:
    Ui::WorkWindowClass ui;
};
