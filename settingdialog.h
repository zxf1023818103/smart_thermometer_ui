#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QSerialPortInfo>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();

signals:
    void settingsChanged(const QSerialPortInfo& serialPortInfo,
                         const QString& endPoint,
                         const QString& deviceName,
                         const QString& deviceSecret,
                         const QString& productKey);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::SettingDialog *ui;

    QList<QSerialPortInfo> ports;
};

#endif // SETTINGDIALOG_H
