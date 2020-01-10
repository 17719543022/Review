#ifndef API_PARAM_DEF_H
#define API_PARAM_DEF_H

#include <QString>
#include <QStringList>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QtDebug>

typedef struct{
    QString     channelCode{"null"};        //区域编码
    QString     deviceId{"null"};           //设备ID
}DeviceInfo;

/*2.3.17 调度系统拉取航班计划 paramList*******************************************************************/
typedef struct                                                                                                                                                                                         /**/
{                                                                                                                                                                                                                /**/
    QString     reqId{"null"};                  //32位UUID                                                                                                                               /**/
    QString     gateNo{"null"};                 //登机口编号                                                                                                                           /**/
}Request_Flight_Plan_Param;                                                                                                                                                                 /**/
                                                                                                                                                                                                                  /**/
typedef struct{                                                                                                                                                                                         /**/
    QString threeFlightNo{"null"};        //非必须    航班3字码                                                                                                                /**/
    QString twoFlightNo{"null"};      //非必须    航班2字码                                                                                                                     /**/
    QString depTimeJ{"null"};         //非必须    预计起飞时间                                                                                                                 /**/
    QString flightDate{"null"};       //非必须    航班日期yyyy-MM-dd                                                                                                     /**/
    int status{-1};                  //非必须    航班状态  0-等待，1-建库，2-开始登机，3-结束登机，4-起飞                                            /**/
    QString midwayCode{"null"};       //非必须    中转机场3字码                                                                                                           /**/
    QString midwayName{"null"};       //非必须    中转机场名称                                                                                                            /**/
    QString arrAirportCode{"null"};	    //非必须    到达机场3字码                                                                                                        /**/
    QString arrAirportName{"null"};	    //非必须    到达机场名                                                                                                              /**/
    QString depAirportCode{"null"};	    //非必须    起飞机场3字码                                                                                                        /**/
    QString depAirportName{"null"};	//非必须    起飞机场名称                                                                                                           /**/
    int  kindType{-1};       //非必须    航班类型  0-原始，1-增加航班，2-删除航班，3-备降航班，4-二次返航航班                            /**/
    bool manulOpt{false};      //非必须    true-人工拉取的航班，false-非人工拉取的航班                                                                   /**/
    QString boardingGate{"null"};     //非必须    登机口                                                                                                                          /**/
}FlightPlan;                                                                                                                                                                                                 /**/
                                                                                                                                                                                                                    /**/
typedef struct {                                                                                                                                                                                          /**/
    int curl_err_code{-1};     //调用服务器接口返回的URL错误码                                                                                                            /**/
    int serverStatus{-1};           //0表示成功，具体见后面的详细的错误码                                                                                             /**/
    QString reqId{"null"};              //返回客户端传递的reqId                                                                                                                   /**/
    QString msg{"null"};                //对当前结果的描述                                                                                                                             /**/
    FlightPlan flightPlan;  //航班计划                                                                                                                                                          /**/
}Return_Flight_Plan_Param;                                                                                                                                                                       /**/
/*******************************************************************************************************/



typedef struct
{
    int optType;        //100-发布航班计划, 101-建库消息, 102-开始登机消息, 103-结束登机消息, 109-删库消息, 300-识别结果推送, 301-航班的登机人数
    QJsonObject msg;
}MqQueueMsg_Style;

/* 2.3.13登机口建库、已登机、未登机旅客查询接口**************************************************************/

struct DataInfo {
    int boardingNum;
    int faceNums;
    int manualNum;
    int midwayNum;
    int orgDepNum;
    int recognizeNum;
    int transferNum;

    DataInfo() {
        boardingNum = -1;
        faceNums = -1;
        manualNum = -1;
        midwayNum = -1;
        orgDepNum = -1;
        recognizeNum = -1;
        transferNum = -1;
    }
};

