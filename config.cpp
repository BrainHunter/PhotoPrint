#include "config.h"
#include "ui_config.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QPrinterInfo>

#include "QR-Code-generator\cpp\qrcodegen.hpp"


config::config(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::config),
    QRCodeImage(NULL)
{
    ui->setupUi(this);

    printer = new QPrinter(QPrinter::HighResolution);
    // test for viewing the widget size in the app
    //QPalette pal;
    //pal.setColor(QPalette::Background, Qt::black);
    //this->setPalette(pal);
    //this->setAutoFillBackground(true);

    settingsFile = qApp->applicationDirPath() + "/config.ini";
    loadSettings();
    qInfo() << settingsFile;

    SetReadOnly(ui->landscapeRadioButton, true);
    SetReadOnly(ui->PortraitRadioButton, true);


}

config::~config()
{
    delete ui;
    delete printer;
}

void config::loadSettings()
{
     QSettings settings(settingsFile, QSettings::IniFormat);

     qDebug() << settings.allKeys();


     // Path:
     QString sPath = settings.value("path", "C:\\Photos").toString();
     if (ui->directoryEdit)
     {
        ui->directoryEdit->setText(sPath);
     }

     //Show Fullscreen
     bool showFS = settings.value("showFullscreen", "true").toBool();
     if (ui->fullScreenCheckBox)
     {
        ui->fullScreenCheckBox->setChecked(showFS);
     }

     //Cut Height
     bool cutHeight = settings.value("cutHeight", "true").toBool();
     if (ui->cutHeightCheckBox)
     {
        ui->cutHeightCheckBox->setChecked(cutHeight);
     }

     //Autostart
     bool autostart = settings.value("autostart", "false").toBool();
     if (ui->AutostartCheckBox)
     {
        ui->AutostartCheckBox->setChecked(autostart);
     }

     //single Image Print
     bool singlePrint = settings.value("singlePrint", "false").toBool();
     if (ui->singlePrintCheckBox)
     {
        ui->singlePrintCheckBox->setChecked(singlePrint);
     }

     //shopping cart
     bool shoppingCart = settings.value("shoppingCart", "false").toBool();
     if (ui->shoppingCartCheckBox)
     {
        ui->shoppingCartCheckBox->setChecked(shoppingCart);
     }

     // Printer things:
     QString printerName = settings.value("Printer/PrinterName",QPrinterInfo::defaultPrinterName()).toString();
     if(printerName == "")
     {
         printerName = QPrinterInfo::defaultPrinterName();
     }

     printer->setPrinterName(printerName);
     int printerOrientation = settings.value("Printer/PrinterOrientation").toInt();
     switch(printerOrientation)
     {
     case 0:
         printer->setOrientation(QPrinter::Portrait);
         break;
     case 1:
         printer->setOrientation(QPrinter::Landscape);
         break;
     default:
         printer->setOrientation(QPrinter::Portrait);
     }

     bool PrintPreviewEnabled = settings.value("Printer/PrintPreviewEnabled", "false").toBool();
     if (ui->PrintPreviewCheckBox)
     {
        ui->PrintPreviewCheckBox->setChecked(PrintPreviewEnabled);
     }

     uint printActiveTimeout = settings.value("PrintActiveTimeout", "5").toUInt();
     if (ui->printActiveTimeoutSpinBox)
     {
        ui->printActiveTimeoutSpinBox->setValue(printActiveTimeout);
     }

     //thumbnail view settings:
     uint ThumbnailSize = settings.value("ThumbnailView/ThumbnailSize", "200").toUInt();
     if (ui->thumbnailSizeSpinBox)
     {
        ui->thumbnailSizeSpinBox->setValue(ThumbnailSize);
     }
     uint ThumbnailScrollDownTimeout = settings.value("ThumbnailView/ThumbnailScrollDownTimeout", "60").toUInt();
     if (ui->thumbnailScrollDownTimeoutSpinBox)
     {
        ui->thumbnailScrollDownTimeoutSpinBox->setValue(ThumbnailScrollDownTimeout);
     }

     // File Copy
     bool LocalCopyEnabled = settings.value("FileCopy/LocalCopyEnabled", "false").toBool();
     if (ui->localCopyEnableCheckBox)
     {
        ui->localCopyEnableCheckBox->setChecked(LocalCopyEnabled);
        on_localCopyEnableCheckBox_clicked(LocalCopyEnabled);
     }
     QString LocalCopyDirectory = settings.value("FileCopy/LocalCopyDirectory", "C:\\Photos-Temp").toString();
     if (ui->directoryLocalCopyEdit)
     {
        ui->directoryLocalCopyEdit->setText(LocalCopyDirectory);
     }

     // QR Code
     bool QRCodeEnabled = settings.value("QRCode/QRCodeEnabled", "false").toBool();
     if (ui->QRCodeEnableCheckBox)
     {
        ui->QRCodeEnableCheckBox->setChecked(QRCodeEnabled);
     }
     QString QRCodeLink = settings.value("QRCode/QRCodeLink", "https://github.com/BrainHunter/PhotoPrint").toString();
     if (ui->QRCodeLinkEdit)
     {
        ui->QRCodeLinkEdit->setText(QRCodeLink);
     }
     uint QRCodeSize = settings.value("QRCode/QRCodeSize", "160").toUInt();
     if (ui->QRCodeSizeSpinBox)
     {
        ui->QRCodeSizeSpinBox->setValue(QRCodeSize);
     }
     uint QRCodeDistance = settings.value("QRCode/QRCodeDistance", "30").toUInt();
     if (ui->QRCodeDistanceSpinBox)
     {
        ui->QRCodeDistanceSpinBox->setValue(QRCodeDistance);
     }

     uint QRCodePosition = settings.value("QRCode/QRCodePosition", "0").toUInt();
     if (ui->QRPosTopLeftRadioButton)
     {
        set_QRCodePosition((config::QRCodePosition)QRCodePosition);
     }

     // update ui
     setPrinterUi();
}

