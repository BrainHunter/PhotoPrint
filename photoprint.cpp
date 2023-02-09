#include "photoprint.h"
#include "ui_photoprint.h"

#include "filecopyrunner.h"
#include <QPrintPreviewDialog>


PhotoPrint::PhotoPrint(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PhotoPrint)
    , configWidget(new config)
    , imgView(new ImageView(this))
    , printActiveView(new ImageView(this))
    , watcherRestartTimer(new QTimer(this))
    , printActiveTimer(new QTimer(this))
    , thumbnailScrollDownTimer(new QTimer(this))
    , returnToThumbnailViewTimer(new QTimer(this))
{
    ui->setupUi(this);

    // no image is selected at the beginig:
    selectedImageItem = NULL;

    // ----  thumbnail view list  ----
    ui->listWidget->setViewMode(QListWidget::IconMode);
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setIconSize(QSize(200,200));
    //ui->listWidget->setResizeMode(QListWidget::Adjust);
    //ui->listWidget->setSelectionRectVisible(true);
    ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    //ui->listWidget->addItem(new QListWidgetItem(QIcon("image.jpg"),NULL));
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // make this list widget as big as the window is
    ui->listWidget->setGeometry(0,0,this->size().width(),this->size().height() );

    // list widget background color:
    ui->listWidget->setStyleSheet("QListWidget{ background: black; } ");
    ui->listWidget->setFrameShape(QFrame::NoFrame); // this removes the white border
    // enable sorting in the listWidget
    ui->listWidget->setSortingEnabled(true);


    // ---- scrolling thingy ----
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::DragVelocitySmoothingFactor, 0.6);
    sp.setScrollMetric(QScrollerProperties::MinimumVelocity, 0.0);
    sp.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.5);
    sp.setScrollMetric(QScrollerProperties::AcceleratingFlickMaximumTime, 0.4);
    sp.setScrollMetric(QScrollerProperties::AcceleratingFlickSpeedupFactor, 1.2);
    sp.setScrollMetric(QScrollerProperties::SnapPositionRatio, 0.2);
    sp.setScrollMetric(QScrollerProperties::MaximumClickThroughVelocity, 0);
    sp.setScrollMetric(QScrollerProperties::DragStartDistance, 0.001);
    sp.setScrollMetric(QScrollerProperties::MousePressEventDelay, 0.5);

    QScroller* scroller = QScroller::scroller(ui->listWidget);
    scroller->grabGesture(ui->listWidget, QScroller::LeftMouseButtonGesture);
    scroller->setScrollerProperties(sp);

    connect(scroller,SIGNAL(stateChanged(QScroller::State)), this, SLOT(listWidget_ScrollerStateChanged(QScroller::State)));

    // thumbnailScrollDownTimer
    thumbnailScrollDownTimer->setSingleShot(true);
    connect(thumbnailScrollDownTimer,SIGNAL(timeout()),this,SLOT(thumbnailScrollDownTimer_Timeout()));

    // returnTothumbnailView Timer
    returnToThumbnailViewTimer->setSingleShot(true);
    connect(returnToThumbnailViewTimer,SIGNAL(timeout()),this,SLOT(returnToThumbnailViewTimer_Timeout()));

    // ---- config widget ----
    configWidget->setParent(this);
    connect(configWidget, SIGNAL(StartButton_clicked()), this, SLOT(start()));
    //qDebug() << configWidget->palette().color(QWidget::backgroundRole()).name();


    // ---- Image View ----
    imgView->hide();
    connect(imgView, SIGNAL(Mouse_pressed()), this, SLOT(on_imageViewpressed()));

    // ---- print Active view ----
    printActiveView->hide();
    printActiveView->setImage(QImage("icons/printActive.png"));
    // print Active timer:
    printActiveTimer->setSingleShot(true);
    connect(printActiveTimer,SIGNAL(timeout()),this,SLOT(printActiveTimerTimeout()));


    // ----- File System checker -----
    watcher = new QFileSystemWatcher(this);
    connect(watcher,SIGNAL(directoryChanged(QString)),this,SLOT(fileSystemUpdated(QString)));

    watcherRestartTimer->setSingleShot(true);
    connect(watcherRestartTimer,SIGNAL(timeout()),this,SLOT(watcherRestartTimerTimeout()));

    // --------  Buttons: --------
    // ---- printButton ----
    ui->printButton->hide();
    ui->printButton->setStyleSheet("background-color: rgba(255, 255, 255, 50);");
    if(QFile::exists("icons/print.png"))
    {
        ui->printButton->setIcon(QIcon("icons/print.png"));
        ui->printButton->setIconSize(QSize(100,100));
        ui->printButton->setText("");
    }
    else
    {
        ui->printButton->setText("Print");
    }
    // ---- cartButton ----
    ui->cartButton->hide();
    ui->cartButton->setStyleSheet("background-color: rgba(255, 255, 255, 50);");
    if(QFile::exists("icons/cart.png"))
    {
        ui->cartButton->setIcon(QIcon("icons/cart.png"));
        ui->cartButton->setIconSize(QSize(100,100));
        ui->cartButton->setText("");
    }
    else
    {
        ui->cartButton->setText("TO CART");
    }

    // ---- backButton ----
    ui->backButton->hide();
    ui->backButton->setStyleSheet("background-color: rgba(255, 255, 255, 50);");
    if(QFile::exists("icons/back.png"))
    {
        ui->backButton->setIcon(QIcon("icons/back.png"));
        ui->backButton->setIconSize(QSize(100,100));
        ui->backButton->setText("");
    }
    else
    {
        ui->backButton->setText("↶"); // unicode: U+21B6
        QFont font = ui->backButton->font();
        font.setBold(true);
        font.setPointSize(100);
        ui->backButton->setFont(font);
    }

    // ---- next Button ----
    ui->nextButton->hide();
    ui->nextButton->setStyleSheet("background-color: rgba(255, 255, 255, 50);");
    if(QFile::exists("icons/next.png"))
    {
        ui->nextButton->setIcon(QIcon("icons/next.png"));
        ui->nextButton->setIconSize(QSize(100,100));
        ui->nextButton->setText("");
    }
    else
    {
        ui->nextButton->setText("❯"); // Unicode U+276F
        QFont font = ui->nextButton->font();
        font.setBold(true);
        font.setPointSize(100);
        ui->nextButton->setFont(font);

    }
    // ---- previousButton ----
    ui->prevButton->hide();
    ui->prevButton->setStyleSheet("background-color: rgba(255, 255, 255, 50);");
    if(QFile::exists("icons/previous.png"))
    {
        ui->prevButton->setIcon(QIcon("icons/previous.png"));
        ui->prevButton->setIconSize(QSize(100,100));
        ui->prevButton->setText("");
    }
    else
    {
        ui->prevButton->setText("❮");  // Unicode U+276E

        QFont font = ui->prevButton->font();
        font.setBold(true);
        font.setPointSize(100);
        ui->prevButton->setFont(font);

        //auto effect = new QGraphicsOpacityEffect(this);
        //effect->setOpacity(0.5);
        //ui->prevButton->setGraphicsEffect(effect);
        //ui->prevButton->setAutoFillBackground(true);
    }

    // -------- END  Buttons --------


    // ---- set default view ----
    set_View(viewConfig);
    this->resize(configWidget->size());

    // autostart
    if(configWidget->get_autostart())
    {
        start();
    }

}

