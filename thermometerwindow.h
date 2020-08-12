#ifndef THERMOMETERWINDOW_H
#define THERMOMETERWINDOW_H

#include <QMainWindow>
#include "aliyuniotserviceclient.h"
#include "thermometer.h"
#include "settingdialog.h"

namespace Ui {
class ThermometerWindow;
}

class ThermometerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ThermometerWindow(QWidget *parent = nullptr);
    ~ThermometerWindow();

private slots:
    void on_settingsButton_clicked();
    void on_settingsDialog_closed();
    void on_settings_changed(const QSerialPortInfo& serialPortInfo,
                             const QString& endPoint,
                             const QString& deviceName,
                             const QString& deviceSecret,
                             const QString& productKey);
    void on_temperature_changed(qreal temperature);
    void on_error_occurred(const QString& errorString);

private:
    Ui::ThermometerWindow *ui;
    SettingDialog *dialog = nullptr;
    Thermometer thermometer;
    AliyunIotServiceClient client;
};

#endif // THERMOMETERWINDOW_H
