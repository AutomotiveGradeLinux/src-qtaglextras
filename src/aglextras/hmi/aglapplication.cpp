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

AGLApplicationPrivate::AGLApplicationPrivate (int &argc, char **argv, int flags)
    : QGuiApplicationPrivate (argc, argv, flags)
    , engine(nullptr)
{
    ;
}

AGLApplicationPrivate::~AGLApplicationPrivate (void)
{
    ;
}

AGLApplication::AGLApplication (int& argc, char** argv, int _internal)
    : QGuiApplication(*new AGLApplicationPrivate(argc, argv, _internal))
{
    Q_D(AGLApplication);
    d->init();

    setApplicationVersion(QStringLiteral("4.99.5"));
    setOrganizationDomain(QStringLiteral("automotivelinux.org"));
    setOrganizationName(QStringLiteral("AutomotiveGradeLinux"));

    d->engine = new QQmlApplicationEngine(this);

    QCommandLineParser parser;
    parser.addPositionalArgument(QLatin1String("port"),
                                 translate("main", "port for binding"));
    parser.addPositionalArgument(QLatin1String("secret"),
                                 translate("main", "secret for binding"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(*this);
    QStringList positionalArguments = parser.positionalArguments();

    if (positionalArguments.length() == 2) {
        d->port = positionalArguments.takeFirst().toInt();
        d->secret = positionalArguments.takeFirst();

        d->binding_address.setScheme(QStringLiteral("ws"));
        d->binding_address.setHost(QStringLiteral("localhost"));
        d->binding_address.setPort(d->port);
        d->binding_address.setPath(QStringLiteral("/api"));

        QUrlQuery query;
        query.addQueryItem(QStringLiteral("token"), d->secret);
        d->binding_address.setQuery(query);
    }

    d->engine->rootContext()->setContextProperty(QStringLiteral("bindingAddress"),
                                                     d->binding_address);
}

void
AGLApplication::load (const QUrl &url)
{
    Q_D(AGLApplication);
    d->engine->load(url);
}

void
AGLApplication::load (const QString &filePath)
{
    Q_D(AGLApplication);
    d->engine->load(filePath);
}

QQmlApplicationEngine*
AGLApplication::getQmlApplicationEngine() const {
    Q_D(const AGLApplication);
    return d->engine;
}

int
AGLApplication::exec (void)
{
    Q_D(AGLApplication);
    if (d->wmclient) {
        d->wmclient->attach(d->engine);
    }
    return QGuiApplication::exec();
}

void AGLApplication::setupApplicationRole (const QString &role)
{
    Q_D(AGLApplication);
    d->wmclient = new AGLWmClient(role, d->port, d->secret);
    d->screen_info = new AGLScreenInfo(this, d->wmclient->get_scale_factor());
    d->engine->rootContext()->setContextProperty(QStringLiteral("screenInfo"),
                                                     d->screen_info);
}

QT_END_NAMESPACE
