# -------------------------------------------------
# Project created by QtCreator 2011-06-28T23:00:22
# -------------------------------------------------
QT += opengl
TARGET = leveler
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    tonemap.cpp \
    cubiccurve.cpp \
    cvtiff.cpp \
    quadcurve.cpp
HEADERS += mainwindow.h \
    tonemap.h \
    cubiccurve.h \
    cvtiff.h \
    quadcurve.h
FORMS += mainwindow.ui
INCLUDEPATH += /usr/local/include/opencv2
LIBS += -L/usr/local/lib \
    -lopencv_core \
    -lopencv_imgproc \
    -lopencv_highgui \
    -lopencv_ml \
    -lopencv_video \
    -lopencv_features2d \
    -lopencv_calib3d \
    -lopencv_objdetect \
    -lopencv_contrib \
    -lopencv_legacy \
    -lopencv_flann \
    -ltiff
