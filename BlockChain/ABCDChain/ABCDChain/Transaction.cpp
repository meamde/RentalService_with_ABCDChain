//
//  Transaction.cpp
//  Private_Blockchain_forDB
//
//  Created by HwaSoek Syhn on 2018. 6. 25..
//  Copyright © 2018년 HwaSoek Syhn. All rights reserved.
//

#include "Transaction.hpp"

Transaction::Transaction(TransactionType type, std::string address1)
{
    _type = type;
    _address1 = address1;
}
Transaction::Transaction(TransactionType type, std::string address1, std::string address2, double amount)
{
    _type = type;
    _address1 = address1;
    _address2 = address2;
    _amount = amount;
}
Transaction::Transaction(TransactionType type, std::string address1, double amount)
{
    _type = type;
    _address1 = address1;
    _amount = amount;
}
Transaction::Transaction(TransactionType type, std::string address1, std::string deviceId, double amount,
                         time_t Time1, time_t Time2)
{
    _type = type;
    _address1 = address1;
    _deviceId = deviceId;
    _amount = amount;
    
    switch (type) {
        case Rental:
            _rentalTime = Time1;
            _dueTime = Time2;
            break;
        case Return:
            _dueTime = Time1;
            _returnTime = Time2;
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
		_deviceId = jsonValue["RentalDevice"].asInt();
		_rentalTime = jsonValue["RentalTime"].asInt64();
		_dueTime = jsonValue["DueTime"].asInt64();
		_amount = jsonValue["Amount"].asDouble();
		break;
	case Return:
		_address1 = jsonValue["UserAddress"].asString();
		_deviceId = jsonValue["RentalDevice"].asInt();
		_dueTime = jsonValue["DueTime"].asInt64();
		_returnTime = jsonValue["ReturnTime"].asInt64();
		_amount = jsonValue["Amount"].asDouble();
		break;
	}
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
time_t Transaction::GetRentalTime()
{
    return _rentalTime;
}
time_t Transaction::GetDueTime()
{
    return _dueTime;
}
time_t Transaction::GetReturnTime()
{
    return _returnTime;
}

std::string Transaction::GetJson()
{
    std::string strJson;
    
    Json::StreamWriterBuilder builder;
    builder.settings_["identation"] = "";
    strJson = Json::writeString(builder, GetJsonValue());
    
    return strJson;
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
            root["RentalTime"] = (Json::Int64) _rentalTime;
            root["DueTime"] = (Json::Int64)_dueTime;
            root["Amount"] = _amount;
            break;
        case Return:
            root["UserAddress"] = _address1;
            root["RentalDevice"] = _deviceId;
            root["DueTime"] = (Json::Int64) _dueTime;
            root["ReturnTime"] = (Json::Int64)_returnTime;
            root["Amount"] = _amount;
            break;
    }
    
    return root;
}
