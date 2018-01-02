/*
 * Copyright (c) 2017 TOYOTA MOTOR CORPORATION
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


#include <QtCore/QDebug>
#include <QtCore/QCommandLineParser>
#include <QtCore/QUrlQuery>
#include <QtCore/QSettings>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQuickControls2/QQuickStyle>
#include <QQuickWindow>

#include <libhomescreen/libhomescreen.hpp>
#include "wmhandler.h"
#include "smhandler.h"


static LibHomeScreen*     hs;
static LibWindowmanager*  wm;
static LibSMWrapper*      smw;
static WmHandler*         wmh;

static std::string myname = std::string("Templete");


static void onRep(struct json_object* reply_contents);
static void onEv(const std::string& event, struct json_object* event_contents);


int main(int argc, char *argv[])
{
    QGuiApplication       app(argc, argv);
    QQmlApplicationEngine engine;
    QQmlContext*          context = engine.rootContext();
    QObject*              root;
    QQuickWindow*         window;

    QQuickStyle::setStyle("AGL");

    /*
     * Set argument and option
     */
    QCommandLineParser parser;
    parser.addPositionalArgument("port", app.translate("main", "port for binding"));
    parser.addPositionalArgument("secret", app.translate("main", "secret for binding"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);
    QStringList positionalArguments = parser.positionalArguments();

    if (positionalArguments.length() == 2) {
        /*
         * Get argument
         */
        int port = positionalArguments.takeFirst().toInt();
        QString secret = positionalArguments.takeFirst();
        std::string token = secret.toStdString();


        /*
         * Get instance
         */
        hs = new LibHomeScreen();
        wm = new LibWindowmanager();
        smw = new LibSMWrapper(port, secret);
        wmh = new WmHandler();


        /*
         * Set WindowManager
         */
        // Initialize
        if(wm->init(port, token.c_str()) != 0){
            exit(EXIT_FAILURE);
        }

        // Application should call requestSurface at first
        json_object *obj = json_object_new_object();
        json_object_object_add(obj, wm->kKeyDrawingName, json_object_new_string(app_name.c_str()));
        if (wm->requestSurface(obj) != 0) {
            exit(EXIT_FAILURE);
        }

        // Set event handlers for each event
        wm->set_event_handler(LibWindowmanager::Event_Active, [wm](json_object *object) {
            const char *label = json_object_get_string(
                json_object_object_get(object, wm->kKeyDrawingName));
            fprintf(stderr, "Surface %s got activated!\n", label);
        });
        wm->set_event_handler(LibWindowmanager::Event_Inactive, [wm](json_object *object) {
            const char *label = json_object_get_string(
                json_object_object_get(object, wm->kKeyDrawingName));
            fprintf(stderr, "Surface %s got deactivated!\n", label);
        });
        wm->set_event_handler(LibWindowmanager::Event_Visible, [wm](json_object *object) {
            const char *label = json_object_get_string(
                json_object_object_get(object, wm->kKeyDrawingName));
            fprintf(stderr, "Surface %s got visible!\n", label);
        });
        wm->set_event_handler(LibWindowmanager::Event_Invisible, [wm](json_object *object) {
            const char *label = json_object_get_string(
                json_object_object_get(object, wm->kKeyDrawingName));
            fprintf(stderr, "Surface %s got invisible!\n", label);
        });
        wm->set_event_handler(LibWindowmanager::Event_SyncDraw, [wm](json_object *object) {
            const char *label = json_object_get_string(
                json_object_object_get(object, wm->kKeyDrawingName));
            const char *area = json_object_get_string(
                json_object_object_get(object, wm->kKeyDrawingArea));
                fprintf(stderr, "Surface %s got syncDraw!\n", label);
            // Application should call LibWindowmanager::endDraw() in SyncDraw handler
            json_object *obj = json_object_new_object();
            json_object_object_add(obj, wm->kKeyDrawingName, json_object_new_string(app_name.c_str()));
            wm->endDraw(obj);
        });
        wm->set_event_handler(LibWindowmanager::Event_FlushDraw, [wm](json_object *object) {
            const char *label = json_object_get_string(
                json_object_object_get(object, wm->kKeyDrawingName));
            fprintf(stderr, "Surface %s got flushDraw!\n", label);
        });

        // Initialize WmHandler
        wmh->init(wm, myname.c_str());


        /*
         * Set HomeScreen
         */
        // Initialize
        hs->init(port, token.c_str());

        // Set event handler
        hs->set_event_handler(LibHomeScreen::Event_TapShortcut, [wm](json_object *object) {
            const char *appname = json_object_get_string(
			    json_object_object_get(object, "application_name"));
            if(myname == appname) {
                qDebug("Surface %s got tapShortcut\n", appname);
                // Application should call LibWindowmanager::endDraw() in TapShortcut handler
				json_object *obj = json_object_new_object();
				json_object_object_add(obj, wm->kKeyDrawingName, json_object_new_string(app_name.c_str()));
				json_object_object_add(obj, wm->kKeyDrawingArea, json_object_new_string("normal.full"));
                wm->activateSurface(obj);
            }
        });

        /*
         * Set SoundManager
         */
        smw->wrapper_registerCallback(onEv, onRep);
        smw->subscribe(QString("newMainConnection"));
        smw->subscribe(QString("mainConnectionStateChanged"));
        smw->subscribe(QString("removedMainConnection"));
        smw->subscribe(QString("asyncSetSourceState"));
        smw->subscribe(QString("asyncConnect"));

        // Set context property for SoundManager
        context->setContextProperty("smw", smw);


        /*
         * Load qml
         */
        engine.load(QUrl(QStringLiteral("qrc:/QmlForThisApp.qml")));


        /*
         * Set slot for WindowManager and SoundManager
         */
        root = engine.rootObjects().first();
        window = qobject_cast<QQuickWindow *>(root);

        // Set slot for calling LibWindowmanager::activateSurface() when loading qml have completed
        QObject::connect(window, SIGNAL(frameSwapped()),
            wmh, SLOT(slotActivateSurface()));

        // Set slot for SoundManager
        QObject::connect(smw, SIGNAL(smEvent(QVariant, QVariant)),
            root, SLOT(slotEvent(QVariant, QVariant)));
        QObject::connect(smw, SIGNAL(smReply(QVariant)),
            root, SLOT(slotReply(QVariant)));
    }

    return app.exec();
}

static void onRep(struct json_object* reply_contents)
{
    qDebug("%s is called", __FUNCTION__);
    QString str = QString(json_object_get_string(reply_contents));
    QJsonParseError error;
    QJsonDocument jdoc = QJsonDocument::fromJson(str.toUtf8(), &error);
    QJsonObject jobj = jdoc.object();

    smw->emit_reply(jobj);
    json_object_put(reply_contents);
}

static void onEv(const std::string& event, struct json_object* event_contents)
{
    qDebug("%s is called", __FUNCTION__);
    const QString event_name = QString(event.c_str());
    QString str = QString(json_object_get_string(event_contents));
    QJsonParseError error;
    QJsonDocument jdoc = QJsonDocument::fromJson(str.toUtf8(), &error);
    const QJsonObject jobj = jdoc.object();
    smw->emit_event(event_name, jobj);

    json_object_put(event_contents);
}

