#ifndef IMAGESCALERUNNER_H
#define IMAGESCALERUNNER_H

#include <QObject>
#include <QRunnable>
#include <QSize>
#include <QImage>

class ImageScaleRunner : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit ImageScaleRunner(QString filename, QSize size, QObject *parent = nullptr);


public slots:
    void run();

private:
    QString filename;
    QSize size;

signals:
    void finished(QString filename, QImage img);
};

#endif // IMAGESCALERUNNER_H
