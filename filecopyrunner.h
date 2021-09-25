#ifndef FILECOPYRUNNER_H
#define FILECOPYRUNNER_H

#include <QObject>
#include <QRunnable>

class FileCopyRunner : public QObject, public QRunnable
{
    Q_OBJECT
public:
    FileCopyRunner(QString sourceFile, QString destinationFile, QObject *parent = nullptr);
    ~FileCopyRunner();
    //static FileCopyRunner* get_Instance(QString sourceFile, QString destinationFile, QObject *parent = nullptr);
    static bool isRunning();
    static bool instanceExits();


public slots:
    void run();

private:
    //we only want one instance of this type so make the constructor private;

    QString sourceFile;
    QString destinationFile;
    static bool _running;
    //static bool _instanceExists;
    //static FileCopyRunner* _instance;

signals:
    void finished(QString destinationFile);
};

#endif // FILECOPYRUNNER_H
