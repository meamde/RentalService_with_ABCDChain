//
//  ABCDBlock.cpp
//  Private_Blockchain_forDB
//
//  Created by HwaSoek Syhn on 2018. 6. 25..
//  Copyright © 2018년 HwaSoek Syhn. All rights reserved.
//

#include "ABCDBlock.hpp"

ABCDBlock::ABCDBlock(int blockId, std::string preHash)
{
    _blockId = blockId;
    _previousHash = preHash;
    _blockLength = 0;
}
ABCDBlock::ABCDBlock(std::string jsonStr)
{
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    Json::Value jsonValue;
    std::string errstr;
    bool parseSuccess = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(),
                                      &jsonValue, &errstr);
    delete reader;
    
    if(!parseSuccess)
    {
        std::cout << &"ABCDBlock : Fail to parse Json On Initializing BlockId : " [ _blockId] << std::endl;
        return;
    }

	SetFromJson(jsonValue);
}
ABCDBlock::ABCDBlock(Json::Value jsonValue)
{
	SetFromJson(jsonValue);
}
void ABCDBlock::SetFromJson(Json::Value jsonValue)
{
    _transactionList.clear();
    Json::Value header = jsonValue["Header"];
    
    _blockId = header["BlockId"].asInt();
    _previousHash = header["PreviousHash"].asString();
    _blockLength = header["BlockLength"].asInt();
    _blockHash = header["BlockHash"].asString();
    
    Json::Value transaction = jsonValue["Transaction"];
    
    for(int index = 0; index < transaction.size(); index++)
    {
        Transaction trans(transaction[index]);
        _transactionList.push_back(trans);
    }
}
void ABCDBlock::AddTransaction(Transaction transaction)
{
	std::cout << "Transaction on Add : " << transaction.GetJsonValue() << std::endl;
	_transactionList.push_back(transaction);
}
std::list<Transaction> ABCDBlock::GetTransactionList()
{
    return _transactionList;
}
int ABCDBlock::GetBlockLength()
{
	return 0;
}
std::string ABCDBlock::GetBlockHash()
{
    std::string blockJsonStr = "";
    std::list<Transaction>::iterator it;

    for(it = _transactionList.begin(); it != _transactionList.end(); it++)
        blockJsonStr += it->GetJson();

	blockJsonStr += _previousHash;

    SHA256 sha256;
    
    std::string HasedStr = sha256(blockJsonStr.c_str(), blockJsonStr.size());
    
	return HasedStr;
}
void ABCDBlock::Determine()
{
	_blockLength = GetBlockLength();
	_blockHash = GetBlockHash();
}
std::string ABCDBlock::GetJson()
{
	/*std::string strJson;

	Json::StreamWriterBuilder builder;
	builder.settings_["identation"] = "";
	strJson = Json::writeString(builder, GetJsonValue());*/

	return GetJsonValue().toStyledString();
}
Json::Value ABCDBlock::GetJsonValue()
{
    Json::Value root;
	Json::Value header;
	Json::Value transaction;
	std::list<Transaction>::iterator tIt;
	int index = 0;

	header["BlockId"] = _blockId;
	header["PreviousHash"] = _previousHash;
	header["BlockLength"] = GetBlockLength();
	header["BlockHash"] = GetBlockHash();

	root["Header"] = header;

	std::cout << "Transaction Size : " << _transactionList.size() << std::endl;

	for (tIt = _transactionList.begin(); tIt != _transactionList.end(); tIt++)
	{
		std::cout << "this is transaction\n" << tIt->GetJsonValue() << std::endl;
		transaction[index++] = tIt->GetJsonValue();
	}

	root["Transaction"] = transaction;

    return root;
}