struct FlightReviewResult {
    QString boardingNumber;
    int boardingStatus;
    QString cardNo;
    QString flightNumber;
    QString id;
    QString passengerName;
    QString photoPath;
    int repeatFlag;
    QString seatNumber;

    FlightReviewResult() {
        boardingNumber = QString();
        boardingStatus = -1;
        cardNo = QString();
        flightNumber = QString();
        id = QString();
        passengerName = QString();
        photoPath = QString();
        repeatFlag = -1;
        seatNumber = QString();
    }
};

struct FlightReviewInterface {
    int status;
    QString reqId;
    QString msg;
    int num;
    int total;
    DataInfo dataInfo;
    int validSize;
    FlightReviewResult results[1000];

    FlightReviewInterface() {
        status = -1;
        reqId = QString();
        msg = QString();
        num = -1;
        total = -1;
        validSize = -1;
    }

    void update(QJsonDocument &doc) {
        QJsonObject docObj = doc.object();

        if (docObj.contains("status")) {
            status = docObj.value("status").toInt();
        }
        if (docObj.contains("reqId")) {
            reqId = docObj.value("reqId").toString();
        }
        if (docObj.contains("msg")) {
            msg = docObj.value("msg").toString();
        }
        if (docObj.contains("num")) {
            num = docObj.value("num").toInt();
        }
        if (docObj.contains("total")) {
            total = docObj.value("total").toInt();
        }
        if (docObj.contains("dataInfo")) {
            QJsonValue dataInfoJson = docObj.value("dataInfo");
            if (!dataInfoJson.isNull() && dataInfoJson.isObject()) {
                QJsonObject dataInfoObj = dataInfoJson.toObject();
                dataInfo.boardingNum = dataInfoObj.value("boardingNum").toInt();
                dataInfo.faceNums = dataInfoObj.value("faceNums").toInt();
                dataInfo.manualNum = dataInfoObj.value("manualNum").toInt();
                dataInfo.midwayNum = dataInfoObj.value("midwayNum").toInt();
                dataInfo.orgDepNum = dataInfoObj.value("orgDepNum").toInt();
                dataInfo.recognizeNum = dataInfoObj.value("recognizeNum").toInt();
                dataInfo.transferNum = dataInfoObj.value("transferNum").toInt();
            }
        }
        if (docObj.contains("results")) {
            QJsonValue resultsJson = docObj.value("results");
            if (!resultsJson.isNull() && resultsJson.isArray()) {
                QJsonArray array = resultsJson.toArray();
                validSize = array.size();
                for (int i = 0; i < array.size() && i < 1000; i++) {
                    results[i].boardingNumber = array.at(i).toObject().value("boardingNumber").toString();
                    results[i].boardingStatus = array.at(i).toObject().value("boardingStatus").toInt();
                    results[i].cardNo = array.at(i).toObject().value("cardNo").toString();
                    results[i].flightNumber = array.at(i).toObject().value("flightNumber").toString();
                    results[i].id = array.at(i).toObject().value("id").toString();
                    results[i].passengerName = array.at(i).toObject().value("passengerName").toString();
                    results[i].photoPath = array.at(i).toObject().value("photoPath").toString();
                    results[i].repeatFlag = array.at(i).toObject().value("repeatFlag").toInt();
                    results[i].seatNumber = array.at(i).toObject().value("seatNumber").toString();
                }
            }
        }
    }
};

struct FlightReviewRequest {
    int queryType;
    QString input;
};

struct FlightReviewResponse {
    QJsonDocument doc;
    int errorCode;
    bool founded;
    FlightReviewInterface interface;

    void dump() {
        qDebug() << doc;
    }

    void update(QJsonDocument &doc) {
        doc = doc;

        QJsonObject docObj = doc.object();

        QJsonValue dataInfoJson, resultsJson;
        if (docObj.contains("dataInfo")) {
            dataInfoJson = docObj.value("dataInfo");
        }

        if (docObj.contains("results")) {
            resultsJson = docObj.value("results");
        }
        if (dataInfoJson == QJsonValue() || resultsJson == QJsonValue()) {
            founded = false;
        }

        interface.update(doc);
    }

