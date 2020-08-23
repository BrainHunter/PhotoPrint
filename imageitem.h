#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <QObject>
#include <QtWidgets/QListWidget>

class ImageItem : public QObject, public QListWidgetItem
{
    Q_OBJECT
public:
    explicit ImageItem(QString filename, QSize ThumbnailSize=QSize(200,200), QObject *parent = nullptr);
    ~ImageItem();
    QString         filename;
    //QListWidgetItem *listWidgetItem;
    QIcon           icon;

    // read the image from the harddisk again and return:
    QImage          getImage();

public slots:
    void thumbnailLoaded(QString file, QImage img);

private:
    QString bla;
    QObject *parent;
    QSize ThumbnailSize;

signals:

};

#endif // IMAGEITEM_H
