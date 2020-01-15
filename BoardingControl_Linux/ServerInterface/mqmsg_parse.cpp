#include "mqmsg_parse.h"
#include "settings.h"

#include <QJsonDocument>
#include <QDebug>


MsgParse::MsgParse(QObject* parent)
    :AmqpImp(parent)
{
    initAmqp();
}

MsgParse::~MsgParse()
{
    if (ConnectionoptType){
        disconnect(ConnectionoptType);
    }
}

void MsgParse::initAmqp()
{
    QByteArray hostname = LocalSettings::config->value("amqpServer/hostname").toByteArray();
    int port=LocalSettings::config->value("amqpServer/port").toInt();
    QByteArray vhost = LocalSettings::config->value("amqpServer/vhost").toByteArray();
    QByteArray user_name=LocalSettings::config->value("amqpServer/user_name").toByteArray();
    QByteArray password=LocalSettings::config->value("amqpServer/password").toByteArray();
    QByteArray exchange_id=LocalSettings::config->value("amqpServer/exchange_id").toByteArray();
    QByteArray binding_key=LocalSettings::config->value("amqpServer/binding_key").toByteArray();
    int sec_heartbeat = LocalSettings::config->value("amqpServer/sec_heartbeat").toInt();

    ChannelType type=CHANNEL_CONSUMER;
    const char* queue_name = "bordingctrl";
    consumer_msg_cb callback = msgRecivedCallBack;
    void* userData = this;

    //创建通道,注册消息队列回调
//    create_channel(hostname.data(),  port,  vhost.data(),  user_name.data(), password.data(), type, queue_name, callback, userData, exchange_id.data(), binding_key.data(), sec_heartbeat);
    create_channel("175.168.1.199",  5672,  "/",  "root", "123456", type, "data_terminal_fanoutexchange", callback, userData, "data_terminal_fanoutexchange", "", sec_heartbeat);

}

void CALLBACK MsgParse::msgRecivedCallBack(const char* jsonMsg, void* userData)
{
    MsgParse *this_data = static_cast <MsgParse*> (userData);
    QByteArray byte_JsonMsg(jsonMsg);

//    this_data->recive_msg(byte_JsonMsg);

    qDebug()<<"mq_msg:\n"<<byte_JsonMsg;//debug Info
}