    FlightReviewResponse() {
        doc = QJsonDocument();
        errorCode = -1;
        founded = true;
    }
};

/******************************************************************************************************/

/* 2.3.24 人员回查-安检、登机口接口************************************************************************/

struct BoardingInfo {
    int passType;
    int passStatus;
    QString passTime;
    QString gateNo;
    QString deviceId;
    QString channelName;
    QString photoPath;

    BoardingInfo() {
        passType = -1;
        passStatus = -1;
        passTime = QString();
        gateNo = QString();
        deviceId = QString();
        channelName = QString();
        photoPath = QString();
    }
};

struct ReviewInfo {
    int passType;
    int passStatus;
    QString passTime;
    QString gateNo;
    QString deviceId;
    QString channelName;
    QString photoPath;

    ReviewInfo() {
        passType = -1;
        passStatus = -1;
        passTime = QString();
        gateNo = QString();
        deviceId = QString();
        channelName = QString();
        photoPath = QString();
    }
};

struct SecurityInfo {
    int passType;
    int passStatus;
    QString passTime;
    QString gateNo;
    QString deviceId;
    QString channelName;
    QString photoPath;

    SecurityInfo() {
        passType = -1;
        passStatus = -1;
        passTime = QString();
        gateNo = QString();
        deviceId = QString();
        channelName = QString();
        photoPath = QString();
    }
};

struct TransferInfo {
    int sourceType;
    QString photoPath;
    QString collectTime;
    QString gateNo;
    QString deviceId;
    QString channelName;

    TransferInfo() {
        sourceType = -1;
        photoPath = QString();
        collectTime = QString();
        gateNo = QString();
        deviceId = QString();
        channelName = QString();
    }
};

struct UserInfo {
    QString certificateNumber;
    QString passengerName;
    QString passengerEnglishName;
    QString flightNo;
    QString boardingNumber;
    QString flightDay;
    QString seatId;
    int isFocus;

    UserInfo() {
        certificateNumber = QString();
        passengerName = QString();
        passengerEnglishName = QString();
        flightNo = QString();
        boardingNumber = QString();
        flightDay = QString();
        seatId = QString();
        isFocus = -1;
    }
};

struct FlowReviewResult {
    bool hasUserInfo;
    UserInfo userInfo;
    bool hasSecurityInfo;
    SecurityInfo securityInfo;
    bool hasReviewInfo;
    ReviewInfo reviewInfo;
    bool hasTransferInfo;
    TransferInfo transferInfo;
    bool hasBoardingInfo;
    BoardingInfo boardingInfo;

    FlowReviewResult() {
        hasUserInfo = false;
        hasSecurityInfo = false;
        hasReviewInfo = false;
        hasTransferInfo = false;
        hasBoardingInfo = false;
    }
};

struct FlowReviewInterface {
    int status;
    QString reqId;
    QString msg;
    FlowReviewResult result[10];
    int isBaby;

    FlowReviewInterface() {
        status = -1;
        reqId = QString();
        msg = QString();
        isBaby = -1;
    }

