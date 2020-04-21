#include "httpAPI.h"
#include "libHTTP.h"
#include "settings.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QDebug>
#include <QByteArray>
#include <QCoreApplication>
#include <QThread>
#include <QUuid>

HttpAPI::HttpAPI(QObject *parent) : QObject(parent)
{
    caPath=QCoreApplication::applicationDirPath()+"/boot/cacert.crt";
    apiId="123456";
    apiKey="1285384ddfb057814bad78127bc789be";
    faceIpAddr=LocalSettings::config->value("Service/ip_FlightPlan").toString();
    faceApiAddr="/api/v1/face/boarding/flightplan";

    flowReviewServer = LocalSettings::config->value("Server/flowReviewServer").toString();
    flowReviewUrl = "/api/v1/face/data/flowback-query";
    flightReviewServer = LocalSettings::config->value("Server/flightReviewServer").toString();
    flightReviewUrl = "/api/v1/face/boarding/passenger-query";
    libDeleteUrl = "/api/v1/face/boarding/lib-delete";

    Init();
}

HttpAPI::~HttpAPI()
{
    UnInit();
}

int HttpAPI::Init()
{
    int res=-1;
    try {
        char* capathchr=caPath.toLatin1().data();

        res=LIBInit(capathchr);
        if(res!=0)
        {
            qCritical()<<QString("LIBInit res:%1").arg(res);
            return res;
        }

        //
        QByteArray header1;
        QJsonObject json1;

        json1.insert("Content-Type","application/json");
        json1.insert("apiId",apiId);
        json1.insert("apiKey",apiKey);
        json1.insert("url",faceApiAddr);
        header1.append(QJsonDocument(json1).toJson());

        char* headchr1=header1.data();
        QString faceurl=faceIpAddr+faceApiAddr;
        char* faceurlchr=faceurl.toLatin1().data();

        res=LIBSetHeaderEx(faceurlchr,headchr1);
        if(res!=0)
        {
            qCritical()<<QString("LIBSetHeaderEx res:%1").arg(res);
            return  res;
        }

        // FlowReviewRequest
        QByteArray header2;
        QJsonObject json2;

        json2.insert("Content-Type", "application/json");
        json2.insert("apiId", apiId);
        json2.insert("apiKey", apiKey);
        json2.insert("url", flowReviewUrl);
        header2.append(QJsonDocument(json2).toJson());

        char *headchr2 = header2.data();
        QString url2 = flowReviewServer + flowReviewUrl;
        char *url2chr = url2.toLatin1().data();

        res = LIBSetHeaderEx(url2chr, headchr2);
        if (res != 0) {
            qCritical() << QString("LIBSetHeaderEx res: %1").arg(res);
            return res;
        }

        // FlightReviewRequest
        QByteArray header3;
        QJsonObject json3;

        json3.insert("Content-Type", "application/json");
        json3.insert("apiId", apiId);
        json3.insert("apiKey", apiKey);
        json3.insert("url", flightReviewUrl);
        header3.append(QJsonDocument(json3).toJson());

        char *headchr3 = header3.data();
        QString url3 = flightReviewServer + flightReviewUrl;
        char *url3chr = url3.toLatin1().data();

        res = LIBSetHeaderEx(url3chr, headchr3);
        if (res != 0) {
            qCritical() << QString("LIBSetHeaderEx res: %1").arg(res);
            return res;
        }

        return 0;
    } catch (std::exception &ex) {
        qCritical()<<ex.what();
        return res;
    }
}

int HttpAPI::UnInit()
{
    int res=-1;

    try {
        QString faceurl=faceIpAddr+faceApiAddr;
        char* faceurlchr=faceurl.toLatin1().data();

        res=LIBRemoveHeader(faceurlchr);
        res=LIBUnInit();
        return res;
    } catch (std::exception &ex) {
        qCritical()<<ex.what();
        return res;
    }
}

