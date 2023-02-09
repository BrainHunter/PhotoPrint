#ifndef CONFIG_H
#define CONFIG_H

#include <QWidget>

#include <QtWidgets>
#include <QPrinter>

namespace Ui {
class config;
}




class config : public QWidget
{
    Q_OBJECT

public:
    explicit config(QWidget *parent = nullptr);
    ~config();

    QString get_Image_Path();
    bool get_cutHeight();
    QPrinter* get_Printer();
    bool get_singlePrintEnable();
    bool get_shoppingCartEnable();
    bool get_autostart();
    bool get_showFullscreen();
    QSize get_ThumbnailSize();
    uint get_ThumbnailScrollDownTimeout();
    uint get_ReturnToThumbnailViewTimeout();

    uint get_printActiveTimeout();

    bool get_externalPrintEnable();
    bool get_printPreviewEnabled();

    bool get_QRCodeEnabled();
    QImage* get_QRCodeImage();
    enum QRCodePosition{
        QRPosTopLeft,
        QRPosTopCenter,
        QRPosTopRight,
        QRPosCenterLeft,
        QRPosCenterCenter,
        QRPosCenterRight,
        QRPosBottomLeft,
        QRPosBottomCenter,
        QRPosBottomRight,
        QRPosOutsideTop,
        QRPosOutsideCenter,
        QRPosOutsideBottom
    };
    enum QRCodePosition get_QRCodePosition();
    uint get_QRCodeDistance();

    //Image Overlay things:
    bool get_OverlayEnabled();
    QImage* get_OverlayImage();
    enum QRCodePosition get_OverlayPosition();
    uint get_OverlayDistance();
    bool get_OverlayShowOnPreview();

    bool script_prePrint(QString filename, bool blocking=false);
    bool script_externalPrint(QString filename, bool blocking=true);
    bool script_setViewImage(QString filename, bool blocking=false);
    bool script_setViewThumbnails(bool blocking=false);
    bool script_setViewCart(bool blocking=false);

    bool get_localCopyEnabled();
    QString get_LocalCopyPath();

private slots:
    void on_saveButton_clicked();

    void on_loadButton_clicked();

    void on_browseButton_clicked();

    void on_pushButton_clicked();

    void on_printerSelectButton_clicked();


    void on_localCopyEnableCheckBox_clicked(bool checked);

    void on_browseLocalCopyButton_clicked();

    void on_QRCodeGenerateButton_clicked();

    void on_QRCodeLinkEdit_textChanged(const QString &arg1);

    void on_OverlayBrowseButton_clicked();

    void on_OverlayEnableCheckBox_clicked(bool checked);

private:
    Ui::config *ui;
    void SetReadOnly(QRadioButton* RadioButton, bool readOnly);

    // settings:
    QString settingsFile;
    void loadSettings();
    void saveSettings();


    // Printer things:
    QPrinter *printer;
    void setPrinterUi();

    // QRCode things:
    void updateQrCode( QString text );
    QImage* QRCodeImage;
    void set_QRCodePosition(enum QRCodePosition);

    // Image Overlay things:
    QImage OverlayImage;
    bool loadOverlayImage(bool noerror = false);
    void set_OverlayPosition(enum QRCodePosition);

    // External Script
    bool executeExternal(QString filename, QStringList arguments, bool blocking );

signals:
    void StartButton_clicked();


};





#endif // CONFIG_H
