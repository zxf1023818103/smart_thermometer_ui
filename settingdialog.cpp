#include "settingdialog.h"
#include "ui_settingdialog.h"
#include <QSerialPortInfo>
#include <QSizePolicy>
#include <QVariant>

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    ports = QSerialPortInfo::availablePorts();
    for (auto port : ports) {
        ui->serialPortsComboBox->addItem(port.systemLocation() + " (" + port.description() + ")");
    }
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::on_buttonBox_accepted()
{
    ui->buttonBox->setDisabled(true);
    if (!ports.empty()) {
        emit settingsChanged(ports[ui->serialPortsComboBox->currentIndex()],
                ui->endPointLineEdit->text(), ui->deviceNameLineEdit->text(),
                ui->deviceSecretLineEdit->text(),
                ui->productKeyLineEdit->text());
    }
    ui->buttonBox->setDisabled(false);
    close();
}

void SettingDialog::on_buttonBox_rejected()
{
    ui->buttonBox->setDisabled(true);
    close();
    ui->buttonBox->setDisabled(false);
}
