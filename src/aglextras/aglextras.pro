TARGET = QtAGLExtras

QT += quick
QT += gui-private

include(hmi/hmi.pri)

qtConfig(agl_hmi_framework) {
    DEFINES += USE_AGL_HMI_LOWLEVEL_API
    QMAKE_USE += libhomescreen libwindowmanager
}

load(qt_module)
