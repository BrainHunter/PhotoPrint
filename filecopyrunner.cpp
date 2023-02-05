#include "filecopyrunner.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>

#include <QtDebug>

bool FileCopyRunner::_running = false;
// bool FileCopyRunner::_instanceExists = false;
// FileCopyRunner* FileCopyRunner::_instance;

FileCopyRunner::FileCopyRunner(QString sourceFile, QString destinationFile, QObject *parent): QObject(parent)
{
    this->sourceFile = sourceFile;
    this->destinationFile = destinationFile;
    _running = false;
}

FileCopyRunner::~FileCopyRunner()
{
    //FileCopyRunner::_instance = nullptr;

}

//FileCopyRunner* FileCopyRunner::get_Instance(QString sourceFile, QString destinationFile, QObject *parent)
//{
//    if(FileCopyRunner::_instance == nullptr)
//    {
//        FileCopyRunner::_instance = new FileCopyRunner(sourceFile, destinationFile, parent );
//    }
//    return FileCopyRunner::_instance;
//}


void FileCopyRunner::run()
{
    _running = true;
    QFileInfo DstFileInfo(destinationFile);

    QDir dir(DstFileInfo.absolutePath());
    if (!dir.exists())
        dir.mkpath(".");

    if (!QFile::exists(destinationFile)) // check if destination file already exists
    {
        //QFile::remove("/path/copy-of-file");
        if(QFile::copy(sourceFile, destinationFile))
        {
            //  check if source and Destination is the same size:
            QFileInfo srcFileInfo(sourceFile);
            DstFileInfo.refresh();
            if( DstFileInfo.size() == srcFileInfo.size())
            {
                emit finished(destinationFile);
            }
            else
            {
                qWarning() << "Destination File is not the same size as Sourcefile; deleting the destination!";
                if(!QFile::remove(destinationFile))
                {
                    qWarning() << "Destination File could not be deleted!";
                }
            }
        }
        else
        {
            qWarning() << "File Copy Failed!";
        }
    }

    _running = false;
}

bool FileCopyRunner::isRunning()
{
    return _running;
}
