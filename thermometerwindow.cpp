#include "thermometerwindow.h"
#include "ui_thermometerwindow.h"
#include <stdio.h>

ThermometerWindow::ThermometerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ThermometerWindow)
{
    ui->setupUi(this);
    ui->lcdNumber->setDecMode();
    ui->lcdNumber->setDigitCount(5);
    ui->lcdNumber->setSmallDecimalPoint(true);
    connect(&thermometer, &Thermometer::temperatureChanged, this, &ThermometerWindow::on_temperature_changed);
    connect(&thermometer, &Thermometer::errorOccurred, this, &ThermometerWindow::on_error_occurred);
    connect(&client, &AliyunIotServiceClient::errorOccured, this, &ThermometerWindow::on_error_occurred);
}

ThermometerWindow::~ThermometerWindow()
{
    delete ui;
}

void ThermometerWindow::on_settingsDialog_closed()
{
    dialog = nullptr;
}

void ThermometerWindow::on_settingsButton_clicked()
{
    if (dialog != nullptr) {
        dialog->hide();
    }
    else {
        dialog = new SettingDialog();
    }
    connect(dialog, &SettingDialog::accepted, this, &ThermometerWindow::on_settingsDialog_closed);
    connect(dialog, &SettingDialog::rejected, this, &ThermometerWindow::on_settingsDialog_closed);
    connect(dialog, &SettingDialog::settingsChanged, this, &ThermometerWindow::on_settings_changed);
    dialog->show();
}

void ThermometerWindow::on_settings_changed(const QSerialPortInfo& serialPortInfo,
                                            const QString& endPoint,
                                            const QString& deviceName,
                                            const QString& deviceSecret,
                                            const QString& productKey) {
    thermometer.setSerialPort(serialPortInfo);
    if (!endPoint.isEmpty() && !deviceName.isEmpty() && !deviceSecret.isEmpty() && !productKey.isEmpty()) {
        client.setEndPoint(endPoint);
        client.setDeviceName(deviceName);
        client.setDeviceSecret(deviceSecret);
        client.setProductKey(productKey);
        client.setClientId("zxf");
    }
}

void ThermometerWindow::on_temperature_changed(qreal temperature) {
    char digits[10] = {0};
    sprintf(digits, "%.2f", temperature);
    ui->lcdNumber->display(digits);
    client.reportTemperature(temperature);
}

void ThermometerWindow::on_error_occurred(const QString& errorString) {
    ui->statusbar->showMessage(errorString);
}
