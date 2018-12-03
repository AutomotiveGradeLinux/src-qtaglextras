TARGET = QtAGLExtras

QT += quick
QT += core-private

include(hmi/hmi.pri)

qtConfig(agl_hmi_framework) {
    DEFINES += USE_AGL_HMI_LOWLEVEL_API
    QMAKE_USE += libhomescreen libwindowmanager
}
DEFINES += USE_AGL_HMI_LOWLEVEL_API
CONFIG += link_pkgconfig
PKGCONFIG += libwindowmanager libhomescreen
LIBS += -ljson-c
load(qt_module)
