#ifndef AMQPIMP_H
#define AMQPIMP_H

#include "amqp.h"
#include <thread>
#include <memory>
#include <QObject>

#define DEFAULT_EXCHANGE_ID "boarding_ctrl.fanout"
#define DEFAULT_BINDING_KEY "boarding_ctrl"

#ifdef _WIN32
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif

typedef void CALLBACK ( *consumer_msg_cb)(const char* msg, void* userData);

typedef enum _ChannelType
{
    CHANNEL_PRODUCER = 0,
    CHANNEL_CONSUMER,

}ChannelType;

class AmqpImp : public QObject
{
    Q_OBJECT
public:
    explicit AmqpImp(QObject* parent = 0);
    virtual ~AmqpImp(void);

    //创建通道 /*areaId_deviceId 构成 queueName*/
    int create_channel(const char* ip, int port, const char* vhost, const char* user_name, const char* password, ChannelType type, const char* queue_name = nullptr,
                       consumer_msg_cb cb = nullptr, void* userData = nullptr, const char* exchange_id = nullptr, const char* bingding_key = nullptr, int sec_heartbeat = 0);
    //关闭通道
    int close_channel();
    //设置交换机参数,如果具有交换机，则需要在创建队列之前调用此方法，否则采用默认参数可能出错
    void set_exchange(const char* exchange_id,const char* exchange_type, amqp_boolean_t passive, amqp_boolean_t durable, amqp_boolean_t auto_delete, amqp_boolean_t internal, amqp_table_t arguments);
    //发送消息public
    int publish_message(const char* message, const char* exchange, const char* rout_key);
    //获取信息public
    char* get_message(const char* queue_name_, uint64_t* out_body_size);

private:
    //log error
    static void die(const char *fmt, ...);
    static void die_on_error(int x, char const *context);
    static void die_on_amqp_error(amqp_rpc_reply_t x, char const *context);
    //创建连接, 设置连接和通道,登录以及设置心跳参数
    int setup_connection(const char* ip, int port, const char* vhost, const char* user_name, const char* password, int sec_heartbeat = 0);
    //关闭销毁连接
    int close_and_destory_connection();
    //创建消息接收队列绑定
    int queue_declare_and_bind(const char* queue_name, const char* exchange_name, const char* routingKey, int ttl, int msglen);
	//获取信息
	char* basic_get(const char* queue_name, uint64_t* out_body_size);
	//发布消息
	int  basic_publish(const char* message, const char* exchange, const char* rout_key);
	//消费消息
	void consume_message();

private:
    amqp_connection_state_t _conn_state{nullptr};
    int _channel{1};
    consumer_msg_cb _msg_cb{nullptr};
    void* _userData{nullptr};
    bool  _consum_loop{false};
	std::thread _consume_thread;
};

typedef std::shared_ptr<AmqpImp> AmqpImpPtr;

#endif //AMQPIMP_H