Return_Flight_Plan_Param HttpAPI::getFlightPlan(const Request_Flight_Plan_Param& data)
{
    Return_Flight_Plan_Param result;
    try {
        QByteArray bytes;
        QJsonObject json;

        json.insert("reqId",data.reqId);
        json.insert("gateNo",data.gateNo);
        bytes.append(QJsonDocument(json).toJson());

        char* body=bytes.data();
        char* response;
        int resLen=0;
        int res=-1;

        res=LIBPostSynEx(faceIpAddr.toLatin1().data(),faceApiAddr.toLatin1().data(),body,&response,&resLen,true);
        result.curl_err_code=res;
        if(res!=0 || resLen==0)
        {
            qCritical()<<QString("LIBPostSynEx failed, res:%1").arg(res)<<QThread::currentThread();
            return result;
        }

        QByteArray data1=QByteArray(response,resLen);

        if(data1!=nullptr)
        {
            QJsonParseError jsonParseError;
            QJsonDocument document = QJsonDocument::fromJson(data1, &jsonParseError);

            if(!document.isNull() && jsonParseError.error == QJsonParseError::NoError)
            {
                QJsonObject json1 = document.object();

                if (LocalSettings::config->value("isPrintResponse").toBool()){
                    qDebug()<<"resHttpRes:\n"<<json1;
                }
                if(json1.contains("reqId")){
                    result.reqId=json1["reqId"].toString();
                }
                if(json1.contains("status")){
                    result.serverStatus=json1["status"].toInt();
                }
                if(json1.contains("msg")){
                    result.msg=json1["msg"].toString();
                }
                if(json1.contains("flightPlan")){
                    QJsonObject flightPlan=json1["flightPlan"].toObject();

                    if (flightPlan.contains("threeFlightNo"))
                        result.flightPlan.threeFlightNo=flightPlan["threeFlightNo"].toString();
                    if (flightPlan.contains("twoFlightNo"))
                        result.flightPlan.twoFlightNo=flightPlan["twoFlightNo"].toString();
                    if (flightPlan.contains("depTimeJ"))
                        result.flightPlan.depTimeJ=flightPlan["depTimeJ"].toString();
                    if (flightPlan.contains("flightDate"))
                        result.flightPlan.flightDate=flightPlan["flightDate"].toString();
                    if (flightPlan.contains("status"))
                        result.flightPlan.status=flightPlan["status"].toInt();
                    if (flightPlan.contains("midwayCode"))
                        result.flightPlan.midwayCode=flightPlan["midwayCode"].toString();
                    if (flightPlan.contains("midwayName"))
                        result.flightPlan.midwayName=flightPlan["midwayName"].toString();
                    if (flightPlan.contains("arrAirportCode"))
                        result.flightPlan.arrAirportCode=flightPlan["arrAirportCode"].toString();
                    if (flightPlan.contains("arrAirportName"))
                        result.flightPlan.arrAirportName=flightPlan["arrAirportName"].toString();
                    if (flightPlan.contains("depAirportCode"))
                        result.flightPlan.depAirportCode=flightPlan["depAirportCode"].toString();
                    if (flightPlan.contains("depAirportName"))
                        result.flightPlan.depAirportName=flightPlan["depAirportName"].toString();
                    if (flightPlan.contains("kindType"))
                        result.flightPlan.kindType=flightPlan["kindType"].toInt();
                    if (flightPlan.contains("manulOpt"))
                        result.flightPlan.manulOpt=flightPlan["manulOpt"].toBool();
                    if (flightPlan.contains("boardingGate"))
                        result.flightPlan.boardingGate=flightPlan["boardingGate"].toBool();
                }
            }
        }
        return result;
    } catch (std::exception &ex) {
        qCritical()<<ex.what();
        return result;
    }
}