    void update(QJsonDocument &doc) {
        QJsonObject docObj = doc.object();

        if (docObj.contains("status")) {
            status = docObj.value("status").toInt();
        }
        if (docObj.contains("reqId")) {
            reqId = docObj.value("reqId").toString();
        }
        if (docObj.contains("msg")) {
            msg = docObj.value("msg").toString();
        }
        if (docObj.contains("result")) {
            QJsonValue resultObj = docObj.value("result");
            if (!resultObj.isNull() && resultObj.isArray()) {
                QJsonArray array = resultObj.toArray();
                for (int i = 0; i < array.size() && i < 10; i++) {
                    QJsonValue userInfo = array.at(i).toObject().value("userInfo");
                    if (!userInfo.isNull()) {
                        result[i].hasUserInfo = true;
                        result[i].userInfo.certificateNumber = userInfo.toObject().value("certificateNumber").toString();
                        result[i].userInfo.passengerName = userInfo.toObject().value("passengerName").toString();
                        result[i].userInfo.passengerEnglishName = userInfo.toObject().value("passengerEnglishName").toString();
                        result[i].userInfo.flightNo = userInfo.toObject().value("flightNo").toString();
                        result[i].userInfo.boardingNumber = userInfo.toObject().value("boardingNumber").toString();
                        result[i].userInfo.flightDay = userInfo.toObject().value("flightDay").toString();
                        result[i].userInfo.seatId = userInfo.toObject().value("seatId").toString();
                        result[i].userInfo.isFocus = userInfo.toObject().value("isFocus").toInt();
                    } else {
                        result[i].hasUserInfo = false;
                    }
                    QJsonValue securityInfo = array.at(i).toObject().value("securityInfo");
                    if (!securityInfo.isNull()) {
                        result[i].hasSecurityInfo = true;
                        result[i].securityInfo.passType = securityInfo.toObject().value("passType").toInt();
                        result[i].securityInfo.passStatus = securityInfo.toObject().value("passStatus").toInt();
                        result[i].securityInfo.passTime = securityInfo.toObject().value("passTime").toString();
                        result[i].securityInfo.gateNo = securityInfo.toObject().value("gateNo").toString();
                        result[i].securityInfo.deviceId = securityInfo.toObject().value("deviceId").toString();
                        result[i].securityInfo.channelName = securityInfo.toObject().value("channelName").toString();
                        result[i].securityInfo.photoPath = securityInfo.toObject().value("photoPath").toString();
                    } else {
                        result[i].hasSecurityInfo = false;
                    }
                    QJsonValue reviewInfo = array.at(i).toObject().value("reviewInfo");
                    if (!reviewInfo.isNull()) {
                        result[i].hasReviewInfo = true;
                        result[i].reviewInfo.passType = reviewInfo.toObject().value("passType").toInt();
                        result[i].reviewInfo.passStatus = reviewInfo.toObject().value("passStatus").toInt();
                        result[i].reviewInfo.passTime = reviewInfo.toObject().value("passTime").toString();
                        result[i].reviewInfo.gateNo = reviewInfo.toObject().value("gateNo").toString();
                        result[i].reviewInfo.deviceId = reviewInfo.toObject().value("deviceId").toString();
                        result[i].reviewInfo.channelName = reviewInfo.toObject().value("channelName").toString();
                        result[i].reviewInfo.photoPath = reviewInfo.toObject().value("photoPath").toString();
                    } else {
                        result[i].hasReviewInfo = false;
                    }
                    QJsonValue transferInfo = array.at(i).toObject().value("transferInfo");
                    if (!transferInfo.isNull()) {
                        result[i].hasTransferInfo = true;
                        result[i].transferInfo.sourceType = transferInfo.toObject().value("sourceType").toInt();
                        result[i].transferInfo.photoPath = transferInfo.toObject().value("photoPath").toString();
                        result[i].transferInfo.collectTime = transferInfo.toObject().value("collectTime").toString();
                        result[i].transferInfo.gateNo = transferInfo.toObject().value("gateNo").toString();
                        result[i].transferInfo.deviceId = transferInfo.toObject().value("deviceId").toString();
                        result[i].transferInfo.channelName = transferInfo.toObject().value("channelName").toString();
                    } else {
                        result[i].hasTransferInfo = false;
                    }
                    QJsonValue boardingInfo = array.at(i).toObject().value("boardingInfo");
                    if (!boardingInfo.isNull()) {
                        result[i].hasBoardingInfo = true;
                        result[i].boardingInfo.passType = boardingInfo.toObject().value("passType").toInt();
                        result[i].boardingInfo.passStatus = boardingInfo.toObject().value("passStatus").toInt();
                        result[i].boardingInfo.passTime = boardingInfo.toObject().value("passTime").toString();
                        result[i].boardingInfo.gateNo = boardingInfo.toObject().value("gateNo").toString();
                        result[i].boardingInfo.deviceId = boardingInfo.toObject().value("deviceId").toString();
                        result[i].boardingInfo.channelName = boardingInfo.toObject().value("channelName").toString();
                        result[i].boardingInfo.photoPath = boardingInfo.toObject().value("photoPath").toString();
                    } else {
                        result[i].hasBoardingInfo = false;
                    }
                }
            }
        }
        if (docObj.contains("isBaby")) {
            isBaby = docObj.value("isBaby").toInt();
        }
    }
};

