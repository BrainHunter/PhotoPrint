#include "photoprint.h"
#include "ui_photoprint.h"

PhotoPrint::PhotoPrint(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PhotoPrint)
    , configWidget(new config)
    , imgView(new ImageView(this))
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


    // File System checker
    watcher = new QFileSystemWatcher(this);
    connect(watcher,SIGNAL(directoryChanged(QString)),this,SLOT(fileSystemUpdated(QString)));

    // Buttons:
    // ---- printButton ----
    ui->printButton->hide();
    if(QFile::exists("icons/print.png"))
    {
        ui->printButton->setIcon(QIcon("icons/print.png"));
        ui->printButton->setIconSize(QSize(40,40));
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
        ui->cartButton->setIconSize(QSize(40,40));
        ui->cartButton->setText("");
    }
    else
    {
        ui->cartButton->setText("TO CART");
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
            it = new ImageItem(tmp);
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
        configWidget->show();
        ui->listWidget->hide();
        imgView->hide();
        ui->printButton->hide();
        ui->cartButton->hide();
        currentView = viewConfig;
        break;
    case viewThumbnails:
        configWidget->hide();
        ui->listWidget->show();
        imgView->hide();
        ui->printButton->hide();
        ui->cartButton->hide();

        configWidget->script_setViewThumbnails(); // execute the external script

        currentView = viewThumbnails;
        break;
    case viewImage:
        imgView->show();
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

        configWidget->script_setViewImage(selectedImageItem->filename); // execute the external script

        currentView = viewImage;
        break;
    case viewCart:
        configWidget->script_setViewCart(); // execute the external script

        currentView = viewCart;
        break;
    default:
       break;
    }

    // place buttons:
    int bx = 20;    // left
    int by = 20;     // top
    if(ui->printButton->isVisible())
    {
        ui->printButton->setGeometry(bx, by, 50,50);
        by = by + 20 +50; // next button is further down
    }
    if(ui->cartButton->isVisible())
    {
        ui->cartButton->setGeometry(bx, by, 50,50);
        by = by + 20 +50; // next button is further down
    }

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
    set_View(viewThumbnails);

    imgView->cutHeight = configWidget->get_cutHeight();

    checkForNewImages(configWidget->get_Image_Path());

    // hide the pushbutton:
    ui->pushButton->hide();

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

    // resize list widget:
    ui->listWidget->setGeometry(0,0,event->size().width(),event->size().height() );
    ui->listWidget->sortItems();

    // resize the image view
    imgView->resize(event->size());

    // call the resize event of the base class
    QWidget::resizeEvent(event);
}

void PhotoPrint::on_printButton_clicked()
{
    if(selectedImageItem != NULL)
    {
        configWidget->script_prePrint(selectedImageItem->filename );
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
