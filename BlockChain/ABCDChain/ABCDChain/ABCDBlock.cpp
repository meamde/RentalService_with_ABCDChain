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
	_nonce = 0;
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
        std::cout << &"ABCDBlock : Fail to parse Json On Initializing BlockId : " <<  _blockId << std::endl;
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
	_nonce = header["Nonce"].asUInt64();
    
    Json::Value transaction = jsonValue["Transaction"];
    
    for(int index = 0; index < transaction.size(); index++)
    {
        Transaction trans(transaction[index]);
        _transactionList.push_back(trans);
    }
}
void ABCDBlock::AddTransaction(Transaction transaction)
{
	_transactionList.push_back(transaction);
}
int ABCDBlock::GetBlockId()
{
	return _blockId;
}
std::string ABCDBlock::GetPreviousHash()
{
	return _previousHash;
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

	blockJsonStr += _nonce + "";

    SHA256 sha256;
    
    std::string HasedStr = sha256(blockJsonStr.c_str(), blockJsonStr.size());
    
	return HasedStr;
}

void ABCDBlock::Mining()
{
	while (true)
	{
		if (GetBlockHash().substr(0, 2) == "00")
		{
			std::cout << "ABCDChain : Success to mining block " << _blockId << std::endl;
			break;
		}
		_nonce++;
	}
}


void ABCDBlock::Determine()
{
	Mining();
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
	header["Nonce"] = _nonce;

	root["Header"] = header;

	for (tIt = _transactionList.begin(); tIt != _transactionList.end(); tIt++)
	{
		transaction[index++] = tIt->GetJsonValue();
	}

	root["Transaction"] = transaction;

    return root;
}