PhotoPrint::~PhotoPrint()
{
    delete ui;
}


// check the directory for new images.
// if new images exist --> add to the imageFileList
// return true if new image added, else false
// note: this function will not remove images from the list.
bool PhotoPrint::checkForNewImages(QString path)
{
    bool ret =false;
    QStringList FilterList;
    FilterList << "*.jpg" << "*.JPG" << "*.BMP" ;

    QDir directory(path);
    QFileInfoList dirList= directory.entryInfoList(FilterList, QDir::NoFilter,QDir::Time);

    while( dirList.length() )
    {
        QFileInfo srcFile = dirList.last();
        QString tmp=srcFile.absoluteFilePath();
        dirList.removeLast();

        if(configWidget->get_localCopyEnabled())
        {
            //QFileInfo srcFile(tmp);
            QString dstFile = configWidget->get_LocalCopyPath() + "//" +  srcFile.fileName();

            if(!imageMap.contains(dstFile))
            {
                // check if file has size != 0
                if(srcFile.size()==0)
                {
                    qDebug() << "File is empty";
                    break;
                }

                // if copy is already running - do noting, this function is called again when running file copy is finished
                if(FileCopyRunner::isRunning()) //there might be a short time when running is false but finished signal is not emitted. so there is a chance that two copy processes get started.
                {
                    qInfo() << "File Copy is runnig: break ";
                    break;
                }

                // check if dst file is already copied:
                if(QFile::exists(dstFile))
                {
                    QFileInfo DstFileInfo(dstFile);
                    if( DstFileInfo.size() != srcFile.size())
                    {
                        qInfo() << "Destination already exists but is not same size " << tmp;
                        continue;
                    }
                    addNewImage(dstFile);
                    ret = true;
                    continue;
                }

                // Start the file copy runner:
                FileCopyRunner *runner = new FileCopyRunner(tmp, dstFile);  // do not set parent of runner thread as emit will fail
                QObject::connect(runner, SIGNAL(finished(QString)), this, SLOT(fileCopyFinished(QString)),Qt::QueuedConnection);
                // QThreadPool deletes the QRunnable automatically by default. //delete will cause disconnect so everything should be fine
                QThreadPool::globalInstance()->start(runner);

                // only start one copy at a time
                // if there are more files to be copied over, this function will be called again by the fileCopyFinished slot
                ret = true;
                break;
            }
        }
        else
        {
            if(!imageMap.contains(tmp))
            {
                addNewImage(tmp);
                ret = true;
            }
        }

    }
    return ret;
}

