#include "imageview.h"
#include <QDebug>
#include <QIcon>
#include <QFile>

ImageView::ImageView(QWidget *parent):
    QScrollArea(parent),
    imageLabel(new QLabel),
    printButton(new QPushButton(this)),
    cartButton(new QPushButton(this))
{

    // Image Label settings
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    // printButton
    printButton->hide();
    printButton->setGeometry(20,20,50,50);
    if(QFile::exists("print.png"))
    {
        printButton->setIcon(QIcon("print.png"));
        printButton->setIconSize(QSize(40,40));
    }
    else
    {
        printButton->setText("Print");
    }
    connect(printButton,SIGNAL(clicked()), this, SLOT(on_printButton_clicked()));

    // cartButton
    cartButton->hide();
    cartButton->setGeometry(20,90,50,50);
    if(QFile::exists("cart.png"))
    {
        cartButton->setIcon(QIcon("cart.png"));
        cartButton->setIconSize(QSize(40,40));
    }
    else
    {
        cartButton->setText("CART");
    }
    connect(cartButton,SIGNAL(clicked()), this, SLOT(on_cartButton_clicked()));


    this->setBackgroundRole(QPalette::Dark);
    this->setWidget(imageLabel);
    this->setWidgetResizable(true);
    this->setFrameShape(QFrame::NoFrame); // this removes the white border!


    cutHeight=false;

}

ImageView::~ImageView()
{
    delete imageLabel;
}

void ImageView::resize(QSize size)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    if(!imageLabel->pixmap(Qt::ReturnByValueConstant()).isNull()) // if no image is loaded don't do this
#else
    if(imageLabel->pixmap() != NULL) // if no image is loaded don't do this
#endif
    {
        QSize imgSize = image.size();
        if(imgSize.width() == 0){
            qDebug() << "rezize with image size = 0";
            return;
        }
        QSize tempSize = size;
        if(cutHeight)
        {   // modify the height to "oversize" the image --> the upper and lower borders will be cut when the image is centered in the window.
            tempSize.setHeight((size.width()*imgSize.height())/imgSize.width());
        }
        QScrollArea::resize(imgSize.scaled(tempSize,Qt::KeepAspectRatio));

        int x = size.width() - this->size().width();
        int y = size.height() - this->size().height();
        this->move(x/2, y/2);  // center the scrolArea. (--> the image)

        // buttons to on the top left corner:
        int button_x = 20;  // left + 20
        int button_y = 0;    // top -
        // well we don't know the window size here :-/

    }
    else
    {
        QScrollArea::resize(size);
    }


}

void ImageView::setImage(const QImage &newImage)
{
    image = newImage;
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
    this->resize(this->size());
}

void ImageView::setPrintButtonVisible(bool state)
{
    printButton->setVisible(state);
}

void ImageView::setCartButtonVisible(bool state)
{
    cartButton->setVisible(state);
}

void ImageView::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "ImageView::mousePressEvent";
    emit Mouse_pressed();
}

void ImageView::on_printButton_clicked()
{
    emit printButton_Pressed();

}
void ImageView::on_cartButton_clicked()
{
    emit cartButton_Pressed();

}

void ImageView::setGreyedOut(bool val)
{
    imageLabel->setDisabled(val);

}
