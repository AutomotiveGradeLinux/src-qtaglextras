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
#ifndef AGLWMCLIENT_H
#define AGLWMCLIENT_H

#include <QtAGLExtras/aglextrasglobal.h>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE
class LibHomeScreen;
class LibWindowmanager;
class QQmlApplicationEngine;
class AGLWmClientPrivate;
class AGLEXTRAS_EXPORT AGLWmClient : public QObject
{
    Q_OBJECT

public:
    AGLWmClient (const QString& layout, int port, QString secret);
    void attach (QQmlApplicationEngine* engine);

private slots:
    void disconnect_frame_swapped (void);

private:
    void activate_window (void);
    friend class AGLWmClientPrivate;
    QSharedPointer<AGLWmClientPrivate> d_ptr;
    QSharedPointer<LibHomeScreen> h_ptr;
    QSharedPointer<LibWindowmanager> w_ptr;
};
QT_END_NAMESPACE

#endif // AGLWMCLIENT_H
