﻿#ifndef API_PARAM_DEF_H
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
        boardingNum = 0;
        faceNums = 0;
        manualNum = 0;
        midwayNum = 0;
        orgDepNum = 0;
        recognizeNum = 0;
        transferNum = 0;
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
    QString basePhoto;
    int repeatFlag;
    QString seatNumber;
    QString updateTime;
    QString boardingTime;
    int sourceType;
    int isInterceptLabel;
    bool isSameBoardingNumber;

    FlightReviewResult() {
        boardingNumber = QString();
        boardingStatus = -1;
        cardNo = QString();
        flightNumber = QString();
        id = QString();
        passengerName = QString();
        photoPath = QString();
        basePhoto = QString();
        repeatFlag = -1;
        seatNumber = QString();
        updateTime = QString();
        boardingTime = QString();
        sourceType = 255;
        isInterceptLabel = 0;
        isSameBoardingNumber = true;
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
    FlightReviewResult boarded[1000];
    FlightReviewResult unboard[1000];

    FlightReviewInterface() {
        status = -1;
        reqId = QString();
        msg = QString();
        num = -1;
        total = -1;
        validSize = 0;
    }

    void doUpdate(QJsonDocument &doc) {
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
            updateResults(docObj);
            updateBoarded(docObj);
            updateUnBoard(docObj);
        }
    }

    void updateResults(QJsonObject &docObj) {
        FlightReviewResult sortings[1000];

        QJsonValue resultsJson = docObj.value("results");
        if (!resultsJson.isNull() && resultsJson.isArray()) {
            QJsonArray array = resultsJson.toArray();
            validSize = array.size();
            for (int i = 0; i < validSize && i < 1000; i++) {
                QString time = array.at(i).toObject().value("updateTime").toString();
                QString boardingNumber = array.at(i).toObject().value("boardingNumber").toString();
                QString id = array.at(i).toObject().value("id").toString();
                if (i == 0) {
                    sortings[i].boardingNumber = array.at(i).toObject().value("boardingNumber").toString();
                    sortings[i].boardingStatus = array.at(i).toObject().value("boardingStatus").toInt();
                    sortings[i].cardNo = array.at(i).toObject().value("cardNo").toString();
                    sortings[i].flightNumber = array.at(i).toObject().value("flightNumber").toString();
                    sortings[i].id = array.at(i).toObject().value("id").toString();
                    sortings[i].passengerName = array.at(i).toObject().value("passengerName").toString();
                    sortings[i].photoPath = array.at(i).toObject().value("photoPath").toString();
                    sortings[i].repeatFlag = array.at(i).toObject().value("repeatFlag").toInt();
                    sortings[i].seatNumber = array.at(i).toObject().value("seatNumber").toString();
                    sortings[i].updateTime = array.at(i).toObject().value("updateTime").toString();
                    sortings[i].boardingTime = array.at(i).toObject().value("boardingTime").toString();
                    sortings[i].sourceType = array.at(i).toObject().value("sourceType").toInt();
                    sortings[i].isInterceptLabel = array.at(i).toObject().value("isInterceptLabel").toInt();
                    sortings[i].isSameBoardingNumber = false;
                } else {
                    bool isSameboardingNumber = false;
                    int sameBoardingNumber = 0;
                    for (int j = 0; j < i; j++) {
                        if ((boardingNumber == sortings[j].boardingNumber) && (boardingNumber != "") && (id != "")) {
                            isSameboardingNumber = true;
                            sameBoardingNumber = j;
                            break;
                        }
                    }

                    if (isSameboardingNumber) {
                        QString tempBoardingNumber = sortings[sameBoardingNumber].boardingNumber;
                        int tempBoardingStatus = sortings[sameBoardingNumber].boardingStatus;
                        QString tempCardNo = sortings[sameBoardingNumber].cardNo;
                        QString tempFlightNumber = sortings[sameBoardingNumber].flightNumber;
                        QString tempId = sortings[sameBoardingNumber].id;
                        QString tempPassengerName = sortings[sameBoardingNumber].passengerName;
                        QString tempPhotoPath = sortings[sameBoardingNumber].photoPath;
                        int tempRepeatFlag = sortings[sameBoardingNumber].repeatFlag;
                        QString tempSeatNumber = sortings[sameBoardingNumber].seatNumber;
                        QString tempUpdateTime = sortings[sameBoardingNumber].updateTime;
                        QString tempBoardingTime = sortings[sameBoardingNumber].boardingTime;
                        int tempSourceType = sortings[sameBoardingNumber].sourceType;
                        int tempIsInterceptLabel = sortings[sameBoardingNumber].isInterceptLabel;

                        for (int k = i; k > 0; k--) {
                            if (k > sameBoardingNumber + 1) {
                                sortings[k].boardingNumber = sortings[k - 1].boardingNumber;
                                sortings[k].boardingStatus = sortings[k - 1].boardingStatus;
                                sortings[k].cardNo = sortings[k - 1].cardNo;
                                sortings[k].flightNumber = sortings[k - 1].flightNumber;
                                sortings[k].id = sortings[k - 1].id;
                                sortings[k].passengerName = sortings[k - 1].passengerName;
                                sortings[k].photoPath = sortings[k - 1].photoPath;
                                sortings[k].repeatFlag = sortings[k - 1].repeatFlag;
                                sortings[k].seatNumber = sortings[k - 1].seatNumber;
                                sortings[k].updateTime = sortings[k - 1].updateTime;
                                sortings[k].boardingTime = sortings[k - 1].boardingTime;
                                sortings[k].sourceType = sortings[k - 1].sourceType;
                                sortings[k].isInterceptLabel = sortings[k - 1].isInterceptLabel;
                                sortings[k].isSameBoardingNumber = sortings[k - 1].isSameBoardingNumber;
                            } else if (k >= 2) {
                                sortings[k].boardingNumber = sortings[k - 2].boardingNumber;
                                sortings[k].boardingStatus = sortings[k - 2].boardingStatus;
                                sortings[k].cardNo = sortings[k - 2].cardNo;
                                sortings[k].flightNumber = sortings[k - 2].flightNumber;
                                sortings[k].id = sortings[k - 2].id;
                                sortings[k].passengerName = sortings[k - 2].passengerName;
                                sortings[k].photoPath = sortings[k - 2].photoPath;
                                sortings[k].repeatFlag = sortings[k - 2].repeatFlag;
                                sortings[k].seatNumber = sortings[k - 2].seatNumber;
                                sortings[k].updateTime = sortings[k - 2].updateTime;
                                sortings[k].boardingTime = sortings[k - 2].boardingTime;
                                sortings[k].sourceType = sortings[k - 2].sourceType;
                                sortings[k].isInterceptLabel = sortings[k - 2].isInterceptLabel;
                                sortings[k].isSameBoardingNumber = sortings[k - 2].isSameBoardingNumber;
                            }
                        }

                        sortings[1].boardingNumber = tempBoardingNumber;
                        sortings[1].boardingStatus = tempBoardingStatus;
                        sortings[1].cardNo = tempCardNo;
                        sortings[1].flightNumber = tempFlightNumber;
                        sortings[1].id = tempId;
                        sortings[1].passengerName = tempPassengerName;
                        sortings[1].photoPath = tempPhotoPath;
                        sortings[1].repeatFlag = tempRepeatFlag;
                        sortings[1].seatNumber = tempSeatNumber;
                        sortings[1].updateTime = tempUpdateTime;
                        sortings[1].boardingTime = tempBoardingTime;
                        sortings[1].sourceType = tempSourceType;
                        sortings[1].isInterceptLabel = tempIsInterceptLabel;
                        sortings[1].isSameBoardingNumber = true;

                        sortings[0].boardingNumber = array.at(i).toObject().value("boardingNumber").toString();
                        sortings[0].boardingStatus = array.at(i).toObject().value("boardingStatus").toInt();
                        sortings[0].cardNo = array.at(i).toObject().value("cardNo").toString();
                        sortings[0].flightNumber = array.at(i).toObject().value("flightNumber").toString();
                        sortings[0].id = array.at(i).toObject().value("id").toString();
                        sortings[0].passengerName = array.at(i).toObject().value("passengerName").toString();
                        sortings[0].photoPath = array.at(i).toObject().value("photoPath").toString();
                        sortings[0].repeatFlag = array.at(i).toObject().value("repeatFlag").toInt();
                        sortings[0].seatNumber = array.at(i).toObject().value("seatNumber").toString();
                        sortings[0].updateTime = array.at(i).toObject().value("updateTime").toString();
                        sortings[0].boardingTime = array.at(i).toObject().value("boardingTime").toString();
                        sortings[0].sourceType = array.at(i).toObject().value("sourceType").toInt();
                        sortings[0].isInterceptLabel = array.at(i).toObject().value("isInterceptLabel").toInt();
                        sortings[0].isSameBoardingNumber = true;
                    } else {
                        for (int j = 0; j < i; j++) {
                            if (time > sortings[j].updateTime && !sortings[j].isSameBoardingNumber) {
                                for (int k = i - 1; k >= j; k--) {
                                    sortings[k + 1].boardingNumber = sortings[k].boardingNumber;
                                    sortings[k + 1].boardingStatus = sortings[k].boardingStatus;
                                    sortings[k + 1].cardNo = sortings[k].cardNo;
                                    sortings[k + 1].flightNumber = sortings[k].flightNumber;
                                    sortings[k + 1].id = sortings[k].id;
                                    sortings[k + 1].passengerName = sortings[k].passengerName;
                                    sortings[k + 1].photoPath = sortings[k].photoPath;
                                    sortings[k + 1].repeatFlag = sortings[k].repeatFlag;
                                    sortings[k + 1].seatNumber = sortings[k].seatNumber;
                                    sortings[k + 1].updateTime = sortings[k].updateTime;
                                    sortings[k + 1].boardingTime = sortings[k].boardingTime;
                                    sortings[k + 1].sourceType = sortings[k].sourceType;
                                    sortings[k + 1].isInterceptLabel = sortings[k].isInterceptLabel;
                                    sortings[k + 1].isSameBoardingNumber = sortings[k].isSameBoardingNumber;
                                }
                                sortings[j].boardingNumber = array.at(i).toObject().value("boardingNumber").toString();
                                sortings[j].boardingStatus = array.at(i).toObject().value("boardingStatus").toInt();
                                sortings[j].cardNo = array.at(i).toObject().value("cardNo").toString();
                                sortings[j].flightNumber = array.at(i).toObject().value("flightNumber").toString();
                                sortings[j].id = array.at(i).toObject().value("id").toString();
                                sortings[j].passengerName = array.at(i).toObject().value("passengerName").toString();
                                sortings[j].photoPath = array.at(i).toObject().value("photoPath").toString();
                                sortings[j].repeatFlag = array.at(i).toObject().value("repeatFlag").toInt();
                                sortings[j].seatNumber = array.at(i).toObject().value("seatNumber").toString();
                                sortings[j].updateTime = array.at(i).toObject().value("updateTime").toString();
                                sortings[j].boardingTime = array.at(i).toObject().value("boardingTime").toString();
                                sortings[j].sourceType = array.at(i).toObject().value("sourceType").toInt();
                                sortings[j].isInterceptLabel = array.at(i).toObject().value("isInterceptLabel").toInt();
                                sortings[j].isSameBoardingNumber = false;
                                break;
                            }
                            if (j == i - 1) {
                                sortings[i].boardingNumber = array.at(i).toObject().value("boardingNumber").toString();
                                sortings[i].boardingStatus = array.at(i).toObject().value("boardingStatus").toInt();
                                sortings[i].cardNo = array.at(i).toObject().value("cardNo").toString();
                                sortings[i].flightNumber = array.at(i).toObject().value("flightNumber").toString();
                                sortings[i].id = array.at(i).toObject().value("id").toString();
                                sortings[i].passengerName = array.at(i).toObject().value("passengerName").toString();
                                sortings[i].photoPath = array.at(i).toObject().value("photoPath").toString();
                                sortings[i].repeatFlag = array.at(i).toObject().value("repeatFlag").toInt();
                                sortings[i].seatNumber = array.at(i).toObject().value("seatNumber").toString();
                                sortings[i].updateTime = array.at(i).toObject().value("updateTime").toString();
                                sortings[i].boardingTime = array.at(i).toObject().value("boardingTime").toString();
                                sortings[i].sourceType = array.at(i).toObject().value("sourceType").toInt();
                                sortings[i].isInterceptLabel = array.at(i).toObject().value("isInterceptLabel").toInt();
                                sortings[i].isSameBoardingNumber = false;
                            }
                        }
                    }
                }
            }

            // 对isSameBoardingNumber记录再排序，
            // 一是为了让相同序列号（考虑三个或者更多记录具有相同序列号的情况）的排在一起，
            // 二是为了让有特征（0/4/5/7）的居于这些重复序列号记录的前部位置。
            int firstNotSameNumber = 0;
            for (int i = 0; i < validSize && i < 1000; i++) {
                if (!sortings[i].isSameBoardingNumber) {
                    firstNotSameNumber = i;
                    break;
                }
            }

            QList<QString> featureBoardingNoList;
            for (int i = 0; i < firstNotSameNumber; i++) {
                bool isAlreadyInList = false;
                if ((sortings[i].sourceType == 0)
                        || (sortings[i].sourceType == 4)
                        || (sortings[i].sourceType == 5)
                        || (sortings[i].sourceType == 7)) {
                    for (int j = 0; j < featureBoardingNoList.size(); j++) {
                        if (featureBoardingNoList.at(j) == sortings[i].boardingNumber) {
                            isAlreadyInList = true;
                            break;
                        }
                    }
                    if (!isAlreadyInList) {
                        featureBoardingNoList << sortings[i].boardingNumber;
                    }
                }
            }

            QList<QString> noFeatureBoardingNoList;
            for (int i = 0; i < firstNotSameNumber; i++) {
                bool isAlreadyInAggrList = false;
                for (int j = 0; j < featureBoardingNoList.size(); j++) {
                    if (featureBoardingNoList.at(j) == sortings[i].boardingNumber) {
                        isAlreadyInAggrList = true;
                        break;
                    }
                }
                if (!isAlreadyInAggrList) {
                    bool isAlreadyInNoFeaList = false;
                    for (int j = 0; j < noFeatureBoardingNoList.size(); j++) {
                        if (noFeatureBoardingNoList.at(j) == sortings[i].boardingNumber) {
                            isAlreadyInNoFeaList = true;
                            break;
                        }
                    }
                    if (!isAlreadyInNoFeaList) {
                        noFeatureBoardingNoList << sortings[i].boardingNumber;
                    }
                }
            }

            for (int i = 0; i < noFeatureBoardingNoList.size(); i++) {
                for (int j = 0; j < firstNotSameNumber; j++) {
                    if (sortings[j].boardingNumber == noFeatureBoardingNoList.at(i)) {
                        for (int k = firstNotSameNumber; k > 0; k--) {
                            results[k].boardingNumber = sortings[k - 1].boardingNumber;
                            results[k].boardingStatus = sortings[k - 1].boardingStatus;
                            results[k].cardNo = sortings[k - 1].cardNo;
                            results[k].flightNumber = sortings[k - 1].flightNumber;
                            results[k].id = sortings[k - 1].id;
                            results[k].passengerName = sortings[k - 1].passengerName;
                            results[k].photoPath = sortings[k - 1].photoPath;
                            results[k].repeatFlag = sortings[k - 1].repeatFlag;
                            results[k].seatNumber = sortings[k - 1].seatNumber;
                            results[k].updateTime = sortings[k - 1].updateTime;
                            results[k].boardingTime = sortings[k - 1].boardingTime;
                            results[k].sourceType = sortings[k - 1].sourceType;
                            results[k].isInterceptLabel = sortings[k - 1].isInterceptLabel;
                            results[k].isSameBoardingNumber = sortings[k - 1].isSameBoardingNumber;
                        }
                        results[0].boardingNumber = sortings[j].boardingNumber;
                        results[0].boardingStatus = sortings[j].boardingStatus;
                        results[0].cardNo = sortings[j].cardNo;
                        results[0].flightNumber = sortings[j].flightNumber;
                        results[0].id = sortings[j].id;
                        results[0].passengerName = sortings[j].passengerName;
                        results[0].photoPath = sortings[j].photoPath;
                        results[0].repeatFlag = sortings[j].repeatFlag;
                        results[0].seatNumber = sortings[j].seatNumber;
                        results[0].updateTime = sortings[j].updateTime;
                        results[0].boardingTime = sortings[j].boardingTime;
                        results[0].sourceType = sortings[j].sourceType;
                        results[0].isInterceptLabel = sortings[j].isInterceptLabel;
                        results[0].isSameBoardingNumber = sortings[j].isSameBoardingNumber;
                    }
                }
            }

            for (int i = 0; i < featureBoardingNoList.size(); i++) {
                for (int j = 0; j < firstNotSameNumber; j++) {
                    if (sortings[j].boardingNumber == featureBoardingNoList.at(i)) {
                        for (int k = firstNotSameNumber; k > 0; k--) {
                            results[k].boardingNumber = sortings[k - 1].boardingNumber;
                            results[k].boardingStatus = sortings[k - 1].boardingStatus;
                            results[k].cardNo = sortings[k - 1].cardNo;
                            results[k].flightNumber = sortings[k - 1].flightNumber;
                            results[k].id = sortings[k - 1].id;
                            results[k].passengerName = sortings[k - 1].passengerName;
                            results[k].photoPath = sortings[k - 1].photoPath;
                            results[k].repeatFlag = sortings[k - 1].repeatFlag;
                            results[k].seatNumber = sortings[k - 1].seatNumber;
                            results[k].updateTime = sortings[k - 1].updateTime;
                            results[k].boardingTime = sortings[k - 1].boardingTime;
                            results[k].sourceType = sortings[k - 1].sourceType;
                            results[k].isInterceptLabel = sortings[k - 1].isInterceptLabel;
                            results[k].isSameBoardingNumber = sortings[k - 1].isSameBoardingNumber;
                        }
                        results[0].boardingNumber = sortings[j].boardingNumber;
                        results[0].boardingStatus = sortings[j].boardingStatus;
                        results[0].cardNo = sortings[j].cardNo;
                        results[0].flightNumber = sortings[j].flightNumber;
                        results[0].id = sortings[j].id;
                        results[0].passengerName = sortings[j].passengerName;
                        results[0].photoPath = sortings[j].photoPath;
                        results[0].repeatFlag = sortings[j].repeatFlag;
                        results[0].seatNumber = sortings[j].seatNumber;
                        results[0].updateTime = sortings[j].updateTime;
                        results[0].boardingTime = sortings[j].boardingTime;
                        results[0].sourceType = sortings[j].sourceType;
                        results[0].isInterceptLabel = sortings[j].isInterceptLabel;
                        results[0].isSameBoardingNumber = sortings[j].isSameBoardingNumber;
                    }
                }
            }

            for (int i = 0; i < validSize && i < 1000; i++) {
                if (!sortings[i].isSameBoardingNumber) {
                    results[i].boardingNumber = sortings[i].boardingNumber;
                    results[i].boardingStatus = sortings[i].boardingStatus;
                    results[i].cardNo = sortings[i].cardNo;
                    results[i].flightNumber = sortings[i].flightNumber;
                    results[i].id = sortings[i].id;
                    results[i].passengerName = sortings[i].passengerName;
                    results[i].photoPath = sortings[i].photoPath;
                    results[i].repeatFlag = sortings[i].repeatFlag;
                    results[i].seatNumber = sortings[i].seatNumber;
                    results[i].updateTime = sortings[i].updateTime;
                    results[i].boardingTime = sortings[i].boardingTime;
                    results[i].sourceType = sortings[i].sourceType;
                    results[i].isInterceptLabel = sortings[i].isInterceptLabel;
                    results[i].isSameBoardingNumber = sortings[i].isSameBoardingNumber;
                }
            }
        }
    }

    void updateBoarded(QJsonObject &docObj) {
        QJsonValue resultsJson = docObj.value("results");
        if (!resultsJson.isNull() && resultsJson.isArray()) {
            QJsonArray array = resultsJson.toArray();
            validSize = array.size();
            for (int i = 0; i < validSize && i < 1000; i++) {
                QString boardingTime = array.at(i).toObject().value("boardingTime").toString();
                if (i == 0) {
                    boarded[i].boardingNumber = array.at(i).toObject().value("boardingNumber").toString();
                    boarded[i].boardingStatus = array.at(i).toObject().value("boardingStatus").toInt();
                    boarded[i].cardNo = array.at(i).toObject().value("cardNo").toString();
                    boarded[i].flightNumber = array.at(i).toObject().value("flightNumber").toString();
                    boarded[i].id = array.at(i).toObject().value("id").toString();
                    boarded[i].passengerName = array.at(i).toObject().value("passengerName").toString();
                    boarded[i].photoPath = array.at(i).toObject().value("photoPath").toString();
                    boarded[i].repeatFlag = array.at(i).toObject().value("repeatFlag").toInt();
                    boarded[i].seatNumber = array.at(i).toObject().value("seatNumber").toString();
                    boarded[i].updateTime = array.at(i).toObject().value("updateTime").toString();
                    boarded[i].boardingTime = array.at(i).toObject().value("boardingTime").toString();
                    boarded[i].sourceType = array.at(i).toObject().value("sourceType").toInt();
                    boarded[i].isInterceptLabel = array.at(i).toObject().value("isInterceptLabel").toInt();
                    boarded[i].isSameBoardingNumber = false;
                } else {
                    for (int j = 0; j < i; j++) {
                        if (boardingTime > boarded[j].boardingTime) {
                            for (int k = i - 1; k >= j; k--) {
                                boarded[k + 1].boardingNumber = boarded[k].boardingNumber;
                                boarded[k + 1].boardingStatus = boarded[k].boardingStatus;
                                boarded[k + 1].cardNo = boarded[k].cardNo;
                                boarded[k + 1].flightNumber = boarded[k].flightNumber;
                                boarded[k + 1].id = boarded[k].id;
                                boarded[k + 1].passengerName = boarded[k].passengerName;
                                boarded[k + 1].photoPath = boarded[k].photoPath;
                                boarded[k + 1].repeatFlag = boarded[k].repeatFlag;
                                boarded[k + 1].seatNumber = boarded[k].seatNumber;
                                boarded[k + 1].updateTime = boarded[k].updateTime;
                                boarded[k + 1].boardingTime = boarded[k].boardingTime;
                                boarded[k + 1].sourceType = boarded[k].sourceType;
                                boarded[k + 1].isInterceptLabel = boarded[k].isInterceptLabel;
                                boarded[k + 1].isSameBoardingNumber = boarded[k].isSameBoardingNumber;
                            }
                            boarded[j].boardingNumber = array.at(i).toObject().value("boardingNumber").toString();
                            boarded[j].boardingStatus = array.at(i).toObject().value("boardingStatus").toInt();
                            boarded[j].cardNo = array.at(i).toObject().value("cardNo").toString();
                            boarded[j].flightNumber = array.at(i).toObject().value("flightNumber").toString();
                            boarded[j].id = array.at(i).toObject().value("id").toString();
                            boarded[j].passengerName = array.at(i).toObject().value("passengerName").toString();
                            boarded[j].photoPath = array.at(i).toObject().value("photoPath").toString();
                            boarded[j].repeatFlag = array.at(i).toObject().value("repeatFlag").toInt();
                            boarded[j].seatNumber = array.at(i).toObject().value("seatNumber").toString();
                            boarded[j].updateTime = array.at(i).toObject().value("updateTime").toString();
                            boarded[j].boardingTime = array.at(i).toObject().value("boardingTime").toString();
                            boarded[j].sourceType = array.at(i).toObject().value("sourceType").toInt();
                            boarded[j].isInterceptLabel = array.at(i).toObject().value("isInterceptLabel").toInt();
                            boarded[j].isSameBoardingNumber = false;
                            break;
                        }
                        if (j == i - 1) {
                            boarded[i].boardingNumber = array.at(i).toObject().value("boardingNumber").toString();
                            boarded[i].boardingStatus = array.at(i).toObject().value("boardingStatus").toInt();
                            boarded[i].cardNo = array.at(i).toObject().value("cardNo").toString();
                            boarded[i].flightNumber = array.at(i).toObject().value("flightNumber").toString();
                            boarded[i].id = array.at(i).toObject().value("id").toString();
                            boarded[i].passengerName = array.at(i).toObject().value("passengerName").toString();
                            boarded[i].photoPath = array.at(i).toObject().value("photoPath").toString();
                            boarded[i].repeatFlag = array.at(i).toObject().value("repeatFlag").toInt();
                            boarded[i].seatNumber = array.at(i).toObject().value("seatNumber").toString();
                            boarded[i].updateTime = array.at(i).toObject().value("updateTime").toString();
                            boarded[i].boardingTime = array.at(i).toObject().value("boardingTime").toString();
                            boarded[i].sourceType = array.at(i).toObject().value("sourceType").toInt();
                            boarded[i].isInterceptLabel = array.at(i).toObject().value("isInterceptLabel").toInt();
                            boarded[i].isSameBoardingNumber = false;
                        }
                    }
                }
            }
        }
    }

    void updateUnBoard(QJsonObject &docObj) {
        QJsonValue resultsJson = docObj.value("results");
        if (!resultsJson.isNull() && resultsJson.isArray()) {
            QJsonArray array = resultsJson.toArray();
            validSize = array.size();
            for (int i = 0; i < validSize && i < 1000; i++) {
                QString seatNumber = array.at(i).toObject().value("seatNumber").toString();
                if (i == 0) {
                    unboard[i].boardingNumber = array.at(i).toObject().value("boardingNumber").toString();
                    unboard[i].boardingStatus = array.at(i).toObject().value("boardingStatus").toInt();
                    unboard[i].cardNo = array.at(i).toObject().value("cardNo").toString();
                    unboard[i].flightNumber = array.at(i).toObject().value("flightNumber").toString();
                    unboard[i].id = array.at(i).toObject().value("id").toString();
                    unboard[i].passengerName = array.at(i).toObject().value("passengerName").toString();
                    unboard[i].photoPath = array.at(i).toObject().value("photoPath").toString();
                    unboard[i].repeatFlag = array.at(i).toObject().value("repeatFlag").toInt();
                    unboard[i].seatNumber = array.at(i).toObject().value("seatNumber").toString();
                    unboard[i].updateTime = array.at(i).toObject().value("updateTime").toString();
                    unboard[i].boardingTime = array.at(i).toObject().value("boardingTime").toString();
                    unboard[i].sourceType = array.at(i).toObject().value("sourceType").toInt();
                    unboard[i].isInterceptLabel = array.at(i).toObject().value("isInterceptLabel").toInt();
                    unboard[i].isSameBoardingNumber = false;
                } else {
                    for (int j = 0; j < i; j++) {
                        if (seatNumber < unboard[j].seatNumber) {
                            for (int k = i - 1; k >= j; k--) {
                                unboard[k + 1].boardingNumber = unboard[k].boardingNumber;
                                unboard[k + 1].boardingStatus = unboard[k].boardingStatus;
                                unboard[k + 1].cardNo = unboard[k].cardNo;
                                unboard[k + 1].flightNumber = unboard[k].flightNumber;
                                unboard[k + 1].id = unboard[k].id;
                                unboard[k + 1].passengerName = unboard[k].passengerName;
                                unboard[k + 1].photoPath = unboard[k].photoPath;
                                unboard[k + 1].repeatFlag = unboard[k].repeatFlag;
                                unboard[k + 1].seatNumber = unboard[k].seatNumber;
                                unboard[k + 1].updateTime = unboard[k].updateTime;
                                unboard[k + 1].boardingTime = unboard[k].boardingTime;
                                unboard[k + 1].sourceType = unboard[k].sourceType;
                                unboard[k + 1].isInterceptLabel = unboard[k].isInterceptLabel;
                                unboard[k + 1].isSameBoardingNumber = unboard[k].isSameBoardingNumber;
                            }
                            unboard[j].boardingNumber = array.at(i).toObject().value("boardingNumber").toString();
                            unboard[j].boardingStatus = array.at(i).toObject().value("boardingStatus").toInt();
                            unboard[j].cardNo = array.at(i).toObject().value("cardNo").toString();
                            unboard[j].flightNumber = array.at(i).toObject().value("flightNumber").toString();
                            unboard[j].id = array.at(i).toObject().value("id").toString();
                            unboard[j].passengerName = array.at(i).toObject().value("passengerName").toString();
                            unboard[j].photoPath = array.at(i).toObject().value("photoPath").toString();
                            unboard[j].repeatFlag = array.at(i).toObject().value("repeatFlag").toInt();
                            unboard[j].seatNumber = array.at(i).toObject().value("seatNumber").toString();
                            unboard[j].updateTime = array.at(i).toObject().value("updateTime").toString();
                            unboard[j].boardingTime = array.at(i).toObject().value("boardingTime").toString();
                            unboard[j].sourceType = array.at(i).toObject().value("sourceType").toInt();
                            unboard[j].isInterceptLabel = array.at(i).toObject().value("isInterceptLabel").toInt();
                            unboard[j].isSameBoardingNumber = false;
                            break;
                        }
                        if (j == i - 1) {
                            unboard[i].boardingNumber = array.at(i).toObject().value("boardingNumber").toString();
                            unboard[i].boardingStatus = array.at(i).toObject().value("boardingStatus").toInt();
                            unboard[i].cardNo = array.at(i).toObject().value("cardNo").toString();
                            unboard[i].flightNumber = array.at(i).toObject().value("flightNumber").toString();
                            unboard[i].id = array.at(i).toObject().value("id").toString();
                            unboard[i].passengerName = array.at(i).toObject().value("passengerName").toString();
                            unboard[i].photoPath = array.at(i).toObject().value("photoPath").toString();
                            unboard[i].repeatFlag = array.at(i).toObject().value("repeatFlag").toInt();
                            unboard[i].seatNumber = array.at(i).toObject().value("seatNumber").toString();
                            unboard[i].updateTime = array.at(i).toObject().value("updateTime").toString();
                            unboard[i].boardingTime = array.at(i).toObject().value("boardingTime").toString();
                            unboard[i].sourceType = array.at(i).toObject().value("sourceType").toInt();
                            unboard[i].isInterceptLabel = array.at(i).toObject().value("isInterceptLabel").toInt();
                            unboard[i].isSameBoardingNumber = false;
                        }
                    }
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
    int queryType;
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

        interface.doUpdate(doc);
    }

    FlightReviewResponse() {
        doc = QJsonDocument();
        queryType = -1;
        errorCode = -1;
        founded = true;
    }
};

struct LibDeleteRequest {
    QString reqId;
    QString flightNo;
    QString gateNo;
    QString boardingGate;
    QString deviceCode;
    QString id;
    QString date;

    LibDeleteRequest() {
        reqId = QString();
        flightNo = QString();
        gateNo = QString();
        boardingGate = QString();
        deviceCode = QString();
        id = QString();
        date = QString();
    }
};

struct LibDeleteResponse {
    int status;
    QString reqId;
    QString msg;

    LibDeleteResponse() {
        status = -1;
        reqId = QString();
        msg = QString();
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
    QString gateNo;
    int isFocus;

    UserInfo() {
        certificateNumber = QString();
        passengerName = QString();
        passengerEnglishName = QString();
        flightNo = QString();
        boardingNumber = QString();
        flightDay = QString();
        seatId = QString();
        gateNo = QString();
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
    int flowNum;
    FlowReviewResult result[10];
    int isBaby;

    FlowReviewInterface() {
        status = -1;
        reqId = QString();
        msg = QString();
        flowNum = 0;
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
                flowNum = array.size();
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
                        result[i].userInfo.gateNo = userInfo.toObject().value("gateNo").toString();
                        result[i].userInfo.isFocus = userInfo.toObject().value("isFocus").toInt();
                    } else {
                        result[i].hasUserInfo = false;
                    }
                    QJsonValue securityInfo = array.at(i).toObject().value("securityInfo");
                    if (!securityInfo.isNull() && !securityInfo.isUndefined()) {
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
                    if (!reviewInfo.isNull() && !reviewInfo.isUndefined()) {
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
                    if (!transferInfo.isNull() && !transferInfo.isUndefined()) {
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
                    if (!boardingInfo.isNull() && !boardingInfo.isUndefined()) {
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