struct FlowReviewRequest {
    QString input;
} ;

struct FlowReviewResponse {
    QJsonDocument doc;
    int errorCode;
    bool founded;
    FlowReviewInterface interface;

    void dump() {
        qDebug() << doc;
    }

    void update(QJsonDocument &doc) {
        doc = doc;

        QJsonObject docObj = doc.object();

        QJsonValue resultJson;
        if (docObj.contains("result")) {
            resultJson = docObj.value("result");
        }

        if (resultJson == QJsonValue()) {
            founded = false;
        }

        interface.update(doc);
    }

    FlowReviewResponse () {
        doc = QJsonDocument();
        errorCode = -1;
        founded = true;
    }
};

/******************************************************************************************************/

/*100-发布航班计划 paramList******************************************************************************/
typedef struct{                                                                                                                                                                                          /**/
    QString threeFlightNo{"null"};        //非必须    航班3字码                                                                                                                /**/
    QString twoFlightNo{"null"};      //非必须    航班2字码                                                                                                                     /**/
    QString depTimeJ{"null"};         //非必须    预计起飞时间                                                                                                                 /**/
    QString flightDate{"null"};       //非必须    航班日期yyyy-MM-dd                                                                                                     /**/
    int status{-1};                  //非必须    航班状态  0-等待，1-建库，2-开始登机，3-结束登机，4-起飞                                            /**/
    QString midwayCode{"null"};       //非必须    中转机场3字码                                                                                                           /**/
    QString midwayName{"null"};       //非必须    中转机场名称                                                                                                            /**/
    QString arrAirportCode{"null"};	    //非必须    到达机场3字码                                                                                                        /**/
    QString arrAirportName{"null"};	    //非必须    到达机场名                                                                                                              /**/
    QString depAirportCode{"null"};	    //非必须    起飞机场3字码                                                                                                        /**/
    QString depAirportName{"null"};	//非必须    起飞机场名称                                                                                                          /**/
    QStringList shareFltno{"null"};                                                                                                                                                                        /**/
    int  kindType{-1};       //非必须    航班类型  0-原始，1-增加航班，2-删除航班，3-备降航班，4-二次返航航班                           /**/
    bool manulOpt{false};      //非必须    true-人工拉取的航班，false-非人工拉取的航班                                                                  /**/
    QString boardingGate{"null"};     //非必须    登机口                                                                                                                          /**/
}content100;                                                                                                                                                                                             /**/
                                                                                                                                                                                                                   /**/
typedef struct {                                                                                                                                                                                         /**/
    QString boardingGate{"null"};                                                                                                                                                             /**/
    content100 content;                                                                                                                                                                            /**/
}msg100;                                                                                                                                                                                                    /**/
                                                                                                                                                                                                                    /**/
typedef struct                                                                                                                                                                                            /**/
{                                                                                                                                                                                                                   /**/
    int optType;        //100                                                                                                                                                                           /**/
    QJsonArray msg;     //[msg100,msg100,msg100,...]                                                                                                                            /**/
}MqArrayMsg100;                                                                                                                                                                                        /**/
/*******************************************************************************************************/

