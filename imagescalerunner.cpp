#include "imagescalerunner.h"
#include <QImageReader>
#include <QThread>

ImageScaleRunner::ImageScaleRunner(QString file, QSize s, QObject *parent) : QObject(parent)
{
    this->filename = file;
    this->size = s;
}

void ImageScaleRunner::run()
{

    QImageReader reader(filename);

    #if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    reader.setAutoTransform(true);
    #endif

    QImage Image;
    Image = reader.read();
    if( Image.isNull())  // image couldn't be loaded. maybe its not completely saved. retry in 2 sec:
    {
        QThread::sleep(2);
        Image = reader.read();
    }

    QImage newImage = Image.scaled(this->size,Qt::KeepAspectRatio );

    emit finished(filename, newImage);

}
