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
#ifndef AGLWINDOW_P_H
#define AGLWINDOW_P_H

QT_BEGIN_NAMESPACE
class AGLWmClientPrivate
{
  public:
    explicit AGLWmClientPrivate (AGLWmClient* wmclient);
    ~AGLWmClientPrivate (void);

  private:
    QString layout;  // id of layaout which is managed by AGL WM
    QMetaObject::Connection loading;
    bool binding = false;
    friend class AGLWmClient;
};
QT_END_NAMESPACE

#endif // AGLWMCLIENT_P_H
