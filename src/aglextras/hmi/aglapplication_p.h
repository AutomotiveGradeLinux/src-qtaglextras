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
#ifndef AGLAPPLICATION_P_H
#define AGLAPPLICATION_P_H

QT_BEGIN_NAMESPACE
class QUrl;
class QQmlApplicationEngine;
class AGLWmClient;
class AGLApplication;
class AGLScreenInfo;
class AGLApplicationPrivate
{
  public:
    explicit AGLApplicationPrivate (AGLApplication *app);
    ~AGLApplicationPrivate (void);

  private:
    int     port = -1;
    QString secret;
    QUrl*   binding_address;
    QQmlApplicationEngine *engine;
    AGLWmClient* wmclient;
    AGLScreenInfo* screen_info;
    friend class AGLApplication;
};
QT_END_NAMESPACE

#endif // AGLAPPLICATION_P_H
