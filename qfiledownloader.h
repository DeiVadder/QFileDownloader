#ifndef QFILEDOWNLOADER_H
#define QFILEDOWNLOADER_H

#include <QNetworkAccessManager>
#include <QFile>

class QDataStream;
class QFileDownloader : public QObject
{
    Q_OBJECT
public:
    explicit QFileDownloader(QUrl source,QObject *parent = nullptr);
    explicit QFileDownloader(QUrl source, QString dest, QObject *parent = nullptr);

    inline const QByteArray &downloadedData() const {return  m_data;}

signals:
    void downloadFinished();
    void downloadProgress(quint64 bytesReceived, quint64 bytesTotal);

    void error(QString errorString);

private slots:
    void setupConnections(QNetworkReply *reply);
    void processProgress(quint64 bytesReceived, quint64 bytesTotal);
    void processFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager m_nam;
    QByteArray m_data;

    QFile m_fileSaveTo;

    QDataStream *m_out;
    QNetworkReply *m_reply;

};

#endif // QFILEDOWNLOADER_H