void config::saveSettings()
{
     QSettings settings(settingsFile, QSettings::IniFormat);

     // Path:
     settings.setValue("path", ui->directoryEdit->text());
     //Show Fullscreen
     settings.setValue("showFullscreen", ui->fullScreenCheckBox->isChecked());
     //Cut Height
     settings.setValue("cutHeight", ui->cutHeightCheckBox->isChecked());
     //Autostart
     settings.setValue("autostart", ui->AutostartCheckBox->isChecked());
     //single Image Print
     settings.setValue("singlePrint", ui->singlePrintCheckBox->isChecked());
     //shopping cart
     settings.setValue("shoppingCart", ui->shoppingCartCheckBox->isChecked());


     //printer settings:
     settings.beginGroup("Printer");
     settings.setValue("PrinterName", printer->printerName());
     settings.setValue("PrinterOrientation",printer->orientation());
     settings.setValue("PrintPreviewEnabled",ui->PrintPreviewCheckBox->isChecked());
     settings.endGroup();     
     settings.setValue("PrintActiveTimeout",ui->printActiveTimeoutSpinBox->value());


     //thumbnail view settings:
     settings.beginGroup("ThumbnailView");
     settings.setValue("ThumbnailSize", ui->thumbnailSizeSpinBox->value());
     settings.setValue("ThumbnailScrollDownTimeout", (uint)ui->thumbnailScrollDownTimeoutSpinBox->value());
     settings.endGroup();

     // File Copy
     settings.beginGroup("FileCopy");
     settings.setValue("LocalCopyEnabled", ui->localCopyEnableCheckBox->isChecked());
     settings.setValue("LocalCopyDirectory", ui->directoryLocalCopyEdit->text());
     settings.endGroup();

     // QR Code
     settings.beginGroup("QRCode");
     settings.setValue("QRCodeEnabled", ui->QRCodeEnableCheckBox->isChecked());
     settings.setValue("QRCodeLink", ui->QRCodeLinkEdit->text());
     settings.setValue("QRCodeSize", ui->QRCodeSizeSpinBox->value());
     settings.setValue("QRCodeDistance", ui->QRCodeDistanceSpinBox->value());
     settings.setValue("QRCodePosition", get_QRCodePosition());
     settings.endGroup();

}

