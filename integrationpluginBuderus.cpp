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

#include "plugininfo.h"
#include "integrationpluginBuderus.h"
#include "network/networkaccessmanager.h"


#include "Qt-AES/qaesencryption.h"

#include <QJsonDocument>
#include <QNetworkRequest>




IntegrationPluginBuderus::IntegrationPluginBuderus()
{

}

void IntegrationPluginBuderus::init()
{
    // Initialisation can be done here.
    qCDebug(dcBuderus()) << " IntegrationPluginBuderus Plugin initialized.";
}

void IntegrationPluginBuderus::setupThing(ThingSetupInfo *info)
{
    // A thing is being set up. Use info->thing() to get details of the thing, do
    // the required setup (e.g. connect to the device) and call info->finish() when done.
    qCInfo(dcBuderus()) << "Setup thing" << info->thing();
    
    info->finish(Thing::ThingErrorNoError);

    Thing *thing = info->thing();

    if(thing->thingClassId() == buderusGatewayThingClassId){
        auto host = thing->paramValue(buderusGatewayThingBuderusHostParamTypeId).toString();
        auto port = thing->paramValue(buderusGatewayThingBuderusPortParamTypeId).toString();
        qCDebug(dcBuderus()) << "Configuring device" << QString("http://%1:%2").arg(host).arg(port);

        sendAsyncRequest(Request{m_uuidUrl, buderusGatewayThingBuderusIdParamTypeId, thing});
        sendAsyncRequest(Request{m_versionFirmwareUrl, buderusGatewayThingBuderusVersionParamTypeId, thing});

    }

}

void IntegrationPluginBuderus::postSetupThing(Thing *thing)
{
    qCDebug(dcBuderus()) << "Post setup Thing" << thing;
    if (thing->thingClassId() == buderusGatewayThingClassId) {
        if (!m_pluginTimer) {
            qCDebug(dcBuderus()) << "Starting plugin timer...";
            m_pluginTimer = hardwareManager()->pluginTimerManager()->registerTimer(10);
            connect(m_pluginTimer, &PluginTimer::timeout, this, [this] {
                foreach (Thing *myThing, myThings()) {
                    sendAsyncRequest(Request{m_temperatureOutdoorUrl, buderusGatewayTemperatureOutdoorStateTypeId, myThing});
                }
            });

            m_pluginTimer->start();
        }
    }

}
void IntegrationPluginBuderus::thingRemoved(Thing *thing)
{
    // A thing is being removed from the system. Do the required cleanup
    // (e.g. disconnect from the device) here.

    qCInfo(dcBuderus()) << "Remove thing" << thing;


}

void IntegrationPluginBuderus::sendAsyncRequest(const Request &request)
{

    auto host = request.thing->paramValue(buderusGatewayThingBuderusHostParamTypeId).toString();
    auto port = request.thing->paramValue(buderusGatewayThingBuderusPortParamTypeId).toString();
    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);

    QNetworkRequest networkRequest(QUrl(QString("http://%1:%2%3").arg(host).arg(port).arg(request.url)));
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader, "TeleHeater/2.2.3");
    networkRequest.setRawHeader("Accept", "application/json");
    qCDebug(dcBuderus) << "Sending request:" << request.url;
    mgr->get(networkRequest);

    QNetworkReply *reply = hardwareManager()->networkManager()->get(networkRequest);
    connect(reply,&QNetworkReply::finished,reply,&QNetworkReply::deleteLater);

    connect(reply,&QNetworkReply::finished, reply , [this, reply, request]{
        Thing *thing = request.thing;

        if (!reply->error()) {
            auto value = parseValue(request.thing , reply->readAll());
            qCDebug(dcBuderus) << "Received value:" << request.url << value;
            if (request.type == Request::Type::Param) {
                thing->setParamValue(request.param, value);
                if (request.url == m_versionFirmwareUrl) {
                    thing->setStateValue(buderusGatewayConnectedStateTypeId,true);
                }
            } else if (request.type == Request::Type::State) {
                thing ->setStateValue(request.state, value);
            }

            // Update the timestamp only for 1 value per request type.
            if (request.url == m_temperatureOutdoorUrl || request.url == m_versionFirmwareUrl) {
                thing->setStateValue(buderusGatewayUpdateTimeStateTypeId, QDateTime::currentMSecsSinceEpoch() / 1000);
            }
        } else {
            qCWarning(dcBuderus) << "Reply error" << reply->errorString();
            if (request.url == m_versionFirmwareUrl) {
                thing->setStateValue(buderusGatewayConnectedStateTypeId,false);
            }
        }
    });

}

QVariant IntegrationPluginBuderus::parseValue(Thing *thing, const QByteArray &responseText,
                                         const QString &key)
{
    QByteArray decryptionKey = thing->paramValue(buderusGatewayThingBuderusKeyParamTypeId).toString().toLocal8Bit();
    auto text = decrypt(responseText, decryptionKey);
    auto json = QJsonDocument::fromJson(text);
    auto result = json.object().value(key).toVariant();
    return result;
}

QByteArray IntegrationPluginBuderus::decrypt(const QByteArray &encrypted, const QByteArray &key)
{
    QAESEncryption encryption{QAESEncryption::AES_256, QAESEncryption::ECB};
    auto base64decoded = QByteArray::fromBase64(encrypted);
    auto decrypted = encryption.decode(base64decoded, QByteArray::fromHex(key));
    while (decrypted.endsWith('\0')) decrypted.chop(1);
    return decrypted;
}