// this is called indirectly by checkForNewImages via the copy runner
void PhotoPrint::fileCopyFinished(QString destinationFile)
{
    // add the newly copied file
    addNewImage(destinationFile);

    // check again for new images:
    checkForNewImages(configWidget->get_Image_Path());
}


void PhotoPrint::addNewImage(QString file)
{
    if(imageMap.contains(file))
    {
        qWarning() << "add new image is called with file alread in map: " << file;
        return;
    }

    ImageItem *it;
    it = new ImageItem(file, configWidget->get_ThumbnailSize());

    // insert into image map:
    imageMap.insert(file,it);
    qInfo() << "adding to map:" + file;

    // insert into widget
    ui->listWidget->addItem(it);
}


void PhotoPrint::set_View(ViewEnum e){

    thumbnailScrollDownTimer->stop(); // always stop the scroll down timer
    ui->remainingPaperLabel->hide();

    uint returnTimeout = configWidget->get_ReturnToThumbnailViewTimeout();

    switch(e)
    {
    case viewConfig:
        // return from fullsize
        this->showNormal();
        // restore normal cursor
        this->unsetCursor();
        // set normal color palette
        this->setPalette(this->style()->standardPalette());
        configWidget->show();
        ui->listWidget->hide();
        imgView->hide();
        printActiveView->hide();
        ui->printButton->hide();
        ui->cartButton->hide();
        ui->backButton->hide();
        ui->nextButton->hide();
        ui->prevButton->hide();
        // cancel the timers:
        printActiveTimer->stop(); // do not jump back to the image view

        currentView = viewConfig;
        break;
    case viewThumbnails:
        configWidget->hide();
        ui->listWidget->show();
        imgView->hide();
        printActiveView->hide();
        ui->printButton->hide();
        ui->cartButton->hide();
        ui->backButton->hide();
        ui->nextButton->hide();
        ui->prevButton->hide();

        configWidget->script_setViewThumbnails(); // execute the external script

        //Start the Scrolldown Timer:
        thumbnailScrollDownTimer->start(configWidget->get_ThumbnailScrollDownTimeout()*1000);

        currentView = viewThumbnails;
        break;
    case viewImage:
        imgView->show();
        printActiveView->hide();
        ui->listWidget->hide();
        configWidget->hide();
        if(configWidget->get_singlePrintEnable())
        {
            ui->printButton->show();
        }else
        {
            ui->printButton->hide();
        }
        ui->cartButton->setVisible(configWidget->get_shoppingCartEnable());
        ui->backButton->show();
        ui->nextButton->show();
        ui->prevButton->show();

        configWidget->script_setViewImage(selectedImageItem->filename); // execute the external script

        imgView->setGreyedOut(false);
        ui->printButton->setStyleSheet("background-color: rgba(255, 255, 255, 50);");
        ui->backButton->setStyleSheet("background-color: rgba(255, 255, 255, 50);");

        //restart the return timer
        if(returnTimeout >0)
        {
            returnToThumbnailViewTimer->start(returnTimeout*1000);
        }

        currentView = viewImage;
        break;
    case viewCart:
        configWidget->script_setViewCart(); // execute the external script
        printActiveView->hide();
        currentView = viewCart;
        break;
    case viewPrintConfirmation:
        ui->cartButton->hide();
        ui->nextButton->hide();
        ui->prevButton->hide();
        printActiveViewMarker = currentView;
        currentView = viewPrintConfirmation;
        imgView->setGreyedOut(true);
        ui->printButton->setStyleSheet("background-color: rgba(255, 255, 255, 200);");
        ui->backButton->setStyleSheet("background-color: rgba(255, 255, 255, 200);");

        //restart the return timer
        if(returnTimeout >0)
        {
            returnToThumbnailViewTimer->start(returnTimeout*1000);
        }
        showRemainingPaper();

        break;

    case viewPrintActive:

        printActiveView->show();
        printActiveView->resize(this->size());
        printActiveView->raise();

        ui->printButton->hide();
        ui->cartButton->hide();
        ui->nextButton->hide();
        ui->prevButton->hide();

        // start the printActiveTimer which restores the old view once timed out
        printActiveTimer->start(configWidget->get_printActiveTimeout()*1000);
        currentView = viewPrintActive;
        break;
    default:
       break;
    }

    // ---- place buttons: -----
    // Left side, top:
    int bx = 20;    // left
    int by = 20;     // top
    ui->backButton->setGeometry(bx,by,120,120);


    // Right side, top:
    bx = this->size().width() - 20 - 120;
    by = 20;
    if(ui->printButton->isVisible())
    {
        ui->printButton->setGeometry(bx, by, 120,120);
        by = by + 20 +120; // next button is further down
    }
    if(ui->cartButton->isVisible())
    {
        ui->cartButton->setGeometry(bx, by, 120,120);
        by = by + 20 + 120; // next button is further down
    }


    // left side, middle
    int windowMiddle = this->size().height()/2;

    ui->prevButton->setGeometry(20,windowMiddle-windowMiddle/2,120,windowMiddle);

    // rightside, middle
    ui->nextButton->setGeometry(bx,windowMiddle-windowMiddle/2,120,windowMiddle);



    // special placement for Print Confirmation
    if(currentView == viewPrintConfirmation)
    {
        int windowMiddleHeight = this->size().height()/2;
        int windowMiddleWidth = this->size().width()/2;
        ui->printButton->setGeometry(windowMiddleWidth-10-120, windowMiddleHeight, 120,120);
        ui->backButton->setGeometry(windowMiddleWidth+10,windowMiddleHeight,120,120);
    }



}

