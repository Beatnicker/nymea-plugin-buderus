include($$[QT_INSTALL_PREFIX]/include/nymea/plugin.pri)

INCLUDEPATH += /home/beatnicker/github.com/libmaia
LIBS += /home/beatnicker/github.com/libmaia/libmaia.a


QT += network xml


INCLUDEPATH += \
    external

SOURCES += \
    external/Qt-AES/qaesencryption.cpp \
    integrationpluginBuderus.cpp

HEADERS += \
    external/Qt-AES/qaesencryption.h \
    integrationpluginBuderus.h

