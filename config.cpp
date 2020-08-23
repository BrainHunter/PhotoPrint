#include "config.h"
#include "ui_config.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QPrinterInfo>


config::config(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::config)
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
    SetReadOnly(ui->landscapeRadioButton, true);


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
     int printerOrientation = settings.value("Printer/PrinterOrrientation").toInt();
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

     //thumbnail view settings:
     uint ThumbnailSize = settings.value("ThumbnailView/ThumbnailSize", "200").toUInt();
     if (ui->thumbnailSizeSpinBox)
     {
        ui->thumbnailSizeSpinBox->setValue(ThumbnailSize);
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

     settings.endGroup();

     //thumbnail view settings:
     settings.beginGroup("ThumbnailView");
     settings.setValue("ThumbnailSize", ui->thumbnailSizeSpinBox->value());
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

bool config::get_externalPrintEnable()
{
    return ui->externalPrintCheckBox->isChecked();
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