void config::on_saveButton_clicked()
{
    saveSettings();
}

void config::on_loadButton_clicked()
{
    loadSettings();
}

//Browse Button opens a file dialog and sets the new Path in the Edit Box
void config::on_browseButton_clicked()
{
    QUrl newPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui->directoryEdit->text());
    ui->directoryEdit->setText(newPath.toString());
}

// start button
void config::on_pushButton_clicked()
{
    emit StartButton_clicked();
}

QString config::get_Image_Path()
{
    return ui->directoryEdit->text();
}

bool config::get_cutHeight()
{
    return ui->cutHeightCheckBox->isChecked();
}

void config::on_printerSelectButton_clicked()
{
    QList<QPrinterInfo> plist = QPrinterInfo::availablePrinters();
    qDebug() << "number of printers: " << plist.length();

    foreach( QPrinterInfo pinfo , plist){
        qDebug() << "printer name: " << pinfo.printerName();
    }
    qDebug() << "printer->printerName: " << printer->printerName();
    QPrintDialog dialog(printer, this);
    dialog.setWindowTitle(tr("Print Document"));
    //if (editor->textCursor().hasSelection())
    dialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);
    int dialogreturn = dialog.exec();
    if (dialogreturn != QDialog::Accepted) {
        qDebug() << "dialog accepted";
    }

    // update ui
    setPrinterUi();
}

QString rectToString(QRect rect){
    QString s="(";
    s += QString::number(rect.x());
    s+=",";
    s += QString::number(rect.y());
    s+=" ";
    s += QString::number(rect.width());
    s+="x";
    s += QString::number(rect.height());
    s+=")";

    return s;
}

void config::setPrinterUi()
{
    ui->PrinterNameEdit->setText(printer->printerName());

    switch(printer->orientation())
    {
    case QPrinter::Portrait:
        ui->PortraitRadioButton->setChecked(true);
        ui->landscapeRadioButton->setChecked(false);
        break;
    case QPrinter::Landscape:
        ui->PortraitRadioButton->setChecked(false);
        ui->landscapeRadioButton->setChecked(true);
        break;
    default:
        ui->PortraitRadioButton->setChecked(false);
        ui->landscapeRadioButton->setChecked(false);
        qWarning() << "printer orrientation not set correctly: " << printer->orientation();
    }

    qDebug() << "pageRect( )" <<printer->pageRect();
    qDebug() << "paperRect( )" <<printer->paperRect();
    qDebug() << "pageSize() " << printer->pageSize();
    qDebug() << "paperName() " << printer->paperName();
    qDebug() << "pageLayout() " << printer->pageLayout();
    qDebug() << "pageSizeMM() " << printer->pageSizeMM();
    qDebug() << "paperSource() " << printer->paperSource();
    qDebug() << "outputFormat() " << printer->outputFormat();
    qDebug() << "resolution() " << printer->resolution();

    ui->printerTextEdit->setText("");
    ui->printerTextEdit->insertPlainText("paperRect: " + rectToString(printer->paperRect()) + "\n");
    ui->printerTextEdit->insertPlainText("pageRect: " + rectToString(printer->pageRect()) + "\n");
    ui->printerTextEdit->insertPlainText("resolution: " + QString::number(printer->resolution()) );
}

void config::SetReadOnly(QRadioButton* RadioButton, bool readOnly)
{
   RadioButton->setAttribute(Qt::WA_TransparentForMouseEvents, readOnly);
   RadioButton->setFocusPolicy(readOnly ? Qt::NoFocus : Qt::StrongFocus);
}

bool config::get_singlePrintEnable()
{
    return ui->singlePrintCheckBox->isChecked();
}

bool config::get_shoppingCartEnable()
{
    return ui->shoppingCartCheckBox->isChecked();
}

QPrinter*  config::get_Printer()
{
    return printer;
}

