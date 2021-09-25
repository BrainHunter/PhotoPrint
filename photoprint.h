#ifndef PHOTOPRINT_H
#define PHOTOPRINT_H

#include <QWidget>
#include "config.h"
#include "imageitem.h"
#include "imageview.h"
#include "filecopyrunner.h"

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
            viewCart,
            viewPrintConfirmation,
            viewPrintActive
    };

private slots:
    void on_pushButton_clicked();
    void start();

    void on_listWidget_itemActivated(QListWidgetItem *item);

    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_imageViewpressed();

    void on_listWidget_itemChanged(QListWidgetItem *item);
    void fileSystemUpdated(QString string);
    void fileCopyFinished(QString destinationFile);

    void on_printButton_clicked();

    void printActiveTimerTimeout();

    void on_backButton_clicked();

    void on_nextButton_clicked();

    void on_prevButton_clicked();

    void thumbnailScrollDownTimer_Timeout();
    void listWidget_ScrollerStateChanged(QScroller::State newState);

private:
    Ui::PhotoPrint *ui;

    //Gui / Widgets:
    config *configWidget;
    void set_View(ViewEnum);
    ImageView *imgView;
    ImageView *printActiveView;

    // control vars:
    ViewEnum currentView;


    // Images:
    //QList<ImageItem> imageList;
    QMap<QString,ImageItem*> imageMap;
    ImageItem* selectedImageItem;

    bool checkForNewImages(QString path);
    void addNewImage(QString file);
    QFileSystemWatcher* watcher;

    //Timer for Printactive message
    QTimer* printActiveTimer;
    ViewEnum printActiveViewMarker;

    // Timer for automatic scroll down:
    QTimer* thumbnailScrollDownTimer;


};
#endif // PHOTOPRINT_H
