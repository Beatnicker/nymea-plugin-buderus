include($$[QT_INSTALL_PREFIX]/include/nymea/plugin.pri)


QT += network xml


INCLUDEPATH += \
    external

SOURCES += \
    external/Qt-AES/qaesencryption.cpp \
    integrationpluginBuderus.cpp \
    oilboiler.cpp

HEADERS += \
    external/Qt-AES/qaesencryption.h \
    integrationpluginBuderus.h \
    oilboiler.h

DISTFILES += \
    integrationpluginBuderus.json

