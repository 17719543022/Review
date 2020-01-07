#ifndef HTTPAPI_H
#define HTTPAPI_H

#include "ServerInterface/api_param_def.h"
#include <QObject>

class HttpAPI : public QObject
{
    Q_OBJECT
public:
    explicit HttpAPI(QObject *parent = nullptr);
    virtual ~HttpAPI(void);
    static HttpAPI* instance();

public:
    Return_Flight_Plan_Param getFlightPlan(const Request_Flight_Plan_Param& );
    FlightReviewResponse get(const FlightReviewRequest& request);
    FlowReviewResponse get(const FlowReviewRequest& request);

private:
    int Init();
    int UnInit();

private:
    QString caPath;
    QString apiId;
    QString apiKey;
    QString faceIpAddr;
    QString faceApiAddr;

    QString flowReviewServer;
    QString flowReviewUrl;
    QString flightReviewServer;
    QString flightReviewUrl;
};

inline HttpAPI* HttpAPI::instance()
{
    static HttpAPI hcSer;
    return &hcSer;
}

#endif // HTTPAPI_H
