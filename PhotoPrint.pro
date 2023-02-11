QT       += core gui
QT += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    config.cpp \
    filecopyrunner.cpp \
    imageitem.cpp \
    imagescalerunner.cpp \
    imageview.cpp \
    main.cpp \
    photoprint.cpp \
    QR-Code-generator/cpp/qrcodegen.cpp

HEADERS += \
    config.h \
    filecopyrunner.h \
    imageitem.h \
    imagescalerunner.h \
    imageview.h \
    photoprint.h \
    QR-Code-generator/cpp/qrcodegen.hpp

FORMS += \
    config.ui \
    photoprint.ui

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target
#

# copy the icons to the output dir
#icons.path = %{buildDir}/icons/
#icons.files += %{sourceDir}/icons/print.png

#INSTALLS += icons


#copydata.commands = $(COPY_DIR) $$system_quote($$PWD/icons) $$system_quote("$$OUT_PWD")
#first.depends = $(first) copydata
#export(first.depends)
#export(copydata.commands)
#QMAKE_EXTRA_TARGETS += first copydata
