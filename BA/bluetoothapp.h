#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_bluetoothapp.h"

class BluetoothApp : public QMainWindow
{
    Q_OBJECT

public:
    BluetoothApp(QWidget *parent = Q_NULLPTR);

private:
    Ui::BluetoothAppClass ui;
};
