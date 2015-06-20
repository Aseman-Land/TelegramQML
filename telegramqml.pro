TEMPLATE = lib
TARGET = TelegramQML
QT += qml quick sql xml multimedia
CONFIG += qt plugin
DEFINES += TELEGRAMQML_LIBRARY

TARGET = $$qtLibraryTarget($$TARGET)
uri = TelegramQML

win32 {
    QT += winextras
    LIBS += -LD:/Projects/cutegram-deps/lib -lssleay32 -lcrypto -lz -lqtelegram-ae
    INCLUDEPATH += D:/Projects/cutegram-deps/include D:/Projects/libqtelegram-aseman-edition
} else {
macx {
    QT += macextras
    LIBS += -lssl -lcrypto -lz -L/Users/bardia/Projects/builds/64/lib/ -lqtelegram-ae
    INCLUDEPATH += /Users/bardia/Projects/builds/64/include/libqtelegram-ae
} else {
openbsd {
    LIBS += -lssl -lcrypto -lz -lqtelegram-ae
    INCLUDEPATH += /usr/local/include/libqtelegram-ae $$OUT_PWD/$$DESTDIR/include/libqtelegram-ae
} else {
    LIBS += -lssl -lcrypto -lz -lqtelegram-ae
    INCLUDEPATH += /usr/include/libqtelegram-ae $$OUT_PWD/$$DESTDIR/include/libqtelegram-ae
}
}
}

# Input
SOURCES += \
    telegramqml_plugin.cpp \
    backgroundmanager.cpp \
    chatparticipantlist.cpp \
    database.cpp \
    databasecore.cpp \
    dialogfilesmodel.cpp \
    mp3converterengine.cpp \
    photosizelist.cpp \
    profilesmodel.cpp \
    telegramsearchmodel.cpp \
    telegramuploadsmodel.cpp \
    telegramwallpapersmodel.cpp \
    usernamefiltermodel.cpp \
    telegramqml.cpp \
    tagfiltermodel.cpp \
    telegramchatparticipantsmodel.cpp \
    telegramcontactsmodel.cpp \
    telegramdialogsmodel.cpp \
    telegramfilehandler.cpp \
    telegrammessagesmodel.cpp \
    newsletterdialog.cpp \
    userdata.cpp \

HEADERS += \
    telegramqml_plugin.h \
    backgroundmanager.h \
    chatparticipantlist.h \
    database.h \
    databasecore.h \
    dialogfilesmodel.h \
    mp3converterengine.h \
    photosizelist.h \
    profilesmodel.h \
    telegramsearchmodel.h \
    telegramuploadsmodel.h \
    telegramwallpapersmodel.h \
    userdata.h \
    usernamefiltermodel.h \
    telegramqml.h \
    tagfiltermodel.h \
    telegramchatparticipantsmodel.h \
    telegramcontactsmodel.h \
    telegramdialogsmodel.h \
    telegramfilehandler.h \
    telegrammessagesmodel.h \
    objects/types.h \
    telegramqml_macros.h \
    telegramqml_global.h \
    newsletterdialog.h

DISTFILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

RESOURCES += \
    resource.qrc

