TEMPLATE = lib
TARGET = telegramqml
DEFINES += TELEGRAMQML_LIBRARY
DEFINES += TELEGRAMQML_QML_PLUGIN ASEMAN_STATIC_BUILD
CONFIG += qt no_keywords c++11

uri = TelegramQml

win32 {
    QT += winextras
} else {
    macx {
        QT += macextras
    }
}

include(telegramqml.pri)

linux {
    contains(QT_ARCH, x86_64) {
        LIB_PATH = x86_64-linux-gnu
    } else:contains(QT_ARCH, arm) {
        LIB_PATH = arm-linux-gnueabihf
    } else {
        LIB_PATH = i386-linux-gnu
    }
    contains(DEFINES, UBUNTU_PHONE) {
        CONFIG += link_pkgconfig
        PKGCONFIG += libthumbnailer-qt
    }
}

contains(BUILD_MODE,lib) {
    isEmpty(PREFIX) {
        isEmpty(INSTALL_HEADERS_PREFIX): INSTALL_HEADERS_PREFIX = $$[QT_INSTALL_HEADERS]
        isEmpty(INSTALL_LIBS_PREFIX): INSTALL_LIBS_PREFIX = $$[QT_INSTALL_LIBS]
    } else {
        isEmpty(INSTALL_HEADERS_PREFIX): INSTALL_HEADERS_PREFIX = $$PREFIX/include
        isEmpty(INSTALL_LIBS_PREFIX): INSTALL_LIBS_PREFIX = $$PREFIX/lib/$$LIB_PATH
    }

    DEFINES += BUILD_MODE_LIB

    !contains(CONFIG, no_install) {
        INSTALL_PREFIX = $$INSTALL_HEADERS_PREFIX/telegramqml
        INSTALL_HEADERS = $$HEADERS
        include(qmake/headerinstall.pri)

        target = $$TARGET
        target.path = $$INSTALL_LIBS_PREFIX

        INSTALLS += target
    }
} else {
    CONFIG += plugin
    DEFINES += BUILD_MODE_PLUGIN

    DESTDIR = TelegramQml
    TARGET = $$qtLibraryTarget($$TARGET)
    DISTFILES = qmldir \
        plugins.qmltypes

    !equals(_PRO_FILE_PWD_, $$OUT_PWD) {
        copy_qmldir.target = $$OUT_PWD/$$DESTDIR/qmldir
        copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
        copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"

        copy_qmltypes.target = $$OUT_PWD/$$DESTDIR/plugins.qmltypes
        copy_qmltypes.depends = $$_PRO_FILE_PWD_/plugins.qmltypes
        copy_qmltypes.commands = $(COPY_FILE) \"$$replace(copy_qmltypes.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmltypes.target, /, $$QMAKE_DIR_SEP)\"

        QMAKE_EXTRA_TARGETS += copy_qmldir copy_qmltypes
        PRE_TARGETDEPS += $$copy_qmldir.target $$copy_qmltypes.target
    }

    qmldir.files = qmldir plugins.qmltypes

    unix {
        !contains(CONFIG, no_install) {
            installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
            qmldir.path = $$installPath
            target.path = $$installPath
            INSTALLS += target qmldir
        }
    }
}