/*101-建库消息 paramList**********************************************************************************/
typedef struct{                                                                                                                                                                                          /**/
    QString threeFlightNo{"null"};        //非必须    航班3字码                                                                                                                /**/
    QString twoFlightNo{"null"};      //非必须    航班2字码                                                                                                                     /**/
    QString depTimeJ{"null"};         //非必须    预计起飞时间                                                                                                                 /**/
    QString flightDate{"null"};       //非必须    航班日期yyyy-MM-dd                                                                                                     /**/
    int status{-1};                  //非必须    航班状态  0-等待，1-建库，2-开始登机，3-结束登机，4-起飞                                            /**/
    QString midwayCode{"null"};       //非必须    中转机场3字码                                                                                                           /**/
    QString midwayName{"null"};       //非必须    中转机场名称                                                                                                            /**/
    QString arrAirportCode{"null"};	    //非必须    到达机场3字码                                                                                                        /**/
    QString arrAirportName{"null"};	    //非必须    到达机场名                                                                                                              /**/
    QString depAirportCode{"null"};	    //非必须    起飞机场3字码                                                                                                        /**/
    QString depAirportName{"null"};	//非必须    起飞机场名称                                                                                                          /**/
    QStringList shareFltno{"null"};                                                                                                                                                                        /**/
    int  kindType{-1};       //非必须    航班类型  0-原始，1-增加航班，2-删除航班，3-备降航班，4-二次返航航班                           /**/
    bool manulOpt{false};      //非必须    true-人工拉取的航班，false-非人工拉取的航班                                                                  /**/
    QString boardingGate{"null"};     //非必须    登机口                                                                                                                          /**/
}content101;                                                                                                                                                                                             /**/
                                                                                                                                                                                                                   /**/
typedef struct {                                                                                                                                                                                         /**/
    QString boardingGate{"null"};                                                                                                                                                             /**/
    content101 content;                                                                                                                                                                            /**/
}msg101;                                                                                                                                                                                                    /**/
/*******************************************************************************************************/

/*102-开始登机消息 paramList******************************************************************************/
typedef struct{                                                                                                                                                                                          /**/
    QString threeFlightNo{"null"};        //非必须    航班3字码                                                                                                                /**/
    QString twoFlightNo{"null"};      //非必须    航班2字码                                                                                                                     /**/
    QString depTimeJ{"null"};         //非必须    预计起飞时间                                                                                                                 /**/
    QString flightDate{"null"};       //非必须    航班日期yyyy-MM-dd                                                                                                     /**/
    int status{-1};                  //非必须    航班状态  0-等待，1-建库，2-开始登机，3-结束登机，4-起飞                                            /**/
    QString midwayCode{"null"};       //非必须    中转机场3字码                                                                                                           /**/
    QString midwayName{"null"};       //非必须    中转机场名称                                                                                                            /**/
    QString arrAirportCode{"null"};	    //非必须    到达机场3字码                                                                                                        /**/
    QString arrAirportName{"null"};	    //非必须    到达机场名                                                                                                              /**/
    QString depAirportCode{"null"};	    //非必须    起飞机场3字码                                                                                                        /**/
    QString depAirportName{"null"};	//非必须    起飞机场名称                                                                                                          /**/
    QStringList shareFltno{"null"};                                                                                                                                                                        /**/
    int  kindType{-1};       //非必须    航班类型  0-原始，1-增加航班，2-删除航班，3-备降航班，4-二次返航航班                           /**/
    bool manulOpt{false};      //非必须    true-人工拉取的航班，false-非人工拉取的航班                                                                  /**/
    QString boardingGate{"null"};     //非必须    登机口                                                                                                                          /**/
}content102;                                                                                                                                                                                             /**/
                                                                                                                                                                                                                   /**/
