#ifndef THERMOMETER_H
#define THERMOMETER_H

#include <stdlib.h>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class Thermometer : public QObject
{
    Q_OBJECT
private:
    qreal m_temperature;

    QSerialPort m_serialPort;

    QString m_errorString;

    QString m_data;

public:
    explicit Thermometer(QObject *parent = nullptr) { (void)parent; }

    const QString& errorString() { return m_errorString; }

    qreal temperature() { return m_temperature; }

    bool setSerialPort(const QSerialPortInfo &serialPortInfo) {
        if (m_serialPort.isOpen()) {
            disconnect(&m_serialPort, &QSerialPort::readyRead, this, &Thermometer::updateTemperature);
            m_serialPort.clear();
            m_serialPort.close();
        }
        m_serialPort.setPort(serialPortInfo);
        if (!m_serialPort.open(QIODevice::ReadOnly)) {
            m_errorString = m_serialPort.errorString();
            emit errorOccurred(m_errorString);
            return false;
        }

        m_serialPort.setBaudRate(QSerialPort::Baud4800, QSerialPort::AllDirections);
        m_serialPort.setDataBits(QSerialPort::Data8);
        m_serialPort.setFlowControl(QSerialPort::NoFlowControl);
        m_serialPort.setParity(QSerialPort::NoParity);
        m_serialPort.setStopBits(QSerialPort::OneStop);

        connect(&m_serialPort, &QSerialPort::readyRead, this, &Thermometer::updateTemperature);
        connect(&m_serialPort, &QSerialPort::errorOccurred, this, &Thermometer::handleSerialPortError);
        return true;
    }

    ~Thermometer()
    {
        disconnect(&m_serialPort, &QSerialPort::readyRead, this, &Thermometer::updateTemperature);
        disconnect(&m_serialPort, &QSerialPort::errorOccurred, this, &Thermometer::handleSerialPortError);
    }

signals:
    void temperatureChanged(qreal temperature);
    void errorOccurred(const QString& errorString);

private slots:
    void updateTemperature() {
        if (m_serialPort.canReadLine()) {
            char line[10];
            m_serialPort.readLine(line, sizeof line);
            m_temperature = strtod(line, NULL);
            emit temperatureChanged(m_temperature);
        }
    }

    void handleSerialPortError(QSerialPort::SerialPortError error) {
        (void)error;
        m_errorString = m_serialPort.errorString();
        emit errorOccurred(m_errorString);
    }
};

#endif // THERMOMETER_H
