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
#include "aglwmclient.h"
#include "aglwmclient_p.h"

#include <QtQuick/QQuickWindow>
#include <QtQml/QQmlApplicationEngine>

#ifdef USE_AGL_HMI_LOWLEVEL_API
#include <libhomescreen.hpp>
#include <libwindowmanager.h>
#endif  // USE_AGL_HMI_LOWLEVEL_API

QT_BEGIN_NAMESPACE

AGLWmClientPrivate::AGLWmClientPrivate (AGLWmClient*)
{
    ;
}

AGLWmClientPrivate::~AGLWmClientPrivate (void)
{
    ;
}

void AGLWmClient::activate_window (void)
{
#ifdef USE_AGL_HMI_LOWLEVEL_API
    json_object *obj = json_object_new_object();
    std::string target = QString(d_ptr->layout).toStdString();

    // request area "normal.full"
    std::string area = w_ptr->kStrLayoutNormal + "." + w_ptr->kStrAreaFull;
    json_object_object_add(obj, w_ptr->kKeyDrawingName, json_object_new_string(target.c_str()));
    json_object_object_add(obj, w_ptr->kKeyDrawingArea, json_object_new_string(area.c_str()));

    w_ptr->activateWindow(obj);
#endif  // USE_AGL_HMI_LOWLEVEL_API
}

void AGLWmClient::publishSubscription (void)
{
    h_ptr->publishSubscription();
}

void AGLWmClient::disconnect_frame_swapped (void)
{
#ifdef USE_AGL_HMI_LOWLEVEL_API
    qDebug("Disconnect!! SLOT disconnect_frame_swapped()");
    disconnect(d_ptr->loading);

//    activate_window();
    publishSubscription();
#endif  // USE_AGL_HMI_LOWLEVEL_API
}

double AGLWmClient::get_scale_factor (void)
{
    return d_ptr->scale_factor;
}

#ifdef USE_AGL_HMI_LOWLEVEL_API
void AGLWmClient::attach (QQmlApplicationEngine* engine)
{
    if (d_ptr->binding) {
        QQuickWindow *window = qobject_cast<QQuickWindow *>(engine->rootObjects().first());

        qDebug("Connect!! frameSapped! (activate_window())");
        d_ptr->loading = connect(window, SIGNAL(frameSwapped()),
                                 this, SLOT(disconnect_frame_swapped()));
    }
}
#else
void AGLWmClient::attach (QQmlApplicationEngine* engine)
{
    QQuickWindow *window = qobject_cast<QQuickWindow *>(engine->rootObjects().first());
    window->resize(1080, 1920 - 218 - 215);
    window->setVisible(true);
}
#endif  // USE_AGL_HMI_LOWLEVEL_API

#ifdef USE_AGL_HMI_LOWLEVEL_API
AGLWmClient::AGLWmClient (const QString& layout, int port, QString secret)
    : d_ptr(new AGLWmClientPrivate(this)),
      h_ptr(new LibHomeScreen()),
      w_ptr(new LibWindowmanager())
#else
AGLWmClient::AGLWmClient (const QString&, int port, QString)
    : d_ptr(new AGLWmClientPrivate(this))
#endif  // USE_AGL_HMI_LOWLEVEL_API
{
    if (port == -1) {
        qDebug("This is not AGL binding application");
        return;
    }
#ifdef USE_AGL_HMI_LOWLEVEL_API
    if (w_ptr->init(port, secret.toStdString().c_str()) != 0 ||
        h_ptr->init(port, secret.toStdString().c_str()) != 0) {
        qDebug("Cannot get binding API");
        return;
    }

    // Get information of screen
    {
        json_object *obj = json_object_new_object();
        int ret = w_ptr->getDisplayInfo(obj);
        if (!ret) {
            json_object *j_val;
            if (json_object_object_get_ex(obj, "scale", &j_val)) {
                d_ptr->scale_factor = json_object_get_double(j_val);
            }
        }
        json_object_put(obj);
    }

    d_ptr->binding = true;
    d_ptr->layout = layout;

    // Request a surface as described in layers.json windowmanager’s file
    std::string role = layout.toStdString();

    json_object *obj = json_object_new_object();
    json_object_object_add(obj, w_ptr->kKeyDrawingName, json_object_new_string(role.c_str()));

    int surface_id = w_ptr->requestSurface(obj);
    if (surface_id < 0) {
        qDebug("Cannot get surface for %s", qPrintable(d_ptr->layout));
        return;
    } else {
        qDebug("SurfaceID is set to %d", surface_id);
        std::string ivi_id = std::to_string(surface_id);
        setenv("QT_IVI_SURFACE_ID", ivi_id.c_str(), true);
    }


    // Create an event callback against an event type. Here a lambda is called when SyncDraw event occurs
    w_ptr->set_event_handler (
        LibWindowmanager::Event_SyncDraw,
        [this](json_object*) {
            qDebug("Surface got syncDraw!\n");
            json_object *obj = json_object_new_object();
            std::string target = QString(d_ptr->layout).toStdString();
            json_object_object_add(obj, w_ptr->kKeyDrawingName, json_object_new_string(target.c_str()));
            w_ptr->endDraw(obj);
        }
        );

        // Set the event handler for Event_ShowWindow which will activate the surface for windowmanager
    h_ptr->set_event_handler (
        LibHomeScreen::Event_ShowWindow,
        [this](json_object* object) {
            qDebug("Surface got showWindow\n");
            activate_window();
        }
        );
#endif  // USE_AGL_HMI_LOWLEVEL_API
}

QT_END_NAMESPACE
