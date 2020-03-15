#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>


class ImageView : public QScrollArea
{
    Q_OBJECT
public:
    ImageView(QWidget *parent);
    ~ImageView();
    void mousePressEvent(QMouseEvent *event);

    void resize(QSize size);
    void setImage(const QImage &newImage);
    void setPrintButtonVisible(bool state);
    void setCartButtonVisible(bool state);
    bool cutHeight;

private slots:
    void on_printButton_clicked();
    void on_cartButton_clicked();

private:
    QImage image;
    QLabel *imageLabel;
    QPushButton* printButton;
    QPushButton* cartButton;

signals:
    void Mouse_pressed();
    void printButton_Pressed();
    void cartButton_Pressed();

};

#endif // IMAGEVIEW_H
