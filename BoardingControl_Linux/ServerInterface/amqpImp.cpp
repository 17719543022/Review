﻿#include "amqpImp.h"
#include "rabbitmq_c/amqp_time.h"

#include <amqp_tcp_socket.h>
#include <QDebug>

#define TIME_OUT 6 //s

AmqpImp::AmqpImp(bool has_exchange):
    _has_exchange(has_exchange)
{
    _exchange.id = (char*)malloc(128*sizeof(char));
    _exchange.type = (char*)malloc(128*sizeof(char));
    strcpy(_exchange.id, NOTIFY_EXCHANGE_NAME);
    strcpy(_exchange.type, "fanout");
}

AmqpImp::~AmqpImp(void)
{
    close_channel();

    if (_exchange.id != nullptr){
       free(_exchange.id);
       _exchange.id = nullptr;
    }
    if (_exchange.type != nullptr){
        free(_exchange.type);
        _exchange.type = nullptr;
    }
}

//创建通道
int AmqpImp::create_channel(const char* ip, int port, const char* vhost, const char* user_name, const char* password, ChannelType type,
                            const char* areaId /*= nullptr*/, const char* deviceId /*= nullptr*/, consumer_msg_cb cb /*= nullptr*/, void* userData /*= nullptr*/)
{
    setup_connection(ip, port, vhost, user_name, password);

    //打开通道
    amqp_channel_open(_conn_state, _channel);
    die_on_amqp_error(amqp_get_rpc_reply(_conn_state), "ERROR: [AmqpImp] Opening channel");

    if(_has_exchange){
        //passive 检测exchange是否存在，设为true，若队列存在则命令成功返回（调用其他参数不会影响exchange属性），若不存在不会创建exchange，返回错误。
        //设为false，如果exchange不存在则创建exchange，调用成功返回。如果exchange已经存在，并且匹配现在exchange的话则成功返回，如果不匹配则exchange声明失败。
        amqp_exchange_declare(_conn_state, _channel, amqp_cstring_bytes(_exchange.id), amqp_cstring_bytes(_exchange.type),
                              _exchange.passive, _exchange.durable, _exchange.auto_delete, _exchange.internal, _exchange.arguments);
        die_on_amqp_error(amqp_get_rpc_reply(_conn_state), "ERROR: [AmqpImp] declare exchange");
    }

    //消费
    if(type == CHANNEL_CONSUMER)
    {
        std::string str_MsgQueueName = areaId;
        str_MsgQueueName.append("_");
        str_MsgQueueName.append(deviceId);

        //创建消息接收队列绑定
        queue_declare_and_bind(str_MsgQueueName.c_str(), NOTIFY_EXCHANGE_NAME, "", 0, 0);

        //声明消费
        amqp_basic_consume(_conn_state, _channel, amqp_cstring_bytes(str_MsgQueueName.c_str()),
                           amqp_empty_bytes,
                           0,
                           1,
                           0,
                           amqp_empty_table);
        die_on_amqp_error(amqp_get_rpc_reply(_conn_state), "ERROR: [AmqpImp] Consuming");

        _msg_cb = cb;
        _userData = userData;
        _consum_loop = true;
        _consume_thread = std::thread(&AmqpImp::consume_message, this);
    }

    return 0;
}

//关闭通道
void AmqpImp::close_channel()
{
    if(_consume_thread.joinable())
    {
        _consum_loop = false;
        _consume_thread.join();
    }

    if(_conn_state != nullptr)
    {
        amqp_rpc_reply_t rpc_reply = amqp_channel_close(_conn_state, _channel, AMQP_REPLY_SUCCESS);
        die_on_amqp_error(rpc_reply, "ERROR: [AmqpImp] Closing channel");

        close_and_destory_connection(_conn_state);
        _conn_state = nullptr;
    }
}