typedef struct {                                                                                                                                                                                         /**/
    QString boardingGate{"null"};                                                                                                                                                             /**/
    content102 content;                                                                                                                                                                            /**/
}msg102;                                                                                                                                                                                                    /**/
/*******************************************************************************************************/

/*103-结束登机消息 paramList******************************************************************************/
typedef struct{                                                                                                                                                                                          /**/
    QString threeFlightNo{"null"};        //非必须    航班3字码                                                                                                                /**/
    QString twoFlightNo{"null"};      //非必须    航班2字码                                                                                                                     /**/
    QString depTimeJ{"null"};         //非必须    预计起飞时间                                                                                                                 /**/
    QString boardingTime{"null"};   //非必须   登机时间                                                                                                                        /**/
    QString flightDate{"null"};       //非必须    航班日期yyyy-MM-dd                                                                                                     /**/
    int status{-1};                  //非必须    航班状态  0-等待，1-建库，2-开始登机，3-结束登机，4-起飞                                            /**/
    QString midwayCode{"null"};       //非必须    中转机场3字码                                                                                                           /**/
    QString midwayName{"null"};       //非必须    中转机场名称                                                                                                            /**/
    QString arrAirportCode{"null"};	    //非必须    到达机场3字码                                                                                                        /**/
    QString arrAirportName{"null"};	    //非必须    到达机场名                                                                                                              /**/
    QString depAirportCode{"null"};	    //非必须    起飞机场3字码                                                                                                        /**/
    QString depAirportName{"null"};	//非必须    起飞机场名称                                                                                                          /**/
    int boardingNum;
    int orgDepNum;
    int transferNum;
    int midwayNum;
    int faceNums;
    QString lastRecogTime{"null"};
    QStringList shareFltno{"null"};                                                                                                                                                                        /**/
    int  kindType{-1};       //非必须    航班类型  0-原始，1-增加航班，2-删除航班，3-备降航班，4-二次返航航班                           /**/
    bool manulOpt{false};      //非必须    true-人工拉取的航班，false-非人工拉取的航班                                                                  /**/
    QString boardingGate{"null"};     //非必须    登机口                                                                                                                          /**/
}content103;                                                                                                                                                                                             /**/
                                                                                                                                                                                                                   /**/
typedef struct {                                                                                                                                                                                         /**/
    QString boardingGate{"null"};                                                                                                                                                             /**/
    content103 content;                                                                                                                                                                            /**/
}msg103;                                                                                                                                                                                                    /**/
/*******************************************************************************************************/

/*109-删库消息 paramList**********************************************************************************/
typedef struct{                                                                                                                                                                                          /**/
    QString threeFlightNo{"null"};        //非必须    航班3字码                                                                                                                /**/
    QString twoFlightNo{"null"};      //非必须    航班2字码                                                                                                                     /**/
    QString depTimeJ{"null"};         //非必须    预计起飞时间                                                                                                                 /**/
    QString boardingTime{"null"};   //非必须   登机时间                                                                                                                        /**/
    QString flightDate{"null"};       //非必须    航班日期yyyy-MM-dd                                                                                                     /**/
    int status{-1};                  //非必须    航班状态  0-等待，1-建库，2-开始登机，3-结束登机，4-起飞                                            /**/
    QString midwayCode{"null"};       //非必须    中转机场3字码                                                                                                           /**/
    QString midwayName{"null"};       //非必须    中转机场名称                                                                                                            /**/
    QString arrAirportCode{"null"};	    //非必须    到达机场3字码                                                                                                        /**/
    QString arrAirportName{"null"};	    //非必须    到达机场名                                                                                                              /**/
    QString depAirportCode{"null"};	    //非必须    起飞机场3字码                                                                                                        /**/
    QString depAirportName{"null"};	//非必须    起飞机场名称                                                                                                          /**/
    QStringList shareFltno{"null"};                                                                                                                                                                        /**/
    int  kindType{-1};       //非必须    航班类型  0-原始，1-增加航班，2-删除航班，3-备降航班，4-二次返航航班                           /**/
    bool manulOpt{false};      //非必须    true-人工拉取的航班，false-非人工拉取的航班                                                                  /**/
    QString boardingGate{"null"};     //非必须    登机口                                                                                                                          /**/
}content109;                                                                                                                                                                                             /**/
                                                                                                                                                                                                                   /**/
