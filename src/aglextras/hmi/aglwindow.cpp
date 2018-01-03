/*
 * Copyright (c) 2017 TOYOTA MOTOR CORPORATION
 * Copyright (C) 2017 The Qt Company Ltd.
 * Copyright (c) 2017 Panasonic Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "aglwindow.h"
#include "aglwindow_p.h"

#include <QtQuick/QQuickWindow>
#include <QtQml/QQmlApplicationEngine>

#ifdef USE_AGL_HMI_FRAMEWORK
#include <libhomescreen.hpp>
#include <qlibwindowmanager.h>
#endif  // USE_AGL_HMI_FRAMEWORK

QT_BEGIN_NAMESPACE

AGLWindowPrivate::AGLWindowPrivate (AGLWindow*)
{
    ;
}

AGLWindowPrivate::~AGLWindowPrivate (void)
{
    ;
}

void AGLWindow::activate_surface (void)
{
#ifdef USE_AGL_HMI_FRAMEWORK
    qDebug("Disconnect!! SLOT activate_surface()");
    QLibWindowmanager::disconnect(d_ptr->loading);

    w_ptr->activateSurface(QString(d_ptr->layout));
#endif  // USE_AGL_HMI_FRAMEWORK
}

#ifdef USE_AGL_HMI_FRAMEWORK
void AGLWindow::attach (QQmlApplicationEngine* engine)
{
    if (d_ptr->binding) {
        QQuickWindow *window = qobject_cast<QQuickWindow *>(engine->rootObjects().first());

        qDebug("Connect!! frameSapped! (activate_surface())");
        d_ptr->loading = QLibWindowmanager::connect(window, SIGNAL(frameSwapped()),
                                                    this, SLOT(activate_surface()));
    }
}
#else
void AGLWindow::attach (QQmlApplicationEngine*)
{
    QQuickWindow *window = qobject_cast<QQuickWindow *>(engine->rootObjects().first());
    window->resize(1080, 1920 - 218 - 215);
    window->setVisible(true);
}
#endif  // USE_AGL_HMI_FRAMEWORK

#ifdef USE_AGL_HMI_FRAMEWORK
AGLWindow::AGLWindow (const QString& layout, int port, QString secret)
    : d_ptr(new AGLWindowPrivate(this)),
      h_ptr(new LibHomeScreen()),
      w_ptr(new QLibWindowmanager(nullptr))
#else
AGLWindow::AGLWindow (const QString&, int port, QString)
    : d_ptr(new AGLWindowPrivate(this))
#endif  // USE_AGL_HMI_FRAMEWORK
{
    if (port == -1) {
        qDebug("This is not AGL binding application");
        return;
    }
#ifdef USE_AGL_HMI_FRAMEWORK
    if (w_ptr->init(port, secret) != 0 ||
        h_ptr->init(port, secret.toStdString().c_str()) != 0) {
        qDebug("Cannot get binding API");
        return;
    }

    d_ptr->layout = layout;

    // Request a surface as described in layers.json windowmanager’s file
    if (w_ptr->requestSurface(d_ptr->layout) != 0) {
        qDebug("Cannot get surface for %s", qPrintable(d_ptr->layout));
        return;
    }

    d_ptr->binding = true;

    // Create an event callback against an event type. Here a lambda is called when SyncDraw event occurs
    w_ptr->set_event_handler (
        QLibWindowmanager::Event_SyncDraw,
        [this](json_object*) {
            qDebug("Surface got syncDraw!\n");
            w_ptr->endDraw(d_ptr->layout);
        }
        );

        // Set the event handler for Event_TapShortcut which will activate the surface for windowmanager
    h_ptr->set_event_handler (
        LibHomeScreen::Event_TapShortcut,
        [this](json_object* object) {
            json_object *jo_app_name = nullptr;
            if(json_object_object_get_ex(object, "application_name", &jo_app_name)) {
                QString name(QLatin1String(json_object_get_string(jo_app_name)));
                if(d_ptr->layout == name) {
                    qDebug("Surface %s got tapShortcut\n", qPrintable(name));
                    w_ptr->activateSurface(d_ptr->layout);
                }
            }
        }
        );
#endif  // USE_AGL_HMI_FRAMEWORK
}

QT_END_NAMESPACE