//设置交换机参数
void AmqpImp::set_exchange(const char *exchange_id, const char *exchange_type, amqp_boolean_t passive, amqp_boolean_t durable, amqp_boolean_t auto_delete, amqp_boolean_t internal, amqp_table_t arguments)
{
    try
    {
        strcpy(_exchange.id, exchange_id);
        strcpy(_exchange.type, exchange_type);
        _exchange.passive = passive;
        _exchange.durable = durable;
        _exchange.auto_delete = auto_delete;
        _exchange.internal = internal;
        _exchange.arguments = arguments;
    }
    catch(std::exception &ex)
    {
        qCritical()<<ex.what()<<QString("设置交换机参数失败, id或type最大128字节");
        exit(1);
    }
}

//发送消息public
int AmqpImp::publish_message(const char* message, const char* exchange, const char* rout_key)
{
    return basic_publish(message, exchange, rout_key);
}

//获取信息public
char* AmqpImp::get_message(const char* queue_name_, uint64_t* out_body_size)
{
    return basic_get(queue_name_, out_body_size);
}

//设置连接和通道,登录以及设置心跳参数
int AmqpImp::setup_connection(const char* ip, int port, const char* vhost, const char* user_name, const char* password)
{
    amqp_connection_state_t connect_state_ = nullptr;
    amqp_socket_t* socket = nullptr;

    connect_state_ = amqp_new_connection();
    if(connect_state_ == nullptr)
    {
        die("ERROR: [AmqpImp] new connection");
    }

    socket = amqp_tcp_socket_new(connect_state_);
    if(socket == nullptr)
    {
        die("ERROR: [AmqpImp] creating TCP socket");
    }

    int ret = amqp_socket_open(socket, ip, port);
    if( ret != AMQP_STATUS_OK)
    {
        die("ERROR: [AmqpImp] opening rabbitMq socket");
    }

    amqp_rpc_reply_t rpc_reply = amqp_login(connect_state_, vhost, 1, AMQP_DEFAULT_FRAME_SIZE, 60, AMQP_SASL_METHOD_PLAIN, user_name, password);
    die_on_amqp_error(rpc_reply, "ERROR: [AmqpImp] login in");

    _conn_state = connect_state_;
    return 0;
}

//关闭销毁连接
int AmqpImp::close_and_destory_connection(amqp_connection_state_t conn_state_)
{
    if(conn_state_ != nullptr)
    {
        amqp_rpc_reply_t rpc_reply = amqp_connection_close(conn_state_, AMQP_REPLY_SUCCESS);
        die_on_amqp_error(rpc_reply, "ERROR: [AmqpImp] Closing connection");

        int ret = amqp_destroy_connection(conn_state_);
        die_on_error(ret, "ERROR: [AmqpImp] Ending connection");
    }
    return 0;
}

//创建消息接收队列绑定
int AmqpImp::queue_declare_and_bind(const char* queue_name, const char* exchange_name, const char* routingKey, int ttl, int msglen)
{
    amqp_table_entry_t inner_entries[2];//额外参数
    amqp_table_t inner_table;
    inner_table.num_entries = 0;
    inner_table.entries = nullptr;
    if(ttl > 0)
    {
        inner_entries[0].key = amqp_cstring_bytes("x-message-ttl");
        inner_entries[0].value.kind = AMQP_FIELD_KIND_I32;
        inner_entries[0].value.value.i32 = ttl;
        inner_table.num_entries++;
    }

    if(msglen > 0)
    {
        inner_entries[1].key = amqp_cstring_bytes("x-max-length");
        inner_entries[1].value.kind = AMQP_FIELD_KIND_I32;
        inner_entries[1].value.value.i32 = msglen;
        inner_table.num_entries++;
    }

    if(inner_table.num_entries > 0)
    {
        inner_table.entries = inner_entries;
    }

    //autoDelete: 设置是否自动删除。为true 则设置队列为自动删除。自动删除的前提是:至少有一个消费者连接到这个队列，之后所有与这个队列连接的消费者都断开时，才会自动删除。
    //不能把这个参数错误地理解为:当连接到此队列的所有客户端断开时，这个队列自动删除"，因为生产者客户端创建这个队列，或者没有消费者客户端与这个队列连接时，都不会自动删除这个队列。
    amqp_queue_declare(_conn_state, _channel, amqp_cstring_bytes(queue_name),
                       0, //passive
                       0, //durable
                       0, //exclusive
                       1, //auto delete
                       inner_table);
    die_on_amqp_error(amqp_get_rpc_reply(_conn_state), "ERROR: [AmqpImp] Declaring queue");


    amqp_queue_bind(_conn_state, _channel, amqp_cstring_bytes(queue_name), amqp_cstring_bytes(exchange_name),
                    amqp_cstring_bytes(routingKey), amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(_conn_state), "ERROR: [AmqpImp] Binding queue");

    return 0;
}

