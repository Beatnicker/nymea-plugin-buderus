/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2017 Christian Fetzer <fetzer.ch@gmail.com>              *
 *                                                                         *
 *  This file is part of guh.                                              *
 *                                                                         *
 *  Guh is free software: you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, version 2 of the License.                *
 *                                                                         *
 *  Guh is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with guh. If not, see <http://www.gnu.org/licenses/>.            *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef INTEGRATIONPLUGINBUDERUS_H
#define INTEGRATIONPLUGINBUDERUS_H

#include "integrations/integrationplugin.h"
#include "plugintimer.h"
#include <QNetworkReply>

class IntegrationPluginBuderus: public IntegrationPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "io.nymea.IntegrationPlugin" FILE "integrationpluginBuderus.json")
    Q_INTERFACES(IntegrationPlugin)

public:

    explicit IntegrationPluginBuderus();

    void init() override;

    void setupThing(ThingSetupInfo *info) override;

    void postSetupThing(Thing *thing) override;

    void thingRemoved(Thing *thing) override;

private:
    PluginTimer *m_pluginTimer = nullptr;


    QString m_uuidUrl = "/gateway/uuid";
    QString m_versionFirmwareUrl = "/gateway/versionFirmware";
    QString m_temperatureOutdoorUrl = "/system/sensors/temperatures/outdoor_t1";


    struct Request {
        enum class Type {
            Unknown,
            Param,
            State
        };

        Request(QString url, const ParamTypeId &paramTypeId,Thing *thing)
            : url{url}, param{paramTypeId}, thing{thing}, type{Type::Param}
        {}

        Request(QString url, const StateTypeId &stateTypeId, Thing *thing)
            : url{url}, state{stateTypeId}, thing{thing}, type{Type::State}
        {}

        Request() = default;

        QString url;
        ParamTypeId param;
        StateTypeId state;
        Thing *thing = nullptr;
        Type type = Type::Unknown;
    };

    void sendAsyncRequest(const Request &request);
    QVariant parseValue(Thing *thing, const QByteArray &responseText,
                        const QString &key = QStringLiteral("value"));
    static QByteArray decrypt(const QByteArray &encrypted, const QByteArray &key);


};

#endif // INTEGRATIONPLUGINBUDERUS_H
