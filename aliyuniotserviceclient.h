#ifndef ALIYUNIOTSERVICECLIENT_H
#define ALIYUNIOTSERVICECLIENT_H

#include <QByteArray>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageAuthenticationCode>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QObject>

class AliyunIotServiceClient : public QObject
{
    Q_OBJECT
private:
    QString m_productKey;

    QString m_deviceSecret;

    QString m_deviceName;

    QString m_clientId;

    QString m_endPoint;

    QString m_token;

    QString m_errorMessage;

    QNetworkAccessManager m_accessManager;

    int m_id;

private:
    void updateToken()
    {
        if (!m_endPoint.isEmpty() && !m_deviceName.isEmpty() && !m_deviceSecret.isEmpty() && !m_productKey.isEmpty() && !m_clientId.isEmpty()) {
            QNetworkRequest networkRequest;
            networkRequest.setUrl(m_endPoint + "/auth");
            networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            QString timestamp = QString::number(QDateTime::currentDateTime().toUTC().toTime_t());
            QString message = "clientId" + m_clientId + "deviceName" + m_deviceName + "productKey" + m_productKey;
            QByteArray sign = QMessageAuthenticationCode::hash(message.toLatin1(), m_deviceSecret.toLatin1(), QCryptographicHash::Md5);
            QString body = "{ \"productKey\": \"" + m_productKey
                    + "\", \"deviceName\": \"" + m_deviceName
                    + "\", \"clientId\": \"" + m_clientId
                    + "\", \"sign\": \"" + sign.toHex() + "\" }";
            m_accessManager.post(networkRequest, body.toLatin1());
        }
    }

public:
    explicit AliyunIotServiceClient(QObject *parent = nullptr)
    {
        (void)parent;
        connect(&m_accessManager, &QNetworkAccessManager::finished, this, &AliyunIotServiceClient::receiveReply);
    }

    ~AliyunIotServiceClient()
    {
        disconnect(&m_accessManager, &QNetworkAccessManager::finished, this, &AliyunIotServiceClient::receiveReply);
    }

    const QString& deviceSecret() { return m_deviceSecret; }

    const QString& productKey() { return m_productKey; }

    const QString& deviceName() { return m_deviceName; }

    const QString& endPoint() { return m_endPoint; }

    const QString& errorMessage() { return m_errorMessage; }

    const QString& clientId() { return m_clientId; }

    void setDeviceSecret(const QString& deviceSecret) { m_deviceSecret = deviceSecret; updateToken(); }

    void setProductKey(const QString& productKey) { m_productKey = productKey; updateToken(); }

    void setDeviceName(const QString& deviceName) { m_deviceName = deviceName; updateToken(); }

    void setEndPoint(const QString& endPoint) { m_endPoint = endPoint; updateToken(); }

    void setClientId(const QString& clientId) { m_clientId = clientId; updateToken(); }

    void reportTemperature(qreal temperature)
    {

        if (!m_endPoint.isEmpty() && !m_deviceName.isEmpty() && !m_deviceSecret.isEmpty() && !m_productKey.isEmpty() && !m_clientId.isEmpty()) {
            QNetworkRequest networkRequest;
            networkRequest.setUrl(m_endPoint + "/topic/sys/" +m_productKey + "/" + m_deviceName + "/thing/event/property/post");
            networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
            networkRequest.setRawHeader("password", m_token.toLatin1());
            QString content = "{ \"id\": " + QString::number(++m_id)
                    + ", \"version\": \"1.0\", \"params\": { \"CurrentTemperature\": { \"value\": "
                    + QString::number(temperature) + " }, \"method\": \"thing.event.property.post\" } }";
            m_accessManager.post(networkRequest, content.toLatin1());
        }
    }

signals:
    void errorOccured(const QString &errorString);
    void finished(int messageId);

private slots:
    void receiveReply(QNetworkReply *reply)
    {
        if (reply->error() != QNetworkReply::NoError) {
            m_errorMessage = reply->errorString();
            emit errorOccured(m_errorMessage);
        }
        else {
            QJsonDocument document;
            QByteArray data = reply->readAll();
            //qDebug() << data;
            document = document.fromJson(data);
            //qDebug() << document.toJson();
            auto object = document.object();
            int code;
            QString message;
            int id = 0;
            for (auto i = object.begin(); i != object.end(); i++) {
                QString key = i.key();
                if (key == "code") {
                    code = i.value().toInt();

                }
                else if (key == "message") {
                    message = i.value().toString();
                }
                else if (key == "info") {
                    auto info = i.value().toObject();
                    for (auto j = info.begin(); j != info.end(); j++) {
                        key = j.key();
                        if (key == "token") {
                            m_token = j.value().toString();
                            return;
                        }
                        else if (key == "messageId") {
                            id = j.value().toInt();
                        }
                    }
                }
            }
            if (code == 0) {
                // messageId
            }
            else {
                m_errorMessage = message;
                emit errorOccured(m_errorMessage);
            }
        }
    }
};

#endif // ALIYUNIOTSERVICECLIENT_H