//获取信息
char* AmqpImp::basic_get(const char* queue_name_, uint64_t* out_body_size)
{
    if(_conn_state == nullptr)
    {
        return nullptr;
    }

    amqp_rpc_reply_t rpc_reply;
    amqp_time_t deadline;
    struct timeval timeout = {TIME_OUT, 0};
    int time_rc = amqp_time_from_now(&deadline, &timeout);
    if(time_rc != AMQP_STATUS_OK)
    {
        return nullptr;
    }

    do
    {
        rpc_reply = amqp_basic_get(_conn_state, _channel, amqp_cstring_bytes(queue_name_), 1);

    }while (rpc_reply.reply_type == AMQP_RESPONSE_NORMAL &&
            rpc_reply.reply.id == AMQP_BASIC_GET_EMPTY_METHOD &&
            amqp_time_has_past(deadline) == AMQP_STATUS_OK);

    if(rpc_reply.reply_type != AMQP_RESPONSE_NORMAL || rpc_reply.reply.id != AMQP_BASIC_GET_OK_METHOD)
    {
        return nullptr;
    }

    amqp_message_t message;
    rpc_reply = amqp_read_message(_conn_state, _channel, &message, 0);
    if(rpc_reply.reply_type != AMQP_RESPONSE_NORMAL)
    {
        //...
        amqp_destroy_message(&message);
        return nullptr;
    }

    char* body = static_cast<char*>(malloc(message.body.len + 1));
    memset(body, 0, message.body.len + 1);
    memcpy(body, message.body.bytes, message.body.len);
    *out_body_size = message.body.len;
    amqp_destroy_message(&message);

    return body;
}

//发布消息
int AmqpImp::basic_publish(const char* message, const char* exchange, const char* rout_key)
{
    amqp_bytes_t message_bytes = amqp_cstring_bytes(message);
    amqp_basic_properties_t properties;
    properties._flags = 0;
    properties._flags |= AMQP_BASIC_DELIVERY_MODE_FLAG;
    properties.delivery_mode = AMQP_DELIVERY_NONPERSISTENT;

    int retval = amqp_basic_publish(_conn_state, _channel, amqp_cstring_bytes(exchange),
                                    amqp_cstring_bytes(rout_key),
                                    /* mandatory=*/1,
                                    /* immediate=*/0, /* RabbitMQ 3.x does not support the "immediate" flag according to https://www.rabbitmq.com/specification.html */
                                    &properties, message_bytes);

    return retval;
}

