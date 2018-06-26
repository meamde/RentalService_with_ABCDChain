//
//  ABCDBlock.hpp
//  Private_Blockchain_forDB
//
//  Created by HwaSoek Syhn on 2018. 6. 25..
//  Copyright © 2018년 HwaSoek Syhn. All rights reserved.
//

#ifndef ABCDBlock_hpp
#define ABCDBlock_hpp

#include "sha256.h"
#include <iostream>
#include <list>
#include "Transaction.hpp"
#include "json/json.h"


#define TRANSACTION_COUNT 1

class ABCDBlock {
private:
    int _blockId;
    std::string _previousHash;
    std::string _blockHash;
    int _blockLength;
	Json::UInt64 _nonce;
    std::list<Transaction> _transactionList;
    
public:
    ABCDBlock(int blockId, std::string preHash);
    ABCDBlock(std::string jsonStr);
    ABCDBlock(Json::Value jsonValue);
    void SetFromJson(Json::Value jsonValue);
    void AddTransaction(Transaction transaction);
	int GetBlockId();
	std::string GetPreviousHash();
    std::list<Transaction> GetTransactionList();
	int GetBlockLength();
	std::string GetBlockHash();
	void Mining();
    void Determine();
    std::string GetJson();
    Json::Value GetJsonValue();
};
#endif /* ABCDBlock_hpp */
