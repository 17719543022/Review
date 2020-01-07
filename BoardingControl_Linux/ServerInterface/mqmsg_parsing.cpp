#include "mqmsg_parsing.h"
#include "settings.h"

MsgParsing::MsgParsing()
{

}

MsgParsing::~MsgParsing()
{

}

void MsgParsing::initAmqp()
{
    const char* ip=LocalSettings::config->value("amqpServer/i").toByteArray().data();
    int port=LocalSettings::config->value("amqpServer/port").toInt();
    const char* vhost=LocalSettings::config->value("amqpServer/vhost").toByteArray().data();
    const char* user_name=LocalSettings::config->value("amqpServer/user_name").toByteArray().data();
    const char* password=LocalSettings::config->value("amqpServer/password").toByteArray().data();
    ChannelType type=CHANNEL_CONSUMER;
//    consumer_msg_cb cb = nullptr;
//    void* userData = nullptr;

    //创建通道
    create_channel(ip,  port,  vhost,  user_name, password, type);
}