FlightReviewResponse HttpAPI::get(const FlightReviewRequest& request)
{
    FlightReviewResponse result;

    result.queryType = request.queryType;

    try {
        QByteArray bytes;
        QJsonObject json;

        QString uuid = QUuid::createUuid().toString();
        uuid.remove("{").remove("}").remove("-");
        json.insert("reqId", uuid);
        json.insert("flightNo", request.input);
        json.insert("date", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
        json.insert("queryType", request.queryType);
        json.insert("pageNum", 1);
        json.insert("pageSize", 1000);
        json.insert("isCount", 1);
        json.insert("gateNo", LocalSettings::config->value("Device/gateNo").toString());
        json.insert("boardingGate", LocalSettings::config->value("Device/boardingGate").toString());

        qDebug() << "FlightReviewRequest: " << json;

        bytes.append(QJsonDocument(json).toJson());

        char *response;
        int resLen = 0;
        int res = -1;

        res = LIBPostSynEx(flightReviewServer.toLatin1().data(), flightReviewUrl.toLatin1().data(), bytes.data(), &response, &resLen, true);
        result.errorCode = res;
        if(res != 0 || resLen == 0)
        {
            qCritical() << QString("LIBPostSynEx failed, res: %1").arg(res) << QThread::currentThread();
            return result;
        }

        QByteArray array = QByteArray(response, resLen);

        if(array != nullptr)
        {
            QJsonParseError jsonParseError;
            QJsonDocument document = QJsonDocument::fromJson(array, &jsonParseError);

//            qDebug() << "document: " << document;

            if(!document.isNull() && jsonParseError.error == QJsonParseError::NoError)
            {
                result.update(document);
            }
            if (LocalSettings::config->value("GateConnect/isPrintResponse").toBool()){
                qDebug() << "HTTP FLIGHT REVIEW RESPONSE: \n" << document;
            }
        }

        return result;
    } catch (std::exception &ex) {
        qCritical() << ex.what();

        return result;
    }
}

LibDeleteResponse HttpAPI::removeSpecific(const LibDeleteRequest& request)
{
    LibDeleteResponse result;

    try {
        QByteArray bytes;
        QJsonObject json;

        json.insert("reqId", request.reqId);
        json.insert("flightNo", request.flightNo);
        json.insert("gateNo", request.gateNo);
        json.insert("boardingGate", request.boardingGate);
        json.insert("deviceCode", request.deviceCode);
        json.insert("id", request.id);
        json.insert("date", request.date);

        qDebug() << "LibDeleteRequest: " << json;

        bytes.append(QJsonDocument(json).toJson());

        char *response;
        int resLen = 0;
        int res = -1;

        res = LIBPostSynEx(flightReviewServer.toLatin1().data(), libDeleteUrl.toLatin1().data(), bytes.data(), &response, &resLen, true);
        if (res != 0 || resLen == 0) {
            qCritical() << QString("LIBPostSynEx failed, res: %1").arg(res) << QThread::currentThread();
            return result;
        }

        QByteArray array = QByteArray(response, resLen);

        if (array != nullptr) {
            QJsonParseError jsonParseError;
            QJsonDocument document = QJsonDocument::fromJson(array, &jsonParseError);

//            qDebug() << "document: " << document;

            if (!document.isNull() && jsonParseError.error == QJsonParseError::NoError) {
                result.reqId = document.object().value("reqId").toString();
                result.status = document.object().value("status").toInt();
                result.msg = document.object().value("msg").toString();
            }
        }

        return  result;
    } catch (std::exception &e) {
        qCritical() << e.what();

        return result;
    }
}

FlowReviewResponse HttpAPI::get(const FlowReviewRequest& request)
{
    FlowReviewResponse result;

    try {
        QByteArray bytes;
        QJsonObject json;

        QString uuid = QUuid::createUuid().toString();
        uuid.remove("{").remove("}").remove("-");
        json.insert("reqId", uuid);
        // isFuzzyQuery的填写规则是,如果是通过身份证查询则填0,通过机票加航班号查询时,手动输入填1,刷票输入填0
        // flightDay的填写规则是,如果用扫码枪扫入,则将机票年月日的日填入,如果手动输入则为-1,查询历史所有
        if (request.input.contains("#", Qt::CaseSensitive)) {
            json.insert("cardId", "");
            json.insert("flightNo", request.input.section("#", 0, 0));
            json.insert("boardingNumber", request.input.section("#", 1, 1));
            json.insert("isFuzzyQuery", 1);
        } else {
            json.insert("cardId", request.input);
            json.insert("flightNo", "");
            json.insert("boardingNumber", "");
            json.insert("isFuzzyQuery", 0);
        }
        json.insert("flightDay", "-1");

        bytes.append(QJsonDocument(json).toJson());

        char *response;
        int resLen = 0;
        int res = -1;

        res = LIBPostSynEx(flowReviewServer.toLatin1().data(), flowReviewUrl.toLatin1().data(), bytes.data(), &response, &resLen, true);
        result.errorCode = res;

        if(res != 0 || resLen == 0)
        {
            qCritical() << QString("LIBPostSynEx failed, res: %1").arg(res) << QThread::currentThread();
            return result;
        }

        QByteArray array = QByteArray(response, resLen);

        if(array != nullptr)
        {
            QJsonParseError jsonParseError;
            QJsonDocument document = QJsonDocument::fromJson(array, &jsonParseError);

            if(!document.isNull() && jsonParseError.error == QJsonParseError::NoError)
            {
                result.update(document);
            }
        }

        return result;
    } catch (std::exception &ex) {
        qCritical() << ex.what();

        return result;
    }
}