/*
void MsgParse::recive_msg(QByteArray &byte_jsonMsg)
{
    m_mutex.lock();

    if (byte_jsonMsg!=nullptr)
    {
        QJsonParseError jsonParseError;
        QJsonDocument jd_jsonMsg = QJsonDocument::fromJson(byte_jsonMsg, &jsonParseError);

        if (!jd_jsonMsg.isNull() && jsonParseError.error == QJsonParseError::NoError)
        {
             QJsonObject jo_jsonMsg = jd_jsonMsg.object();
            int n_type = 0;

            if (jo_jsonMsg.contains("optType")){
                 n_type = jo_jsonMsg["optType"].toInt();
            }
            switch (n_type) {
            case 100:
                if(jo_jsonMsg["msg"].isArray()){
                    QJsonArray arry_msg = jo_jsonMsg["msg"].toArray();
                    int size = arry_msg.size();
                    optType100(arry_msg, size);
                }
                else qWarning()<<"recived mq msg  Parsed, <optType=100>, but <msg> param error, only  'JsonArry' supported!";
                break;

            case 101:
                if (jo_jsonMsg["msg"].isObject()){
                    QJsonObject object_msg;
                    object_msg = jo_jsonMsg["msg"].toObject();
                    optType101(object_msg);
                }
                else qWarning()<<"recived mq msg  Parsed, <optType=101>, but <msg> param error, only  'JsonObject' supported!";
                break;

            case 102:
                if (jo_jsonMsg["msg"].isObject()){
                    QJsonObject object_msg;
                    object_msg = jo_jsonMsg["msg"].toObject();
                    optType102(object_msg);
                }
                else qWarning()<<"recived mq msg  Parsed, <optType=102>, but <msg> param error, only  'JsonObject' supported!";
                break;

            case 103:
                if (jo_jsonMsg["msg"].isObject()){
                    QJsonObject object_msg;
                    object_msg = jo_jsonMsg["msg"].toObject();
                    optType103(object_msg);
                }
                else qWarning()<<"recived mq msg  Parsed, <optType=103>, but <msg> param error, only  'JsonObject' supported!";
                break;

            case 109:
                if (jo_jsonMsg["msg"].isObject()){
                    QJsonObject object_msg;
                    object_msg = jo_jsonMsg["msg"].toObject();
                    optType109(object_msg);
                }
                else qWarning()<<"recived mq msg  Parsed, <optType=109>, but <msg> param error, only  'JsonObject' supported!";
                break;

            case 300:
                if (jo_jsonMsg["msg"].isObject()){
                    QJsonObject object_msg;
                    object_msg = jo_jsonMsg["msg"].toObject();
                    optType300(object_msg);
                }
                else qWarning()<<"recived mq msg  Parsed, <optType=300>, but <msg> param error, only  'JsonObject' supported!";
                break;

            case 301:
                if (jo_jsonMsg["msg"].isObject()){
                    QJsonObject object_msg;
                    object_msg = jo_jsonMsg["msg"].toObject();
                    optType301(object_msg);
                }
                else qWarning()<<"recived mq msg  Parsed, <optType=301>, but <msg> param error, only  'JsonObject' supported!";
                break;

            default:
                qWarning()<<QString("recived mq msg  parse to 'Json' ok, but unknowm <optType = %1> !").arg(n_type);
                break;
            }

        }
        else
        {
            qWarning()<<"mq msg recived  parse to 'Json' error!";
        }
    }

    m_mutex.unlock();
}

void MsgParse::optType100(QJsonArray &arry, int size)
{
    for (int i=0; i<size; i++){
        QJsonObject msg = arry[i].toObject();

        if (0 == QString::compare(msg.value("boardingGate").toString(), LocalSettings::config->value("Device/bordingGateNo").toString())){
            QJsonObject content = msg["content"].toObject();

            m_flight_plan.arrAirportCode=content["arrAirportCode"].toString();
            m_flight_plan.arrAirportName=content["arrAirportName"].toString();
            m_flight_plan.boardingGate=content["boardingGate"].toString();
            m_flight_plan.depAirportCode=content["depAirportCode"].toString();
            m_flight_plan.depAirportName=content["depAirportName"].toString();
            m_flight_plan.depTimeJ=content["depTimeJ"].toString();
            m_flight_plan.flightDate=content["flightDate"].toString();
            m_flight_plan.kindType=content["kindType"].toInt();
            m_flight_plan.manulOpt=content["manulOpt"].toBool();
            m_flight_plan.midwayCode=content["midwayCode"].toString();
            m_flight_plan.midwayName=content["midwayName"].toString();
            QString tmpStr=content["shareFltno"].toString();
//            m_flight_plan.shareFltno=tmpStr.split(",");
            m_flight_plan.status=content["status"].toInt();
            m_flight_plan.threeFlightNo=content["threeFlightNo"].toString();
            m_flight_plan.twoFlightNo=content["twoFlightNo"].toString();
            emit recived_mq_msg(100);
        }
    }
}

void MsgParse::optType101(QJsonObject &object)
{
    if (0 == QString::compare(object.value("boardingGate").toString(), LocalSettings::config->value("Device/bordingGateNo").toString())){
        QJsonObject content = object["content"].toObject();

        m_creat_repository.arrAirportCode=content["arrAirportCode"].toString();
        m_creat_repository.arrAirportName=content["arrAirportName"].toString();
        m_creat_repository.boardingGate=content["boardingGate"].toString();
        m_creat_repository.depAirportCode=content["depAirportCode"].toString();
        m_creat_repository.depAirportName=content["depAirportName"].toString();
        m_creat_repository.depTimeJ=content["depTimeJ"].toString();
        m_creat_repository.flightDate=content["flightDate"].toString();
        m_creat_repository.kindType=content["kindType"].toInt();
        m_creat_repository.manulOpt=content["manulOpt"].toBool();
        m_creat_repository.midwayCode=content["midwayCode"].toString();
        m_creat_repository.midwayName=content["midwayName"].toString();
        QString tmpStr=content["shareFltno"].toString();
        m_creat_repository.shareFltno=tmpStr.split(",");
        m_creat_repository.status=content["status"].toInt();
        m_creat_repository.threeFlightNo=content["threeFlightNo"].toString();
        m_creat_repository.twoFlightNo=content["twoFlightNo"].toString();
        emit recived_mq_msg(101);
    }
}

void MsgParse::optType102(QJsonObject &object)
{
    if (0 == QString::compare(object.value("boardingGate").toString(), LocalSettings::config->value("Device/bordingGateNo").toString())){
     QJsonObject content = object["content"].toObject();

     m_bording_start.arrAirportCode=content["arrAirportCode"].toString();
     m_bording_start.arrAirportName=content["arrAirportName"].toString();
     m_bording_start.boardingGate=content["boardingGate"].toString();
     m_bording_start.depAirportCode=content["depAirportCode"].toString();
     m_bording_start.depAirportName=content["depAirportName"].toString();
     m_bording_start.depTimeJ=content["depTimeJ"].toString();
     m_bording_start.flightDate=content["flightDate"].toString();
     m_bording_start.kindType=content["kindType"].toInt();
     m_bording_start.manulOpt=content["manulOpt"].toBool();
     m_bording_start.midwayCode=content["midwayCode"].toString();
     m_bording_start.midwayName=content["midwayName"].toString();
     QString tmpStr=content["shareFltno"].toString();
     m_bording_start.shareFltno=tmpStr.split(",");
     m_bording_start.status=content["status"].toInt();
     m_bording_start.threeFlightNo=content["threeFlightNo"].toString();
     m_bording_start.twoFlightNo=content["twoFlightNo"].toString();
     emit recived_mq_msg(102);
    }
}

void MsgParse::optType103(QJsonObject &object)
{
    if (0 == QString::compare(object.value("boardingGate").toString(), LocalSettings::config->value("Device/bordingGateNo").toString())){
     QJsonObject content = object["content"].toObject();

     m_boarding_end.arrAirportCode=content["arrAirportCode"].toString();
     m_boarding_end.arrAirportName=content["arrAirportName"].toString();
     m_boarding_end.boardingGate=content["boardingGate"].toString();
     m_boarding_end.boardingTime=content["boardingTime"].toString();
     m_boarding_end.depAirportCode=content["depAirportCode"].toString();
     m_boarding_end.depAirportName=content["depAirportName"].toString();
     m_boarding_end.depTimeJ=content["depTimeJ"].toString();
     m_boarding_end.flightDate=content["flightDate"].toString();
     m_boarding_end.kindType=content["kindType"].toInt();
     m_boarding_end.manulOpt=content["manulOpt"].toBool();
     m_boarding_end.midwayCode=content["midwayCode"].toString();
     m_boarding_end.midwayName=content["midwayName"].toString();
     QString tmpStr=content["shareFltno"].toString();
     m_boarding_end.shareFltno=tmpStr.split(",");
     m_boarding_end.status=content["status"].toInt();
     m_boarding_end.threeFlightNo=content["threeFlightNo"].toString();
     m_boarding_end.boardingNum=content["boardingNum"].toInt();
     m_boarding_end.orgDepNum=content["orgDepNum"].toInt();
     m_boarding_end.transferNum=content["transferNum"].toInt();
     m_boarding_end.midwayNum=content["midwayNum"].toInt();
     m_boarding_end.faceNums=content["faceNums"].toInt();
     m_boarding_end.lastRecogTime=content["lastRecogTime"].toString();
     m_boarding_end.twoFlightNo=content["twoFlightNo"].toString();
     emit recived_mq_msg(103);
    }
}

void MsgParse::optType109(QJsonObject &object)
{
    if (0 == QString::compare(object.value("boardingGate").toString(), LocalSettings::config->value("Device/bordingGateNo").toString())){
     QJsonObject content = object["content"].toObject();

     m_remove_repository.arrAirportCode=content["arrAirportCode"].toString();
     m_remove_repository.arrAirportName=content["arrAirportName"].toString();
     m_remove_repository.boardingGate=content["boardingGate"].toString();
     m_remove_repository.boardingTime=content["boardingTime"].toString();
     m_remove_repository.depAirportCode=content["depAirportCode"].toString();
     m_remove_repository.depAirportName=content["depAirportName"].toString();
     m_remove_repository.depTimeJ=content["depTimeJ"].toString();
     m_remove_repository.flightDate=content["flightDate"].toString();
     m_remove_repository.kindType=content["kindType"].toInt();
     m_remove_repository.manulOpt=content["manulOpt"].toBool();
     m_remove_repository.midwayCode=content["midwayCode"].toString();
     m_remove_repository.midwayName=content["midwayName"].toString();
     QString tmpStr=content["shareFltno"].toString();
     m_remove_repository.shareFltno=tmpStr.split(",");
     m_remove_repository.status=content["status"].toInt();
     m_remove_repository.threeFlightNo=content["threeFlightNo"].toString();
     m_remove_repository.twoFlightNo=content["twoFlightNo"].toString();
     emit recived_mq_msg(109);
    }
}

void MsgParse::optType300(QJsonObject &object)
{
    if (0 == QString::compare(object.value("boardingGate").toString(), LocalSettings::config->value("Device/bordingGateNo").toString())){
     QJsonObject content = object["content"].toObject();

     m_face_identify_res.basePhoto=content["basePhoto"].toVariant().toByteArray();
     m_face_identify_res.boardingNumber=content["boardingNumber"].toString();
     m_face_identify_res.boardingStatus=content["boardingStatus"].toInt();
     m_face_identify_res.cardNo=content["cardNo"].toString();
     m_face_identify_res.cardType=content["cardType"].toInt();
     m_face_identify_res.endPort=content["endPort"].toString();
     m_face_identify_res.faceImage=content["faceImage"].toVariant().toByteArray();
     m_face_identify_res.fId=content["fId"].toString();
     m_face_identify_res.flightDay=content["flightDay"].toString();
     m_face_identify_res.flightNumber=content["flightNumber"].toString();
     m_face_identify_res.isVipChannel=content["isVipChannel"].toString();
     m_face_identify_res.nameEn=content["nameEn"].toString();
     m_face_identify_res.nameZh=content["nameZh"].toString();
     m_face_identify_res.recognizeTime=content["recognizeTime"].toString();
     m_face_identify_res.reviewInfo=content["reviewInfo"].toString();
     m_face_identify_res.score=content["score"].toVariant().toFloat();
     m_face_identify_res.seatNumber=content["seatNumber"].toString();
     m_face_identify_res.securityInfo=content["securityInfo"].toString();
     m_face_identify_res.sex=content["sex"].toInt();
     m_face_identify_res.sourceType=content["sourceType"].toString();
     m_face_identify_res.startPort=content["startPort"].toString();
     m_face_identify_res.threshold=content["threshold"].toVariant().toFloat();
     m_face_identify_res.transferInfo=content["transferInfo"].toString();
     emit recived_mq_msg(300);
    }
}

void MsgParse::optType301(QJsonObject &object)
{
    if (0 == QString::compare(object.value("boardingGate").toString(), LocalSettings::config->value("Device/bordingGateNo").toString())){
     QJsonObject content = object["content"].toObject();

     m_ppl_num.boardingNum=content["boardingNum"].toInt();
     m_ppl_num.flightDate=content["flightDate"].toString();
     m_ppl_num.flightNo=content["flightNo"].toString();
     emit recived_mq_msg(301);
    }
}

*/