// test button:
void PhotoPrint::on_pushButton_clicked()
{
    //static int bla=0;
    //ui->listWidget->addItem(new QListWidgetItem(QIcon("image.jpg"),"button_"+QString::number(bla++)));

    // checkForNewImages(configWidget->get_Image_Path());

    //ui->listWidget->sortItems();

}

void PhotoPrint::start()
{

    // Set the Background of the widget to black
    QPalette pal;
    pal.setColor(QPalette::Background, Qt::black);
    this->setPalette(pal);

    ui->listWidget->setIconSize(configWidget->get_ThumbnailSize());
    set_View(viewThumbnails);

    imgView->cutHeight = configWidget->get_cutHeight();

    checkForNewImages(configWidget->get_Image_Path());

    // hide the pushbutton:
    ui->pushButton->hide();

    if(configWidget->get_showFullscreen())
    {
        this->showFullScreen();
        //this->setCursor(Qt::BlankCursor);
    }

    watcher->addPath(configWidget->get_Image_Path());
    watcherRestartTimer->start(120000); // watchdog for watcher
}

void PhotoPrint::keyPressEvent(QKeyEvent *event)
{
    //maybe need this some day:
//    bool ctrl_pressed = false;
//    if( event->modifiers() == Qt::ControlModifier )
//    {
//        ctrl_pressed = true;
//    }

    switch( event->key() )
    {
        case Qt::Key_Escape:
            watcher->removePaths(watcher->directories());
            watcherRestartTimer->stop();
            set_View(viewConfig);
            break;
        default:
            break;
    }
}

void PhotoPrint::mousePressEvent(QMouseEvent *event)
{
    if(currentView == viewImage)
    {
        set_View(viewThumbnails);
    }
}

