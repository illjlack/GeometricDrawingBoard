#pragma once

#include <QtWidgets/QWidget>
#include "ui_Work.h"

class Work : public QWidget
{
    Q_OBJECT

public:
    Work(QWidget *parent = nullptr);
    ~Work();

private:
    Ui::WorkClass ui;
};
