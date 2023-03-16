QT       += core gui multimedia multimediawidgets  core5compat
#QT += testlib
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
#CONFIG += release
CONFIG += debug
CONFIG += Wall
CONFIG += Wextra
CONFIG += fsanitize=undefined
CONFIG += fsanitize=address

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ComposeVideoManager.cpp \
    Dialog_SingleImgFormConfig.cpp \
    Dialog_VideoJParameterSetting.cpp \
    EditImgForm.cpp \
    ImgForm.cpp \
    PlayVideo.cpp \
    RectText.cpp \
    VMItem.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    ComposeVideoManager.h \
    Dialog_SingleImgFormConfig.h \
    Dialog_VideoJParameterSetting.h \
    EditImgForm.h \
    ImgForm.h \
    MainWindow.h \
    PlayVideo.h \
    RectText.h \
    Share.h \
    VMItem.h

FORMS += \
    Dialog_SingleImgFormConfig.ui \
    Dialog_VideoJParameterSetting.ui \
    EditImgForm.ui \
    MainWindow.ui \
    PlayVideo.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
	Resource.qrc


##msvc {
#    QMAKE_CFLAGS += /utf-8
##    QMAKE_CXXFLAGS += /utf-8
#    QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
##}

INCLUDEPATH += D:\OpenCV\opencv\build\include \
		D:\OpenCV\opencv\build\include\opencv2
#		D:\OpenCV\opencv\build\x64\vc15\lib
#LIBS += D:\OpenCV\opencv\build\x64\vc15\lib\opencv_world460.lib
#note:debug模式必须使用下面这个
LIBS += D:\OpenCV\opencv\build\x64\vc15\lib\opencv_world460d.lib

#	D:\OpenCV\opencv\build\x64\vc15\bin\opencv_videoio_msmf460_64d.dll
#LIBS += D:\OpenCV\opencv\build\x64\vc15\bin\opencv_videoio_ffmpeg460_64.dll

#管理员模式
QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
#QMAKE_LFLAGS += /MANIFESTUAC:"level='requireAdministrator'uiAccess='false'"

#win32: {
FFMPEG_HOME=D:\ffmpeg\ffmpeg-6.0-full_build-shared
#设置 ffmpeg 的头文件
INCLUDEPATH += $$FFMPEG_HOME/include

#设置导入库的目录一边程序可以找到导入库
# -L ：指定导入库的目录
# -l ：指定要导入的 库名称
LIBS +=  -L$$FFMPEG_HOME/lib \
     -lavcodec \
     -lavdevice \
     -lavfilter \
    -lavformat \
    -lavutil \
    -lpostproc \
    -lswresample \
    -lswscale
#}