void PhotoPrint::on_imageViewpressed()
{
    if(currentView == viewImage)
    {
        set_View(viewThumbnails);
    }
}

void PhotoPrint::on_listWidget_itemActivated(QListWidgetItem *item)
{
    qInfo() << "item activated";
}

void PhotoPrint::on_listWidget_itemClicked(QListWidgetItem *item)
{
    qInfo() << "item clicked";

    selectedImageItem = (ImageItem*)item;
    // get the filename:
    try {
        if(configWidget->get_OverlayShowOnPreview() && configWidget->get_OverlayEnabled())
        {
            QImage img = selectedImageItem->getImage();
            QPainter p(&img);

            QRect pRect = configWidget->get_Printer()->pageRect();
            double xscale = double(img.width()) / pRect.width();
            double yscale = double(img.height()) / pRect.height();
            double scale = qMin(xscale, yscale);

            // scale the painter
            p.scale(scale, scale);

            PaintOverlay(&p, configWidget->get_OverlayImage(),
                         configWidget->get_OverlayDistance(),
                         configWidget->get_OverlayPosition(),
                         scale);
            p.end();
            imgView->setImage(img);
        }
        else
        {
            imgView->setImage(selectedImageItem->getImage());
        }

        imgView->resize(this->size());
    } catch (...) {
        qWarning() << "error loading image" << selectedImageItem->filename;
        // removing selected ImageItem from List and ImageMap:
        imageMap.remove(selectedImageItem->filename);
        delete selectedImageItem; // delete the item will also remove it from the list
        return;
    }


    //view image:
    set_View(viewImage);
}

void PhotoPrint::fileSystemUpdated(QString string){
    qInfo() << "Directory has changed! " + string;
    qDebug() << "watcher->dir " << watcher->directories();

    //check if the directory still exists:
    QFileInfo testDir(configWidget->get_Image_Path());
    qDebug() << "exists" << testDir.exists() << " isDir" << testDir.isDir()<< " is readable" << testDir.isReadable();

    if(testDir.exists() && testDir.isDir() && testDir.isReadable())
    {
        if(checkForNewImages(configWidget->get_Image_Path()))
        {
            watcherRestartTimer->start(120000);  // restart the 120sec - normal operation - since we just got a new image
        }
    }
    else
    {
        qWarning() << testDir.absoluteFilePath() << " is not available!";
        watcherRestartTimer->start(10000);
    }

}

void PhotoPrint::watcherRestartTimerTimeout()
{
    qWarning() << QTime::currentTime().toString() << "trying to add the path again ";
    if(!watcher->directories().isEmpty())
    {
        // remove all paths from watcher
        watcher->removePaths(watcher->directories());
    }
    if(watcher->addPath(configWidget->get_Image_Path()))
    {
        fileSystemUpdated(configWidget->get_Image_Path());
        watcherRestartTimer->start(120000);  // 120sec - normal operation - just in case
    }
    else
    {
        qWarning() << "could not add the path again " << configWidget->get_Image_Path() ;
        watcherRestartTimer->start(10000);  //10sec
    }
}

void PhotoPrint::on_listWidget_itemChanged(QListWidgetItem *item)
{
    ui->listWidget->sortItems();
}

void PhotoPrint::resizeEvent(QResizeEvent *event)
{
    // respect the image ratio for resizing the scrollArea
    //QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
    //                       tr("Size: %1 x %2")
    //                       .arg(event->size().width()).arg(event->size().height()));

    //qDebug() << "width: " << event->size().width() << " height: " << event->size().height();
    //qDebug() << "width: " << this->size().width() << " height: " << this->size().height();
    // there is a bug (maybe a feature) as the size returned by the event is not the final size
    // of the window when going into fullscreen. hence the size returned by this->size() is correct
    // it is easy to use this size.


    // resize list widget:
    ui->listWidget->setGeometry(0,0,this->size().width(),this->size().height() );
    ui->listWidget->sortItems();

    // resize the image view
    imgView->resize(this->size());

    // call the resize event of the base class
    QWidget::resizeEvent(event);
}

