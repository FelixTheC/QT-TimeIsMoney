QT       += core \
            gui \
            sql \
            printsupport \
            serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    invoice.cpp \
    main.cpp \
    mainwindow.cpp \
    serialoptions.cpp \
    serialreader_qt.cpp \
    task.cpp \
    taskdb.cpp \
    tasklist.cpp \
    updater.cpp

HEADERS += \
    invoice.h \
    mainwindow.h \
    serialoptions.h \
    serialreader_qt.h \
    task.h \
    taskdb.h \
    tasklist.h \
    updater.h

FORMS += \
    invoice.ui \
    mainwindow.ui \
    serialoptions.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
