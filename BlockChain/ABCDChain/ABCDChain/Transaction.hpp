//
//  Transaction.hpp
//  Private_Blockchain_forDB
//
//  Created by HwaSoek Syhn on 2018. 6. 25..
//  Copyright © 2018년 HwaSoek Syhn. All rights reserved.
//

#ifndef Transaction_hpp
#define Transaction_hpp

#include <iostream>
#include <ctime>
#include "json/json.h"

enum TransactionType { WalletRegistration, Issue, Payment, Rental, Return };

class Transaction{
	
private:
    TransactionType _type;
    std::string _address1;
    std::string _address2;
    double _amount;
    std::string _deviceId;
    time_t _time1;
    time_t _time2;
	time_t _timeStamp;
    
public:
	
    Transaction(TransactionType type, std::string address1, time_t _timeStamp);
    Transaction(TransactionType type, std::string address1, std::string address2, double amount, time_t _timeStamp);
    Transaction(TransactionType type, std::string address1, double amount, time_t _timeStamp);
    Transaction(TransactionType type, std::string address1, std::string deviceId, double amount,
                time_t Time1, time_t Time2, time_t _timeStamp);
    Transaction(Json::Value json);
    Transaction(std::string jsonStr);
	void SetFromJson(Json::Value json);
    
    
    
    TransactionType GetTransactionType();
    std::string GetAddress1();
    std::string GetAddress2();
    double GetAmount();
    std::string GetDeviceId();
    time_t GetTime1();
    time_t GetTime2();
	time_t GetTimeStamp();
    
    std::string GetJson();
    Json::Value GetJsonValue();
};

#endif /* Transaction_hpp */
