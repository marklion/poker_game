QT       += core gui network websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QT_MSG_UTILS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    game/game_control.cpp \
    game/game_module.cpp \
    game/game_view.cpp \
    login_view.cpp \
    main.cpp \
    initwindow.cpp \
    network_rest_util.cpp \
    register_view.cpp \
    msg-encode-utils/msg_encode_utils.cpp

HEADERS += \
    game/game_control.h \
    game/game_module.h \
    game/game_view.h \
    initwindow.h \
    login_view.h \
    network_rest_util.h \
    register_view.h \
    rest_api_json.h \
    msg-encode-utils/msg_encode_utils.h

TRANSLATIONS += \
    poker_game_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    cards_img.qrc
