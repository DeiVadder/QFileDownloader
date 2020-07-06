#include <QGuiApplication>

#include "qfiledownloader.h"

#include <QDebug>
#include <QStandardPaths>
#include <QDesktopServices>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    //Save to ram
    QFileDownloader qfdl(QUrl("https://file-examples.com/wp-content/uploads/2017/10/file-sample_150kB.pdf"));

    QObject::connect(&qfdl, &QFileDownloader::downloadFinished, [&qfdl]()->void{
                         qDebug() << "Download Finished:" << qfdl.downloadedData().size();});
    QObject::connect(&qfdl, &QFileDownloader::downloadProgress, [](quint64 bytes, quint64 total)->void{
        qDebug() << "Downloaded:" << bytes << " of "<< total;
    });

    //Save to disk
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/TestFile.pdf";
    qDebug() << "SaveToFile:" << savePath;
    QFileDownloader qfdlDisc(QUrl("https://file-examples.com/wp-content/uploads/2017/10/file-sample_150kB.pdf"),savePath);

    QObject::connect(&qfdlDisc, &QFileDownloader::downloadFinished, [&savePath]()->void{
                         qDebug() << "Download (Disc) Finished:";
                         QDesktopServices::openUrl(QUrl::fromLocalFile(savePath));
                     });
    QObject::connect(&qfdlDisc, &QFileDownloader::downloadProgress, [](quint64 bytes, quint64 total)->void{
        qDebug() << "Downloaded (Disc):" << bytes << " of "<< total;
    });

    return a.exec();
}
