#pragma once

#ifndef BLUETOOTH_WINDOW_H
#define BLUETOOTH_WINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_BluetoothWindow.h"
#include "BluetoothModule.h"

class BluetoothWindow : public QMainWindow
{
    Q_OBJECT

public:
    BluetoothWindow(QWidget *parent = nullptr);
    ~BluetoothWindow();

private slots:
    void changePowerButtonView();
    void changeBluetoothPowerState();
    void on_scanButton_clicked();
    void on_pairingButton_clicked();
    void on_unPairingButton_clicked();

private:
    Ui::BluetoothWindowClass ui;
    BluetoothManager* manager;
};

#endif // !BLUETOOTH_WINDOW_H