#include "photoprint.h"
#include "ui_photoprint.h"

PhotoPrint::PhotoPrint(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PhotoPrint)
    , configWidget(new config)
    , imgView(new ImageView(this))
    , printActiveView(new ImageView(this))
    , printActiveTimer(new QTimer(this))
{
    ui->setupUi(this);

    // no image is selected at the beginig:
    selectedImageItem = NULL;

    // thumbnail view list
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


    // scrolling thingy
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


    //config widget
    configWidget->setParent(this);
    connect(configWidget, SIGNAL(StartButton_clicked()), this, SLOT(start()));
    qDebug() << configWidget->palette().color(QWidget::backgroundRole()).name();


    // Image View;
    imgView->hide();
    connect(imgView, SIGNAL(Mouse_pressed()), this, SLOT(on_imageView_Pressed()));

    // print Active view
    printActiveView->hide();
    printActiveView->setImage(QImage("icons/printActive.png"));
    // print Active timer:
    printActiveTimer->setSingleShot(true);
    connect(printActiveTimer,SIGNAL(timeout()),this,SLOT(printActiveTimerTimeout()));


    // File System checker
    watcher = new QFileSystemWatcher(this);
    connect(watcher,SIGNAL(directoryChanged(QString)),this,SLOT(fileSystemUpdated(QString)));

    // Buttons:
    // ---- printButton ----
    ui->printButton->hide();
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
    if(QFile::exists("icons/back.png"))
    {
        ui->backButton->setIcon(QIcon("icons/back.png"));
        ui->backButton->setIconSize(QSize(100,100));
        ui->backButton->setText("");
    }
    else
    {
        ui->backButton->setText("Back");
    }

    // set default view:
    set_View(viewConfig);

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
        QString tmp=dirList.last().absoluteFilePath();
        dirList.removeLast();

        if(!imageMap.contains(tmp))
        {
            ImageItem *it;
            it = new ImageItem(tmp, configWidget->get_ThumbnailSize());
            ret = true;

            // insert into image map:
            imageMap.insert(tmp,it);
            qInfo() << "adding to map:" + tmp;

            // insert into widget
            ui->listWidget->addItem(it);
        }
    }
    return ret;
}


void PhotoPrint::set_View(ViewEnum e){
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

        configWidget->script_setViewThumbnails(); // execute the external script

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

        configWidget->script_setViewImage(selectedImageItem->filename); // execute the external script

        currentView = viewImage;
        break;
    case viewCart:
        configWidget->script_setViewCart(); // execute the external script
        printActiveView->hide();
        currentView = viewCart;
        break;
    case viewPrintActive:
        printActiveViewMarker = currentView;
        printActiveView->show();
        printActiveView->resize(this->size());
        printActiveView->raise();

        ui->printButton->hide();
        ui->cartButton->hide();

        // start the printActiveTimer which restores the old view once timed out
        printActiveTimer->start(5000);
        currentView = viewPrintActive;
        break;
    default:
       break;
    }

    // place buttons:
    int bx = 20;    // left
    int by = 20;     // top
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

    bx = this->size().width() - 20 - 120;
    by = 20;
    ui->backButton->setGeometry(bx,by,120,120);


}

// test button:
void PhotoPrint::on_pushButton_clicked()
{
    //static int bla=0;
    //ui->listWidget->addItem(new QListWidgetItem(QIcon("image.jpg"),"button_"+QString::number(bla++)));

    // checkForNewImages(configWidget->get_Image_Path());

    ui->listWidget->sortItems();
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

void PhotoPrint::on_imageView_Pressed()
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
    imgView->setImage(selectedImageItem->getImage());
    imgView->resize(this->size());

    //view image:
    set_View(viewImage);
}

void PhotoPrint::fileSystemUpdated(QString string){
    qInfo() << "Directory has changed! " + string;
    checkForNewImages(configWidget->get_Image_Path());
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
            QImage img = selectedImageItem->getImage();
            QPrinter* printer = configWidget->get_Printer();
            qDebug() << "printer state" << printer->printerState();
            QPainter painter(printer);
            double xscale = printer->pageRect().width() / double(img.width());
            double yscale = printer->pageRect().height() / double(img.height());
            double scale = qMin(xscale, yscale);
            qDebug() << "pageRect().width " << printer->pageRect().width();
            qDebug() << "img.width " << img.width();
            qDebug() << "scale " << scale;
            //painter.translate(printer->paperRect().x() + printer->pageRect().width() / 2,
            //                    printer->paperRect().y() + printer->pageRect().height() / 2);
            painter.scale(scale, scale);
            //painter.translate(-width() / 2, -height() / 2);
            painter.drawImage(QPoint(0,0),img);
            painter.end();

            qDebug() << "printer state" << printer->printerState();
            qDebug() << "printer state" << printer->printerState();
        }

    }
}

void PhotoPrint::printActiveTimerTimeout()
{
    // print active timer timed out.
    // restore the old view.
    set_View(printActiveViewMarker);
}

void PhotoPrint::on_backButton_clicked()
{
    if(currentView == viewImage)
    {
        set_View(viewThumbnails);
    }
}
