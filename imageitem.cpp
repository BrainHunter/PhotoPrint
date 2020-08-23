#include "imageitem.h"
#include <QImageReader>
#include <QDebug>
#include "imagescalerunner.h"
#include <QThreadPool>

ImageItem::ImageItem(QString filename,  QSize ThumbnailSize, QObject *parent) :QObject(parent), QListWidgetItem()
{
    this->parent = parent;
    this->filename = filename;
    this->ThumbnailSize = ThumbnailSize;

    // read the image and scale:
    //QImageReader reader(filename);
    //QStringList sl = reader.textKeys();
    //#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    //reader.setAutoTransform(true);
    //#endif
    //const QImage newImage = reader.read().scaled(200,200,Qt::KeepAspectRatio );
    //const QImage scaledImage = newImage.scaled(200,200,Qt::KeepAspectRatio );
    //icon =  QIcon(QPixmap::fromImage(newImage));
    //this->setIcon(icon);

    //instead do this threaded to improve loading:

    // threaded loading:
    ImageScaleRunner *runner = new ImageScaleRunner(filename, this->ThumbnailSize);  // do not set parent of runner thread as emit will fail
    QObject::connect(runner, SIGNAL(finished(QString, QImage)), this, SLOT(thumbnailLoaded(QString, QImage)),Qt::QueuedConnection);
    QThreadPool::globalInstance()->start(runner);

}


ImageItem::~ImageItem()
{

}

QImage ImageItem::getImage()
{
    QImageReader reader(filename);

    #if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    reader.setAutoTransform(true);
    #endif

    return reader.read();

}

void ImageItem::thumbnailLoaded(QString file, QImage img)
{
    qDebug() << "thumbLoaded receiving " << file;
    if(file != (this->filename))
        qDebug() << "file != filename"  << file << " != " << filename ;
    icon =  QIcon(QPixmap::fromImage(img));
    this->setIcon(icon);
}