bool config::get_autostart()
{
    return ui->AutostartCheckBox->isChecked();
}

bool config::get_showFullscreen()
{
    return ui->fullScreenCheckBox->isChecked();
}

bool config::get_externalPrintEnable()
{
    return ui->externalPrintCheckBox->isChecked();
}

bool config::get_printPreviewEnabled()
{
    return ui->PrintPreviewCheckBox->isChecked();
}

//// ------------------- Script thingies ---------------
///
///
///
///
///

bool config::script_prePrint(QString filename, bool blocking)
{
    QString command("prePrint.bat");

    QStringList sl;
    sl.append(filename);

    return executeExternal(command, sl, blocking);
}

bool config::script_externalPrint( QString filename, bool blocking)
{
    QString command("externalPrint.bat");

    QStringList sl;
    sl.append(filename);

    return executeExternal(command, sl, blocking);
}

bool config::script_setViewImage(QString filename, bool blocking)
{
    QString command("setViewImage.bat");

    QStringList sl;
    sl.append(filename);

    return executeExternal(command, sl, blocking);
}

bool config::script_setViewThumbnails(bool blocking)
{
    QString command("setViewThumbnails.bat");

    QStringList sl;

    return executeExternal(command, sl, blocking);
}

bool config::script_setViewCart(bool blocking)
{
    QString command("setViewCart.bat");

    QStringList sl;

    return executeExternal(command, sl, blocking);
}

bool config::executeExternal(QString filename, QStringList arguments, bool blocking )
{
    qDebug() << "trying to execute: " << filename;
    qDebug() << "with arguments: " << arguments;
    if(blocking)
    {
        qDebug() << "blocking";
        int ret = QProcess::execute(filename, arguments);

        switch(ret)
        {
        case (-2):
            qDebug() << "process could not be startet";
            return false;
            break;
        case (-1):
            qDebug() << "process crashed";
            return false;
            break;
        default:
            return true;
        }
    }
    else
    {
        qDebug() << "no blocking";
        bool ret = QProcess::startDetached(filename, arguments);
        qDebug() << "ret = " << ret;
        return ret;
    }

    // should not get here?
    return false;
}

uint config::get_printActiveTimeout()
{
    return ui->printActiveTimeoutSpinBox->value();

}



//// ------------------- Thumbnail View thingies ---------------
///
///
///
///
///
QSize config::get_ThumbnailSize()
{

    return QSize(ui->thumbnailSizeSpinBox->value(), ui->thumbnailSizeSpinBox->value());
}

uint config::get_ThumbnailScrollDownTimeout()
{
    return (uint)ui->thumbnailScrollDownTimeoutSpinBox->value();
}


//// ------------------- File Copy thingies ---------------
///
///
///
///
///

void config::on_localCopyEnableCheckBox_clicked(bool checked)
{
    ui->localCopyLabel->setEnabled(checked);
    ui->directoryLocalCopyEdit->setEnabled(checked);
    ui->browseLocalCopyButton->setEnabled(checked);
}


bool config::get_localCopyEnabled()
{
    return ui->localCopyEnableCheckBox->isChecked();
}

QString config::get_LocalCopyPath()
{
    return ui->directoryLocalCopyEdit->text();
}

void config::on_browseLocalCopyButton_clicked()
{
    QUrl newPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui->directoryLocalCopyEdit->text());
    ui->directoryLocalCopyEdit->setText(newPath.toString());
}


//// ------------------- QRCode thingies ---------------
///
///
///
///
///


void config::on_QRCodeGenerateButton_clicked()
{
    updateQrCode(ui->QRCodeLinkEdit->text());
}

