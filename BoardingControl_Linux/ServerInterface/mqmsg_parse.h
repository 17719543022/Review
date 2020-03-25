#ifndef MQMSG_PARSING
#define MQMSG_PARSING

#include "amqpImp.h"
#include "api_param_def.h"

#include <QMutex>
#include <QList>

#define RUN_IN_CALLBACK
#define CALLBACK

static void CALLBACK msgRecivedCallBack(const char* jsonMsg, void* userData);

class MsgParse : public AmqpImp
{
    Q_OBJECT

public:
    explicit MsgParse(QObject* parent = 0);
    virtual ~MsgParse();
    static MsgParse* instance();

private:
    void initAmqp();

    void RUN_IN_CALLBACK recive_msg(QByteArray &byte_jsonMsg);

    /*
    //100-发布航班计划
    void RUN_IN_CALLBACK optType100(QJsonArray &arry, int size);
    //101-建库消息
    void RUN_IN_CALLBACK optType101(QJsonObject &object);
    //102-开始登机消息
    void RUN_IN_CALLBACK optType102(QJsonObject &object);
    //103-结束登机消息
    void RUN_IN_CALLBACK optType103(QJsonObject &object);
    //109-删库消息
    void RUN_IN_CALLBACK optType109(QJsonObject &object);
    //300-识别结果推送
    void RUN_IN_CALLBACK optType300(QJsonObject &object);
    //301-航班的登机人数
    void RUN_IN_CALLBACK optType301(QJsonObject &object);

    */
signals:
    void recived_mq_msg(int type);

public:
    FlightPlan m_flight_plan;       //航班计划
    content101 m_creat_repository;       //建库消息
    content102 m_bording_start;       //开始登机消息
    content103 m_boarding_end;       //结束登机消息
    content109 m_remove_repository;       //删库消息
    content300 m_face_identify_res;       //识别结果推送
    content301 m_ppl_num;      //航班的登机人数

private:
    QMutex m_mutex;
    QMetaObject::Connection ConnectionoptType;
    QJsonObject jsonObject302;
};

inline MsgParse* MsgParse::instance()
{
    static MsgParse hcSer;
    return &hcSer;
}

#endif // MQMSG_PARSING