//消费消息
void AmqpImp::consume_message()
{
    if(_conn_state == nullptr)
    {
        return ;
    }

    amqp_frame_t frame;
    while (_consum_loop)
    {
        amqp_envelope_t envelope;
        amqp_rpc_reply_t rpc_reply;
        struct timeval timeout = {TIME_OUT, 0};

        amqp_maybe_release_buffers(_conn_state);
        rpc_reply = amqp_consume_message(_conn_state, &envelope, &timeout, 0);
        if(rpc_reply.reply_type != AMQP_RESPONSE_NORMAL)
        {
            if (AMQP_RESPONSE_LIBRARY_EXCEPTION == rpc_reply.reply_type && AMQP_STATUS_UNEXPECTED_STATE == rpc_reply.library_error)
            {
                if (AMQP_STATUS_OK != amqp_simple_wait_frame(_conn_state, &frame))
                {
                    qCritical() << "ERROR: [AmqpImp::consume_message] wait frame error thread exit";
                    return;
                }

                if (AMQP_FRAME_METHOD == frame.frame_type)
                {
                    switch (frame.payload.method.id)
                    {
                    case AMQP_BASIC_ACK_METHOD:
                        /* if we've turned publisher confirms on, and we've published a
                           * message here is a message being confirmed.
                           */
                        break;
                    case AMQP_BASIC_RETURN_METHOD:
                        /* if a published message couldn't be routed and the mandatory
                           * flag was set this is what would be returned. The message then
                           * needs to be read.
                           */
                    {
                        amqp_message_t message;
                        rpc_reply = amqp_read_message(_conn_state, frame.channel, &message, 0);
                        if (AMQP_RESPONSE_NORMAL != rpc_reply.reply_type)
                        {
                            qCritical() << "ERROR: [AmqpImp::consume_message] read message error thread exit";
                            return;
                        }

                        amqp_destroy_message(&message);
                    }
                        break;

                    case AMQP_CHANNEL_CLOSE_METHOD:
                        /* a channel.close method happens when a channel exception occurs,
                           * this can happen by publishing to an exchange that doesn't exist
                           * for example.
                           *
                           * In this case you would need to open another channel redeclare
                           * any queues that were declared auto-delete, and restart any
                           * consumers that were attached to the previous channel.
                           */
                        qCritical() << "ERROR: [AmqpImp::consume_message] consume message error AMQP_CHANNEL_CLOSE_METHOD";
                        return;

                    case AMQP_CONNECTION_CLOSE_METHOD:
                        /* a connection.close method happens when a connection exception
                           * occurs, this can happen by trying to use a channel that isn't
                           * open for example.
                           *
                           * In this case the whole connection must be restarted.
                           */
                        qCritical() << "ERROR: [AmqpImp::consume_message] consume message error AMQP_CONNECTION_CLOSE_METHOD";
                        return;

                    default:
                        qCritical() << "ERROR: [AmqpImp::consume_message] consume message An unexpected method was received" << frame.payload.method.id;
                        return;
                    }
                }
            }
        }
        else
        {
            size_t out_body_size =  envelope.message.body.len;
            if(out_body_size > 0)
            {
                char* body = static_cast<char*>(malloc(out_body_size + 1));
                memset(body, 0, out_body_size + 1);
                memcpy(body, envelope.message.body.bytes, out_body_size);
                amqp_destroy_envelope(&envelope);
                if(_msg_cb)
                {
                    _msg_cb(body, _userData);
                }
                free(body);
            }
        }
    }
}

//log error
void AmqpImp::die(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  exit(1);
}

void AmqpImp::die_on_error(int x, char const *context) {
  if (x < 0) {
      qCritical()<<QString("%1: %2").arg(context).arg(amqp_error_string2(x));
    exit(1);
  }
}

void AmqpImp::die_on_amqp_error(amqp_rpc_reply_t x, char const *context) {
  switch (x.reply_type) {
    case AMQP_RESPONSE_NORMAL:
      return;

    case AMQP_RESPONSE_NONE:
      qCritical()<<QString("%1: missing RPC reply type!").arg(context);
      break;

    case AMQP_RESPONSE_LIBRARY_EXCEPTION:
      qCritical()<<QString("%1: %2").arg(context).arg(amqp_error_string2(x.library_error));
      break;

    case AMQP_RESPONSE_SERVER_EXCEPTION:
      switch (x.reply.id) {
        case AMQP_CONNECTION_CLOSE_METHOD: {
          amqp_connection_close_t *m =
              (amqp_connection_close_t *)x.reply.decoded;
          qCritical()<<QString("%1: server connection error %2, message: %3").arg(context).arg(m->reply_code).arg(QString((char*)m->reply_text.bytes).left((int)m->reply_text.len));
          break;
        }
        case AMQP_CHANNEL_CLOSE_METHOD: {
          amqp_channel_close_t *m = (amqp_channel_close_t *)x.reply.decoded;
          qCritical()<<QString("%1: server channel error %2, message: %3").arg(context).arg(m->reply_code).arg(QString((char*)m->reply_text.bytes).left((int)m->reply_text.len));
          break;
        }
        default:
          qCritical()<<QString("%1: unknown server error, method id 0x%2").arg(context).arg(QString::number(x.reply.id, 16));
          break;
      }
      break;
  }

  exit(1);
}