void PhotoPrint::on_printButton_clicked()
{

    if(currentView != viewPrintConfirmation)
    {
        set_View(viewPrintConfirmation);
    }
    else
    {
        if(selectedImageItem != NULL)
        {
            configWidget->script_prePrint(selectedImageItem->filename );
            // display the "printing dialog for the user."
            set_View(viewPrintActive);

            if(configWidget->get_externalPrintEnable())
            {
                // start this script blocking.. or maybe not?
                configWidget->script_externalPrint(selectedImageItem->filename );
            }
            else
            {
                QPrinter* printer = configWidget->get_Printer();
                qDebug() << "printer state" << printer->printerState();

                bool preview = configWidget->get_printPreviewEnabled();

                if(preview)
                {
                    QPrintPreviewDialog preview(printer, this);
                    preview.setWindowFlags ( Qt::Window );
                    connect(&preview, SIGNAL(paintRequested(QPrinter* )), SLOT(PaintPage(QPrinter* )));
                    preview.exec();
                }
                else
                {
                    PaintPage(printer);
                }
                qDebug() << "printer state" << printer->printerState();
                qDebug() << "printer state" << printer->printerState();
            }
        }
    }
}

void PhotoPrint::PaintPage(QPrinter* printer){
    QImage img = selectedImageItem->getImage();
    QPainter painter(printer);
    double xscale = printer->pageRect().width() / double(img.width());
    double yscale = printer->pageRect().height() / double(img.height());
    //double xscale = printer->paperRect().width() / double(img.width());
    //double yscale = printer->paperRect().height() / double(img.height());
    double scale = qMax(xscale, yscale);
    qDebug() << "pageRect().width " << printer->pageRect().width() << " pageRect().height " << printer->pageRect().height();
    qDebug() << "img.width " << img.width()<< " img.height " << img.height();
    qDebug() << "scale " << scale;

    // scaled image size is not used but easier to compare to the page size when debugging
    uint scaledImageWidth = img.width()*scale;
    uint scaledImageHeight = img.height()*scale;
    qDebug() << "scaled Image size " << scaledImageWidth << "x" << scaledImageHeight;

    //scale the painter coordinate system to match the image
    painter.scale(scale, scale);

    // calculate the origin of the image
    int printerWidthScaled = printer->pageRect().width()/scale;
    int printerHeightScaled = printer->pageRect().height()/scale;
    int x = (printerWidthScaled - img.width()) /2;
    int y = (printerHeightScaled - img.height()) /2;

    // draw the image at the calculated origin
    painter.drawImage(QPoint(x,y),img);

    // reset scale
    painter.resetTransform();

    // draw overlay image:
    if(configWidget->get_OverlayEnabled())
    {
        PaintOverlay(&painter,
                     configWidget->get_OverlayImage(),
                     configWidget->get_OverlayDistance(),
                     configWidget->get_OverlayPosition());
    }

    // draw QRCode
    if(configWidget->get_QRCodeEnabled())
    {
        PaintOverlay(&painter,
                     configWidget->get_QRCodeImage(),
                     configWidget->get_QRCodeDistance(),
                     configWidget->get_QRCodePosition());
    }

    painter.end();
}


void PhotoPrint::PaintOverlay(QPainter* painter, QImage* Image, uint distance, enum config::QRCodePosition pos , double scale)
{
    QRect r = painter->viewport();
    qDebug() << "PaintOverlay viewport: " << r;
    uint scaledImageWidth = r.width()/scale;
    uint scaledImageHeight = r.height()/scale;
    uint x=0,y=0;

    uint overlayWidth = Image->width();
    uint overlayHeight = Image->height();

    qDebug() << "overlay image size: " << overlayWidth << "x" <<overlayHeight ;

    switch(pos)
    {
    case config::QRPosTopLeft:
        x = distance;
        y = distance;
        break;
    case config::QRPosTopCenter:
        x = scaledImageWidth/2 - overlayWidth/2;
        y = distance;
        break;
    case config::QRPosTopRight:
        x = scaledImageWidth - overlayWidth - distance;
        y = distance;
        break;
    case config::QRPosCenterLeft:
        x = distance;
        y = scaledImageHeight/2 - overlayHeight/2;
        break;
    case config::QRPosCenterCenter:
        x = scaledImageWidth/2 - overlayWidth/2;
        y = scaledImageHeight/2 - overlayHeight/2;
        break;
    case config::QRPosCenterRight:
        x = scaledImageWidth - overlayWidth - distance;
        y = scaledImageHeight/2 - overlayHeight/2;
        break;
    case config::QRPosBottomLeft:
        x = distance;
        y = scaledImageHeight - overlayHeight - distance;
        break;
    case config::QRPosBottomCenter:
        x = scaledImageWidth/2 - overlayWidth/2;
        y = scaledImageHeight - overlayHeight - distance;
        break;
    case config::QRPosBottomRight:
        x = scaledImageWidth - overlayWidth - distance;
        y = scaledImageHeight - overlayHeight - distance;
        break;
    case config::QRPosOutsideTop:
        x = scaledImageWidth + distance;
        y = distance;
        break;
    case config::QRPosOutsideCenter:
        x = scaledImageWidth + distance;
        y = scaledImageHeight/2 - overlayHeight/2;
        break;
    case config::QRPosOutsideBottom:
        x = scaledImageWidth + distance;
        y = scaledImageHeight - overlayHeight - distance;
        break;
    default:
        x = distance;
        y = distance;
        break;
    }

    painter->drawImage(QPoint(x ,y),*Image);

}


