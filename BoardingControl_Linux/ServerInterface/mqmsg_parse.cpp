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
    create_channel("172.18.2.199",  5672,  "/",  "root", "123456", type, "data_terminal_fanoutexchange", callback, userData, "data_terminal_fanoutexchange", "", sec_heartbeat);

}

void CALLBACK MsgParse::msgRecivedCallBack(const char* jsonMsg, void* userData)
{
    qDebug() << "mg_msg: " << QString(jsonMsg);

    MsgParse *this_data = static_cast<MsgParse*>(userData);
    QByteArray byte_JsonMsg(jsonMsg);

    this_data->recive_msg(byte_JsonMsg);
}


void MsgParse::recive_msg(QByteArray &byte_jsonMsg)
{
    QMutexLocker locker(&m_mutex);

    if (byte_jsonMsg != nullptr)
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
            case 302:
                if (jo_jsonMsg["msg"].isArray()) {
                    QJsonArray array;
                    array = jo_jsonMsg["msg"].toArray();
                    optType302(array, array.size());
                } else {
                    qWarning()<<"recived mq msg  Parsed, <optType=302>, but <msg> param error, only  'JsonArry' supported!";
                }
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
}

void MsgParse::optType302(QJsonArray &array, int size)
{
    for (int i = 0; i < size; i++) {
        QJsonObject msg = array.at(i).toObject();

        if (0 == QString::compare(msg.value("boardingGate").toString()
                                  , LocalSettings::config->value("Device/boardingGate").toString())) {


            QJsonArray content = msg["content"].toArray();
            for (int j = 0; j < content.size(); j++) {
                if (0 == QString::compare(content.at(j).toObject().value("flightNumber").toString()
                                          , "AB1250")) {
                    m_depository_new.id = content.at(j).toObject().value("id").toString();
                    m_depository_new.cardNo = content.at(j).toObject().value("cardNo").toString();
                    m_depository_new.passengerName = content.at(j).toObject().value("passengerName").toString();
                    m_depository_new.flightNumber = content.at(j).toObject().value("flightNumber").toString();
                    m_depository_new.boardingNumber = content.at(j).toObject().value("boardingNumber").toString();
                    m_depository_new.seatNumber = content.at(j).toObject().value("seatNumber").toString();
                    m_depository_new.photoPath = content.at(j).toObject().value("photoPath").toString();
                    m_depository_new.boardingStatus = content.at(j).toObject().value("boardingStatus").toInt();
                    m_depository_new.repeatFlag = content.at(j).toObject().value("repeatFlag").toInt();
                    m_depository_new.updateTime = content.at(j).toObject().value("updateTime").toString();
                }
            }
        }
    }

    emit recived_mq_msg(302);
}
