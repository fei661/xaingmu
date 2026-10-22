QT       += core gui sql network

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
    client.cpp \
    datetime.cpp \
    emoji_dialog.cpp \
    httppix.cpp \
    main.cpp \
    login.cpp \
    register.cpp \
    mainwindow.cpp \
    rollpix.cpp \
    rolltext.cpp \
    server.cpp \
    weather.cpp

HEADERS += \
    client.h \
    datetime.h \
    emoji_dialog.h \
    httppix.h \
    login.h \
    register.h \
    mainwindow.h \
    rollpix.h \
    rolltext.h \
    server.h \
    weather.h

FORMS += \
    client.ui \
    datetime.ui \
    httppix.ui \
    login.ui \
    register.ui \
    mainwindow.ui \
    server.ui \
    weather.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES +=