void PhotoPrint::printActiveTimerTimeout()
{
    // print active timer timed out.
    // restore the old view.
    set_View(printActiveViewMarker);
}

void PhotoPrint::thumbnailScrollDownTimer_Timeout()
{
    // ui was not touched until timeout. scroll down to the newest images
    ui->listWidget->scrollToBottom();
    qInfo() << "thumbnailScrollDownTimer Timeout";
}

void PhotoPrint:: returnToThumbnailViewTimer_Timeout()
{
    // ui was not touched until timeout. scroll down to the newest images
    if(currentView == viewImage || currentView == viewPrintConfirmation)
    {
        set_View(viewThumbnails);
    }
    qInfo() << "thumbnailScrollDownTimer Timeout";
}

void PhotoPrint::listWidget_ScrollerStateChanged(QScroller::State newState)
{
    qInfo() << "listWidget_ScrollerStateChanged: " << newState;

    if(currentView == viewThumbnails)
    {
        if(newState == QScroller::Inactive) // restart the scroll down timer
        {
            thumbnailScrollDownTimer->start(configWidget->get_ThumbnailScrollDownTimeout()*1000);
        }
        else
        {
            thumbnailScrollDownTimer->stop();
        }
    }
}

void PhotoPrint::on_backButton_clicked()
{
    if(currentView == viewImage)
    {
        set_View(viewThumbnails);
    }
    if(currentView == viewPrintConfirmation)
    {
        set_View(printActiveViewMarker);
    }
}

void PhotoPrint::on_nextButton_clicked()
{
    if(currentView == viewImage)
    {
        // get current item
        int row = ui->listWidget->currentRow();
        //qDebug() << row;
        row++;
        // check for overflow
        if(row==ui->listWidget->count()) row = 0;

        // display and set the new item
        selectedImageItem = (ImageItem*)ui->listWidget->item(row);
        ui->listWidget->setCurrentItem(selectedImageItem);
        imgView->setImage(selectedImageItem->getImage());
        imgView->resize(this->size());


        /*
        // well not the most elegant way...
        QMap<QString, ImageItem *>::iterator i = imageMap.find(selectedImageItem->filename);
        qDebug() << "selectedImage" << selectedImageItem->filename;
        i++;
        if(i == imageMap.end()){
            i=imageMap.begin();
        }

        selectedImageItem = i.value();
        // get the filename:
        imgView->setImage(selectedImageItem->getImage());
        imgView->resize(this->size());
        */
    }
}

void PhotoPrint::on_prevButton_clicked()
{
    if(currentView == viewImage)
    {
        // get current item
        int row = ui->listWidget->currentRow();
        //qDebug() << row;
        row--;
        // check for overflow
        if(row==-1) row += ui->listWidget->count();

        // display and set the new item
        selectedImageItem = (ImageItem*)ui->listWidget->item(row);
        ui->listWidget->setCurrentItem(selectedImageItem);
        imgView->setImage(selectedImageItem->getImage());
        imgView->resize(this->size());
    }
}


void PhotoPrint::showRemainingPaper()
{
    QFile file("remainingPaper.txt");
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        if(!in.atEnd())
        {
            QString line=in.readLine();
            ui->remainingPaperLabel->setText(line);
        }

        ui->remainingPaperLabel->show();
        file.close();
    }
}

