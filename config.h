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

    uint get_printActiveTimeout();

    bool get_externalPrintEnable();

    bool script_prePrint(QString filename, bool blocking=false);
    bool script_externalPrint(QString filename, bool blocking=true);
    bool script_setViewImage(QString filename, bool blocking=false);
    bool script_setViewThumbnails(bool blocking=false);
    bool script_setViewCart(bool blocking=false);

private slots:
    void on_saveButton_clicked();

    void on_loadButton_clicked();

    void on_browseButton_clicked();

    void on_pushButton_clicked();

    void on_printerSelectButton_clicked();

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

    // External Script
    bool executeExternal(QString filename, QStringList arguments, bool blocking );

signals:
    void StartButton_clicked();


};

#endif // CONFIG_H
