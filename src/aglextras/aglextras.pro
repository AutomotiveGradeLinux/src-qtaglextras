TARGET = QtAGLExtras

QT += quick
QT += core-private

include(hmi/hmi.pri)

qtConfig(agl_hmi_framework) {
    DEFINES += USE_AGL_HMI_LOWLEVEL_API
    QMAKE_USE += libhomescreen libwindowmanager
}

load(qt_module)
