QT += core gui widgets
CONFIG += c++11

TARGET = waterfall
TEMPLATE = app

GIT_DESCRIBE = $$system(git describe --long --always)
DEFINES += GIT_DESCRIBE=\\\"$$GIT_DESCRIBE\\\"
win32: {
  GIT_DESCRIBE_RC = $$replace(GIT_DESCRIBE, \
    "^(\\d+)\\.(\\d+)\\.(\\d+)-(\\d+)-.*$", \
    "\\1, \\2, \\3, \\4" \
  )
  DEFINES += GIT_DESCRIBE_RC=\"$${GIT_DESCRIBE_RC}\"
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    central_widget.cpp

HEADERS += \ 
    mainwindow.h \
    central_widget.h

RESOURCES += \
    resources.qrc

RC_FILE += \
    resources.rc
