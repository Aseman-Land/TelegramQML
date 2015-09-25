TEMPLATE = lib
TARGET = telegramqml
CONFIG += qt no_keywords
DEFINES += TELEGRAMQML_LIBRARY

uri = TelegramQml

win32 {
    QT += winextras
} else {
    macx {
        CONFIG += staticlib
        QT += macextras
    } else {
    openbsd {
    } android {
        CONFIG += staticlib
    }
    }
}

include(telegramqml.pri)

linux {
    contains(QMAKE_HOST.arch, x86_64) {
        LIB_PATH = x86_64-linux-gnu
    } else {
        LIB_PATH = i386-linux-gnu
    }
}

contains(BUILD_MODE,lib) {
    isEmpty(PREFIX) {
        INSTALL_HEADER = $$[QT_INSTALL_HEADERS]
        INSTALL_LIBS = $$[QT_INSTALL_LIBS]
    } else {
        INSTALL_HEADER = $$PREFIX/include
        INSTALL_LIBS = $$PREFIX/lib/$$LIB_PATH
    }

    DEFINES += BUILD_MODE_LIB
    INSTALL_PREFIX = $$INSTALL_HEADER/telegramqml
    INSTALL_HEADERS = $$HEADERS
    include(qmake/headerinstall.pri)

    target = $$TARGET
    target.path = $$INSTALL_LIBS

    INSTALLS += target
} else {
    CONFIG += plugin
    DEFINES += BUILD_MODE_PLUGIN

    TARGET = $$qtLibraryTarget($$TARGET)
    DISTFILES = qmldir \
        plugins.qmltypes

    !equals(_PRO_FILE_PWD_, $$OUT_PWD) {
        copy_qmldir.target = $$OUT_PWD/qmldir
        copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
        copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
        QMAKE_EXTRA_TARGETS += copy_qmldir
        PRE_TARGETDEPS += $$copy_qmldir.target
    }

    qmldir.files = qmldir plugins.qmltypes
    unix {
        installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
        qmldir.path = $$installPath
        target.path = $$installPath
        INSTALLS += target qmldir
    }
}