typedef struct {                                                                                                                                                                                         /**/
    QString boardingGate{"null"};                                                                                                                                                             /**/
    content109 content;                                                                                                                                                                            /**/
}msg109;                                                                                                                                                                                                    /**/
/*******************************************************************************************************/

/*300-识别结果推送 paramList******************************************************************************/
typedef struct{                                                                                                                                                                                          /**/
    QString startPort{"null"};      //:null,                                                                                                                                                    /**/
    QString endPort{"null"};        //:null,                                                                                                                                                    /**/
    QString seatNumber{"null"};     //:null,                                                                                                                                             /**/
    QString faceImage{"null"};      //image",                                                                                                                                            /**/
    QString isVipChannel{"null"};       //:0,                                                                                                                                               /**/
    int recognizeType{-1};      //:-1,                                                                                                                                                    /**/
    int boardingStatus{0};        //:0,                                                                                                                                                   /**/
    int cardType{0};       //:0,                                                                                                                                                                  /**/
    QString cardNo{"null"};     //:null,                                                                                                                                                            /**/
    QString nameZh{"null"};     //:null,                                                                                                                                                         /**/
    QString nameEn{"null"};     //:null,                                                                                                                                                            /**/
    int sex{0};        //:0,                                                                                                                                                                                     /**/
    QString recognizeTime{"null"};      //:1576031373382,                                                                                                                           /**/
    float threshold{0.0};        //:0.0                                                                                                                                                                 /**/
    float score{0.0};        //:0.0,                                                                                                                                                                       /**/
    QString basePhoto{"null"};      //:null,                                                                                                                                                      /**/
    QString flightNumber{"null"};       //:null,                                                                                                                                                /**/
    QString boardingNumber{"null"};     //:null,                                                                                                                                            /**/
    QString flightDay{"null"};      //:null,                                                                                                                                                         /**/
    QString fId{"null"};        //:"3fb6c5f90fe24290b5396c619d8685df",                                                                                                     /**/
    QString sourceType{"null"};     //:null,                                                                                                                                                      /**/
    QString transferInfo{"null"};       //:null,                                                                                                                                                   /**/
    QString reviewInfo{"null"};     //:null,                                                                                                                                                      /**/
    QString securityInfo{"null"};       //:null                                                                                                                                                   /**/
}content300;                                                                                                                                                                                                 /**/
                                                                                                                                                                                                                       /**/
typedef struct {                                                                                                                                                                                            /**/
    QString boardingGate{"null"};                                                                                                                                                                /**/
    content300 content;                                                                                                                                                                               /**/
}msg300;                                                                                                                                                                                                      /**/
/*******************************************************************************************************/

/*301-航班的登机人数 paramList****************************************************************************/
typedef struct{                                                                                                                                                                                          /**/
    QString flightNo{"null"};         //非必须    航班号                                                                                                                               /**/
    QString flightDate{"null"};       //非必须    航班日期yyyy-MM-dd                                                                                                     /**/
    int boardingNum{-1};        //非必须    登机人数                                                                                                                                 /**/
}content301;                                                                                                                                                                                             /**/
                                                                                                                                                                                                                   /**/
typedef struct {                                                                                                                                                                                         /**/
    QString boardingGate{"null"};                                                                                                                                                             /**/
    content301 content;                                                                                                                                                                            /**/
}msg301;                                                                                                                                                                                                    /**/
/*******************************************************************************************************/
#endif // API_PARAM_DEF_H

