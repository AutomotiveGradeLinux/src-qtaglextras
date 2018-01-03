TARGET = QtAGLExtras

QT += quick
QT += core-private

include(hmi/hmi.pri)

qtConfig(agl_hmi_framework) {
    DEFINES += USE_AGL_HMI_FRAMEWORK
    QMAKE_USE += libhomescreen qlibwindowmanager
}

load(qt_module)