void config::updateQrCode( QString text )
{
  using namespace qrcodegen;
  // Create the QR Code object
  QrCode qr = QrCode::encodeText( text.toUtf8().data(), QrCode::Ecc::MEDIUM );
  qint32 sz = qr.getSize();
  qDebug() << "QRCode size" << sz;
  if(QRCodeImage != NULL)
  {
      delete QRCodeImage;
  }
  QRCodeImage = new QImage(sz,sz, QImage::Format_RGB32);
    QRgb black = qRgb(  0,  0,  0);
    QRgb white = qRgb(255,255,255);
  for (int y = 0; y < sz; y++)
    for (int x = 0; x < sz; x++)
      QRCodeImage->setPixel(x,y,qr.getModule(x, y) ? black : white );
  ui->QRCodeLabel->setPixmap( QPixmap::fromImage(QRCodeImage->scaled(160,160,Qt::KeepAspectRatio,Qt::FastTransformation),Qt::MonoOnly) );
}

void config::on_QRCodeLinkEdit_textChanged(const QString &arg1)
{
    updateQrCode(arg1);
}

QImage *config::get_QRCodeImage()
{
    static QImage img;
    int size = ui->QRCodeSizeSpinBox->value();
    if(QRCodeImage == NULL)
    {
        updateQrCode(ui->QRCodeLinkEdit->text());
    }
    img = QRCodeImage->scaled(size,size,Qt::KeepAspectRatio,Qt::FastTransformation);
    return &img;
}

config::QRCodePosition config::get_QRCodePosition()
{
    if(ui->QRPosTopLeftRadioButton->isChecked())        return QRPosTopLeft;
    if(ui->QRPosTopCenterRadioButton->isChecked())      return QRPosTopCenter;
    if(ui->QRPosTopRightRadioButton->isChecked())       return QRPosTopRight;
    if(ui->QRPosCenterLeftRadioButton->isChecked())     return QRPosCenterLeft;
    if(ui->QRPosCenterCenterRadioButton->isChecked())   return QRPosCenterCenter;
    if(ui->QRPosCenterRightRadioButton->isChecked())    return QRPosCenterRight;
    if(ui->QRPosBottomLeftRadioButton->isChecked())     return QRPosBottomLeft;
    if(ui->QRPosBottomCenterRadioButton->isChecked())   return QRPosBottomCenter;
    if(ui->QRPosBottomRightRadioButton->isChecked())    return QRPosBottomRight;
    if(ui->QRPosOutsideTopRadioButton->isChecked())     return QRPosOutsideTop;
    if(ui->QRPosOutsideCenterRadioButton->isChecked())  return QRPosOutsideCenter;
    if(ui->QRPosOutsideBottomRadioButton->isChecked())  return QRPosOutsideBottom;
    return QRPosTopLeft;
}

void config::set_QRCodePosition(config::QRCodePosition pos)
{
    switch(pos)
    {
    case QRPosTopLeft: ui->QRPosTopLeftRadioButton->setChecked(true);
        break;
    case QRPosTopCenter: ui->QRPosTopCenterRadioButton->setChecked(true);
        break;
    case QRPosTopRight: ui->QRPosTopRightRadioButton->setChecked(true);
        break;
    case QRPosCenterLeft: ui->QRPosCenterLeftRadioButton->setChecked(true);
        break;
    case QRPosCenterCenter: ui->QRPosCenterCenterRadioButton->setChecked(true);
        break;
    case QRPosCenterRight: ui->QRPosCenterRightRadioButton->setChecked(true);
        break;
    case QRPosBottomLeft: ui->QRPosBottomLeftRadioButton->setChecked(true);
        break;
    case QRPosBottomCenter: ui->QRPosBottomCenterRadioButton->setChecked(true);
        break;
    case QRPosBottomRight: ui->QRPosBottomRightRadioButton->setChecked(true);
        break;
    case QRPosOutsideTop: ui->QRPosOutsideTopRadioButton->setChecked(true);
        break;
    case QRPosOutsideCenter: ui->QRPosOutsideCenterRadioButton->setChecked(true);
        break;
    case QRPosOutsideBottom: ui->QRPosOutsideBottomRadioButton->setChecked(true);
        break;
    default: ui->QRPosTopLeftRadioButton->setChecked(true);
    }
}

bool config::get_QRCodeEnabled()
{
    return ui->QRCodeEnableCheckBox->isChecked();
}

uint config::get_QRCodeDistance()
{
    return ui->QRCodeDistanceSpinBox->value();
}
