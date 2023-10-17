#include "BluetoothWindow.h"

BluetoothWindow::BluetoothWindow(QWidget* parent) :
    QMainWindow(parent)
{
    ui.setupUi(this);
    manager = new BluetoothManager(); ;
    manager->m_textBrowser = ui.textBrowser;
    manager->m_devicesListWidget = ui.devicesListWidget;
    manager->m_pairingLabel = ui.pairingLabel;

    connect(manager, &BluetoothManager::initFinished, this, &BluetoothWindow::changePowerButtonView);
//    connect(ui.powerButton, &QPushButton::clicked, ui.powerButton, [=]() { ui.powerButton->setEnabled(false); });
    connect(ui.powerButton, &QPushButton::clicked, this, &BluetoothWindow::changeBluetoothPowerState);
    connect(manager, &BluetoothManager::radioPowerStateChanged, ui.powerButton, [=]() { ui.powerButton->setEnabled(true); });
    connect(manager, &BluetoothManager::radioPowerStateChanged, this, &BluetoothWindow::changePowerButtonView);
}

BluetoothWindow::~BluetoothWindow() {
}

void BluetoothWindow::changePowerButtonView() {
    if (manager->getRadioPowerState() == 1) {
        ui.powerButton->setText("Включен");
        ui.powerButton->setStyleSheet(
            "QPushButton{background-color: rgb(130,224,170);}"
            "QPushButton:pressed{background-color: rgb(125,206,160);}"
            "QPushButton:disabled{background-color: rgb(125,206,160);}"
        );
        ui.scanButton->setEnabled(true);
    }
    else {
        ui.powerButton->setText("Отключен");
        ui.powerButton->setStyleSheet(
            "QPushButton{background-color: rgb(241,148,138);}"
            "QPushButton:pressed{background-color: rgb(217,136,128);}"
            "QPushButton:disabled{background-color: rgb(217,136,128);}"
        );
        ui.scanButton->setEnabled(false);
    }
}

void BluetoothWindow::changeBluetoothPowerState() {
    if (manager->getRadioPowerState() == 1)
        manager->setRadioPowerState(0);
    else if (manager->getRadioPowerState() == 0)
        manager->setRadioPowerState(1);
}

void BluetoothWindow::on_scanButton_clicked()
{
    manager->startBluetoothDiscovery();
}

void BluetoothWindow::on_pairingButton_clicked()
{
    manager->pairing(ui.devicesListWidget->currentRow());
}

void BluetoothWindow::on_unPairingButton_clicked()
{
    manager->unPairing(ui.devicesListWidget->currentRow());
}