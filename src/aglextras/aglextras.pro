TARGET = QtAGLExtras

CONFIG += link_pkgconfig
PKGCONFIG += libhomescreen qlibwindowmanager

QT += quick
QT += core-private

include(hmi/hmi.pri)

load(qt_module)

CONFIG-=create_cmake
