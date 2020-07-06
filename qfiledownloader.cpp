#include "qfiledownloader.h"

#include <QDataStream>
#include <QNetworkReply>
#include <QSslError>

QFileDownloader::QFileDownloader(QUrl source, QObject *parent)
    : QObject(parent), m_out(nullptr), m_reply(nullptr)
{
    connect(&m_nam, &QNetworkAccessManager::finished, this, &QFileDownloader::processFinished);
    auto reply = m_nam.get(QNetworkRequest(source));
    setupConnections(reply);
}

QFileDownloader::QFileDownloader(QUrl source, QString dest, QObject *parent)
    : QObject(parent), m_out(nullptr), m_reply(nullptr)
{
    m_fileSaveTo.setFileName(dest);
    if(!m_fileSaveTo.open(QIODevice::WriteOnly)){
        emit error(tr("Could not open/create the save file!"));
        return;
    }
    m_out = new QDataStream(&m_fileSaveTo);

    connect(&m_nam, &QNetworkAccessManager::finished, this, &QFileDownloader::processFinished);

    m_reply = m_nam.get(QNetworkRequest(source));
    setupConnections(m_reply);
}

void QFileDownloader::setupConnections(QNetworkReply *reply)
{
    connect(reply, &QNetworkReply::downloadProgress, this, &QFileDownloader::processProgress);
    connect(reply, QOverload<QNetworkReply::NetworkError >::of(&QNetworkReply::error), this, [=]()->void{
        emit error(reply->errorString());
    });
    connect(reply, &QNetworkReply::sslErrors,
            this, [=](const QList<QSslError> &errors)->void{
        for(const QSslError &sslError : errors)
            emit error(sslError.errorString());
    });
}

void QFileDownloader::processProgress(quint64 bytesReceived, quint64 bytesTotal)
{
    if(m_reply && m_reply->bytesAvailable()){
        QByteArray byteArray = m_reply->readAll();
        m_out->writeBytes(byteArray, byteArray.size());
    }
    //Forward Signal
    emit downloadProgress(bytesReceived, bytesTotal);
}

void QFileDownloader::processFinished(QNetworkReply *reply)
{
    if(m_reply){
        QByteArray byteArray = m_reply->readAll();
        m_out->writeBytes(byteArray, byteArray.size());
        m_fileSaveTo.close();

        delete m_out;
        m_out = nullptr;

        m_reply->deleteLater();
        m_reply = nullptr;
    } else {
        m_data = reply->readAll();
        reply->deleteLater();
    }

    emit downloadFinished();
}
