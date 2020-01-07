#ifndef AMQPIMP_H
#define AMQPIMP_H

#include "amqp.h"
#include <thread>
#include <memory>

#define NOTIFY_EXCHANGE_NAME "MessageNotify.fanout"
#define DETECTRECOGNIZE_EXCHANGE_NAME "DetectRecognize.direct"
#define DETECT_ROUTING_KEY "detect"
#define RECOGNIZE_ROUTING_KEY "recognize"
typedef void (*consumer_msg_cb)(const char* msg, void* userData);

typedef enum _ChannelType
{
    CHANNEL_PRODUCER = 0,
    CHANNEL_CONSUMER,

}ChannelType;

typedef struct
{
    char* id{nullptr};
    char* type{nullptr};       //交换机类型  fanout 广播 direct直接绑定 topic主题模糊匹配
    amqp_boolean_t passive{0};       // 如果为1, 但是交换机之前并不存在, 就会返回失败
    amqp_boolean_t durable{0};      //1持久化 0非持久化
    amqp_boolean_t auto_delete{0};     //1没有exchange绑定后删除队列 0不删除
    amqp_boolean_t internal{0};        // 0可以直接使用 1不能直接使用，只能绑定到其他exchange使用
    amqp_table_t arguments{amqp_empty_table};     // 扩展参数
}ExchangeProperty;

class AmqpImp
{
public:
    AmqpImp(bool has_exchange = false);

    ~AmqpImp(void);

    //创建通道 /*areaId_deviceId 构成 queueName*/
    int create_channel(const char* ip, int port, const char* vhost, const char* user_name, const char* password, ChannelType type,
                       const char* areaId = nullptr, const char* deviceId = nullptr, consumer_msg_cb cb = nullptr, void* userData = nullptr);
    //关闭通道
    void close_channel();
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
    //创建连接
    int setup_connection(const char* ip, int port, const char* vhost, const char* user_name, const char* password);
    //销毁连接
    int close_and_destory_connection(amqp_connection_state_t connect_state_);
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

    bool _has_exchange;
    ExchangeProperty _exchange;

    consumer_msg_cb _msg_cb{nullptr};
    void* _userData{nullptr};
    bool  _consum_loop{false};
	std::thread _consume_thread;
};

typedef std::shared_ptr<AmqpImp> AmqpImpPtr;

#endif //AMQPIMP_H
