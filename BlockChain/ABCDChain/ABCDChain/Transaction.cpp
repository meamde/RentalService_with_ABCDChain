//
//  Transaction.cpp
//  Private_Blockchain_forDB
//
//  Created by HwaSoek Syhn on 2018. 6. 25..
//  Copyright © 2018년 HwaSoek Syhn. All rights reserved.
//

#include "Transaction.hpp"

Transaction::Transaction(TransactionType type, std::string address1, time_t timeStamp)
{
    _type = type;
    _address1 = address1;
	_timeStamp = timeStamp;
}
Transaction::Transaction(TransactionType type, std::string address1, std::string address2, double amount, time_t timeStamp)
{
    _type = type;
    _address1 = address1;
    _address2 = address2;
    _amount = amount;
	_timeStamp = timeStamp;
}
Transaction::Transaction(TransactionType type, std::string address1, double amount, time_t timeStamp)
{
    _type = type;
    _address1 = address1;
    _amount = amount;
	_timeStamp = timeStamp;
}
Transaction::Transaction(TransactionType type, std::string address1, std::string deviceId, double amount,
                         time_t Time1, time_t Time2, time_t timeStamp)
{
    _type = type;
    _address1 = address1;
    _deviceId = deviceId;
    _amount = amount;
	_timeStamp = timeStamp;
    
    switch (type) {
        case Rental:
            _time1 = Time1; //Rental time
            _time2 = Time2; //Due time
            break;
        case Return:
            _time1 = Time1; //Due time
            _time2 = Time2; //Return time
            break;
            
        default:
			std::cout << &"ABCDBlock : Transaction Type Error" << std::endl;
            break;
    }
}

Transaction::Transaction(Json::Value jsonValue)
{
	SetFromJson(jsonValue);
}
Transaction::Transaction(std::string jsonStr)
{
	Json::CharReaderBuilder builder;
	Json::CharReader* reader = builder.newCharReader();
	Json::Value jsonValue;
	std::string errstr;
	bool parseSuccess = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(),
		&jsonValue, &errstr);
	delete reader;

	if (!parseSuccess)
	{
		std::cout << &"ABCDBlock : Fail to parse Json On Initializing Transaction" << std::endl;
		return;
	}

	SetFromJson(jsonValue);
}

void Transaction::SetFromJson(Json::Value jsonValue)
{
	_type = (TransactionType)jsonValue["Type"].asInt();

	switch (_type)
	{
	case WalletRegistration:
		_address1 = jsonValue["UserAddress"].asString();
		break;
	case Issue:
		_address1 = jsonValue["SuperAddress"].asString();
		_address2 = jsonValue["UserAddress"].asString();
		_amount = jsonValue["Amount"].asDouble();
		break;
	case Payment:
		_address1 = jsonValue["UserAddress"].asString();
		_amount = jsonValue["Amount"].asDouble();
		break;
	case Rental:
		_address1 = jsonValue["UserAddress"].asString();
		_deviceId = jsonValue["RentalDevice"].asString();
		_time1 = jsonValue["RentalTime"].asInt64();
		_time2 = jsonValue["DueTime"].asInt64();
		_amount = jsonValue["Amount"].asDouble();
		break;
	case Return:
		_address1 = jsonValue["UserAddress"].asString();
		_deviceId = jsonValue["RentalDevice"].asString();
		_time1 = jsonValue["DueTime"].asInt64();
		_time2 = jsonValue["ReturnTime"].asInt64();
		_amount = jsonValue["Amount"].asDouble();
		break;
	}

	_timeStamp = jsonValue["TimeStamp"].asInt64();
}

TransactionType Transaction::GetTransactionType()
{
    return _type;
}
std::string Transaction::GetAddress1()
{
    return _address1;
}
std::string Transaction::GetAddress2()
{
    return _address2;
}
double Transaction::GetAmount()
{
    return _amount;
}
std::string Transaction::GetDeviceId()
{
    return _deviceId;
}
time_t Transaction::GetTime1()
{
    return _time1;
}
time_t Transaction::GetTime2()
{
    return _time2;
}
time_t Transaction::GetTimeStamp()
{
    return _timeStamp;
}

std::string Transaction::GetJson()
{
    /*std::string strJson;
    
    Json::StreamWriterBuilder builder;
    builder.settings_["identation"] = "";
    strJson = Json::writeString(builder, GetJsonValue());*/

    return GetJsonValue().toStyledString();
}
Json::Value Transaction::GetJsonValue()
{
    Json::Value root;
    
    root["Type"] = _type;
    
    switch(_type)
    {
        case WalletRegistration:
            root["UserAddress"] = _address1;
            break;
        case Issue:
            root["SuperAddress"] = _address1;
            root["UserAddress"] = _address2;
            root["Amount"] = _amount;
            break;
        case Payment:
            root["UserAddress"] = _address1;
            root["Amount"] = _amount;
            break;
        case Rental:
            root["UserAddress"] = _address1;
            root["RentalDevice"] = _deviceId;
            root["RentalTime"] = (Json::Int64) _time1;
            root["DueTime"] = (Json::Int64)_time2;
            root["Amount"] = _amount;
            break;
        case Return:
            root["UserAddress"] = _address1;
            root["RentalDevice"] = _deviceId;
            root["DueTime"] = (Json::Int64)_time1;
            root["ReturnTime"] = (Json::Int64)_time2;
            root["Amount"] = _amount;
            break;
    }

	root["TimeStamp"] = _timeStamp;

    return root;
}
