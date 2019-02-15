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
#include "aglapplication.h"
#include "aglapplication_p.h"
#include "aglwmclient.h"
#include "aglwmclient_p.h"

#include <QtCore/QCommandLineParser>
#include <QtCore/QUrlQuery>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlApplicationEngine>

QT_BEGIN_NAMESPACE

AGLApplicationPrivate::AGLApplicationPrivate (AGLApplication *app)
        : binding_address(new QUrl)
        , engine(new QQmlApplicationEngine(app))
{
    ;
}

AGLApplicationPrivate::~AGLApplicationPrivate (void)
{
    ;
}

AGLApplication::AGLApplication (int& argc, char** argv)
        : QGuiApplication(argc, argv),
          d_ptr(new AGLApplicationPrivate(this))
{
    setApplicationVersion(QStringLiteral("4.99.5"));
    setOrganizationDomain(QStringLiteral("automotivelinux.org"));
    setOrganizationName(QStringLiteral("AutomotiveGradeLinux"));

    QCommandLineParser parser;
    parser.addPositionalArgument(QLatin1String("port"),
                                 translate("main", "port for binding"));
    parser.addPositionalArgument(QLatin1String("secret"),
                                 translate("main", "secret for binding"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(*this);
    QStringList positionalArguments = parser.positionalArguments();

    if (positionalArguments.length() < 2) {
        return;
    }

    d_ptr->port = positionalArguments.takeFirst().toInt();
    d_ptr->secret = positionalArguments.takeFirst();

    d_ptr->binding_address->setScheme(QStringLiteral("ws"));
    d_ptr->binding_address->setHost(QStringLiteral("localhost"));
    d_ptr->binding_address->setPort(d_ptr->port);
    d_ptr->binding_address->setPath(QStringLiteral("/api"));

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("token"), d_ptr->secret);
    d_ptr->binding_address->setQuery(query);

    if (!d_ptr->engine) {
        qDebug("Cannot initialize QML Engine");
        exit(EXIT_FAILURE);
    }

    d_ptr->engine->rootContext()->setContextProperty(QStringLiteral("bindingAddress"),
                                                     *d_ptr->binding_address);
}

void
AGLApplication::load (const QUrl &url)
{
    d_ptr->engine->load(url);
}

void
AGLApplication::load (const QString &filePath)
{
    d_ptr->engine->load(filePath);
}

int
AGLApplication::exec (void)
{
    if (d_ptr->wmclient) {
        d_ptr->wmclient->attach(d_ptr->engine);
    }
    return QGuiApplication::exec();
}

void AGLApplication::setupApplicationRole (const QString &role)
{
    d_ptr->wmclient = new AGLWmClient(role, d_ptr->port, d_ptr->secret);
    d_ptr->screen_info = new AGLScreenInfo(this, d_ptr->wmclient->get_scale_factor());
    d_ptr->engine->rootContext()->setContextProperty(QStringLiteral("screenInfo"),
                                                     d_ptr->screen_info);
    QObject::connect(d_ptr->wmclient, SIGNAL(sizeChanged(int, int)),
                     d_ptr->screen_info, SLOT(setSize(int, int)));
}

QT_END_NAMESPACE
