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
#ifndef AGLAPPLICATION_H
#define AGLAPPLICATION_H

#include <QtAGLExtras/aglextrasglobal.h>

#include <QSharedPointer>

#include <QtGui/QGuiApplication>

class QQmlApplicationEngine;

QT_BEGIN_NAMESPACE
class AGLApplicationPrivate;
class AGLEXTRAS_EXPORT AGLApplication : public QGuiApplication
{
    Q_OBJECT

public:
    AGLApplication (int& argc, char** argv, int = ApplicationFlags);

    void setupApplicationRole(const QString& role);

    void load (const QUrl &url);
    void load (const QString &filePath);

    QQmlApplicationEngine* getQmlApplicationEngine() const;

    int exec (void);

private:
    Q_DISABLE_COPY(AGLApplication)
    Q_DECLARE_PRIVATE(AGLApplication)
};

class AGLScreenInfo : public QObject
{
    Q_OBJECT

public:
    AGLScreenInfo(QObject* parent = 0, double scale = 1.0) : QObject(parent), _scale_factor(scale) {};

    Q_INVOKABLE double scale_factor() const { return _scale_factor; };

private:
    double _scale_factor;
};
QT_END_NAMESPACE

#endif // AGLAPPLICATION_H
