#ifndef PHOTOPRINT_H
#define PHOTOPRINT_H

#include <QWidget>
#include "config.h"
#include "imageitem.h"
#include "imageview.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PhotoPrint; }
QT_END_NAMESPACE





class PhotoPrint : public QWidget
{
    Q_OBJECT

public:
    PhotoPrint(QWidget *parent = nullptr);
    ~PhotoPrint();
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);

    enum ViewEnum{
            viewConfig,
            viewThumbnails,
            viewImage,
            viewCart
    };

private slots:
    void on_pushButton_clicked();
    void start();

    void on_listWidget_itemActivated(QListWidgetItem *item);

    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_imageView_Pressed();

    void on_listWidget_itemChanged(QListWidgetItem *item);
    void fileSystemUpdated(QString string);

    void on_printButton_clicked();

private:
    Ui::PhotoPrint *ui;

    //Gui / Widgets:
    config *configWidget;
    void set_View(ViewEnum);
    ImageView *imgView;

    // control vars:
    ViewEnum currentView;


    // Images:
    //QList<ImageItem> imageList;
    QMap<QString,ImageItem*> imageMap;
    ImageItem* selectedImageItem;

    bool checkForNewImages(QString path);
    QFileSystemWatcher* watcher;

};
#endif // PHOTOPRINT_H
