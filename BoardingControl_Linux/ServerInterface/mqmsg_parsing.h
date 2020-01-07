#ifndef MQMSG_PARSING
#define MQMSG_PARSING

#include "api_param_def.h"
#include "amqpImp.h"
//class AmqpImp;
class MsgParsing : public AmqpImp
{
public:
    MsgParsing();
    ~MsgParsing();

public:
   void ParsingMsg100();
   void ParsingMsg101();
private:
    void initAmqp();
};

#endif // MQMSG_PARSING

