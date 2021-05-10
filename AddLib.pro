#-------------------------------------------------
#
# Project created by QtCreator 2018-05-31T10:35:02
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql

TARGET = AddLib
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    arduinoserial.cpp \
    reportwriter.cpp \
    product.cpp \
    productcube.cpp \
    plcconnect.cpp \
    snap7.cpp \
    verycam.cpp

HEADERS  += mainwindow.h \
    arduinoserial.h \
    reportwriter.h \
    product.h \
    productcube.h \
    plcconnect.h \
    snap7.h \
    verycam.h

FORMS    += mainwindow.ui

LIBS += "C:\Qt_New\Projects\SULIN2\build-CameraConnection-Desktop_Qt_5_14_2_MSVC2017_64bit-Release\release\CameraConnection.lib"
LIBS += "C:\Qt_New\Projects\2\NewBarcode\build-BarCodeProcessing-Desktop_Qt_5_14_2_MSVC2017_64bit-Release\release\BarCodeProcessing.lib"


INCLUDEPATH += "C:/Program Files/Basler/pylon 6/Development/include"
  LIBS += "-LC:/Program Files/Basler/pylon 6/Development/lib/x64" \
 -lGenApi_MD_VC141_v3_1_Basler_pylon \
    -lGCBase_MD_VC141_v3_1_Basler_pylon \
    -lPylonBase_v6_1 \

INCLUDEPATH+= "C:\\Opencv4.2.0\\include"\

LIBS+="C:\Opencv4.2.0\x64\vc15\lib\opencv_imgproc420.lib"
LIBS+="C:\Opencv4.2.0\x64\vc15\lib\opencv_imgcodecs420.lib"
LIBS+="C:\Opencv4.2.0\x64\vc15\lib\opencv_core420.lib"
LIBS+="C:\Opencv4.2.0\x64\vc15\lib\opencv_highgui420.lib"
LIBS+="C:\Opencv4.2.0\x64\vc15\lib\opencv_videoio420.lib"

LIBS+="C:\Opencv4.2.0\x64\vc15\lib\opencv_video420.lib"
LIBS+="C:\Opencv4.2.0\x64\vc15\lib\opencv_objdetect420.lib"

LIBS += "C:\Qt\Projects\qr1\build-AddLib-Desktop_Qt_5_3_MSVC2013_OpenGL_64bit-Release\release\snap7.lib"

