//
//  main.cpp
//  Private_Blockchain_forDB
//
//  Created by HwaSeok Shin on 2018. 6. 22..
//  Copyright © 2018년 HwaSeok Shin. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <list>

#include "ABCDBlock.hpp"
#include "Transaction.hpp"
#include "json/json.h"

std::list<ABCDBlock*> blockList;
std::list<std::string> nodeAdressList;

double GetAmountOfAddress(std::string address)
{
    std::list<ABCDBlock*>::iterator blockIt;
    std::list<Transaction>::iterator transIt;
	std::list<Transaction> transList;
    double amount = 0;
    
    for(blockIt = blockList.begin(); blockIt != blockList.end(); blockIt++)
    {
		transList = (*blockIt)->GetTransactionList();

		for (transIt = transList.begin(); transIt != transList.end(); transIt++)
		{
			if (transIt->GetTransactionType() == Issue &&
				transIt->GetAddress2() == address)
				amount += transIt->GetAmount();
			else if (transIt->GetTransactionType() == Payment &&
				transIt->GetAddress1() == address)
				amount -= transIt->GetAmount();
		}
    }
    
    return amount;
}
bool TransactionVerify(Transaction transaction)
{
    std::list<ABCDBlock*>::iterator blockIt;
    std::list<Transaction>::iterator transIt;
	std::list<Transaction> transList;
    
    double amount = 0;
    bool isRentaled = false;
	std::string RentalAddress;
    
    switch (transaction.GetTransactionType()) {
            
        case WalletRegistration:
            for(blockIt = blockList.begin(); blockIt != blockList.end(); blockIt++)
            {
				transList = (*blockIt)->GetTransactionList();

				for (transIt = transList.begin(); transIt != transList.end(); transIt++)
				{
                    if(transIt->GetTransactionType() == WalletRegistration &&
                       transIt->GetAddress1() == transaction.GetAddress1())
                        return false;
                }
            }
            return true;
        
        case Issue:
            for(blockIt = blockList.begin(); blockIt != blockList.end(); blockIt++)
            {
				transList = (*blockIt)->GetTransactionList();

				for (transIt = transList.begin(); transIt != transList.end(); transIt++)
				{
                    if(transIt->GetTransactionType() == WalletRegistration &&
                       transIt->GetAddress1() == transaction.GetAddress1())
                        return true;
                }
            }
            return false;
            
        case Payment:
            return GetAmountOfAddress(transaction.GetAddress1()) >= transaction.GetAmount();
            
        case Rental:
            
            for(blockIt = blockList.begin(); blockIt != blockList.end(); blockIt++)
            {
				transList = (*blockIt)->GetTransactionList();

				for (transIt = transList.begin(); transIt != transList.end(); transIt++)
				{
                    if(transIt->GetTransactionType() == Issue &&
                       transIt->GetAddress2() == transaction.GetAddress1())
                    {
                        amount += transIt->GetAmount();
                    }
                    else if(transIt->GetTransactionType() == Payment &&
                            transIt->GetAddress1() == transaction.GetAddress1())
                    {
                        amount -= transIt->GetAmount();
                    }
                    
                    if(transIt->GetTransactionType() == Rental &&
                       transIt->GetDeviceId() == transaction.GetDeviceId())
                    {
                        isRentaled = true;
                    }
                    else if(transIt->GetTransactionType() == Return &&
                            transIt->GetDeviceId() == transaction.GetDeviceId())
                    {
                        isRentaled = false;
                    }
                }
            }
            
            if(!isRentaled && amount >= transaction.GetAmount())
                return true;
            else
                return false;
            
        case Return:
            
            for(blockIt = blockList.begin(); blockIt != blockList.end(); blockIt++)
            {
				transList = (*blockIt)->GetTransactionList();

				for (transIt = transList.begin(); transIt != transList.end(); transIt++)
				{
                    if(transIt->GetTransactionType() == Issue &&
                       transIt->GetAddress2() == transaction.GetAddress1())
                    {
                        amount += transIt->GetAmount();
                    }
                    else if(transIt->GetTransactionType() == Payment &&
                            transIt->GetAddress1() == transaction.GetAddress1())
                    {
                        amount -= transIt->GetAmount();
                    }
                    
                    if(transIt->GetTransactionType() == Rental &&
                       transIt->GetDeviceId() == transaction.GetDeviceId())
                    {
						RentalAddress = transIt->GetAddress1();
                        isRentaled = true;
                    }
                    else if(transIt->GetTransactionType() == Return &&
                            transIt->GetDeviceId() == transaction.GetDeviceId())
                    {
                        isRentaled = false;
                    }
                }
            }
            
            if(isRentaled && RentalAddress == transaction.GetAddress1())// && amount >= transaction.GetAmount())
                return true;
            else
                return false;
            
            break;
    }
    
    return false;
}
bool BlockVerify()
{
	std::list<ABCDBlock*>::iterator nowIt;
	std::list<ABCDBlock*>::iterator preIt;

	for (nowIt = blockList.begin(); nowIt != blockList.end(); nowIt++)
	{
		if ((*nowIt)->GetBlockId() == 0)
		{
			preIt = nowIt;
			continue;
		}

		if ((*nowIt)->GetPreviousHash() != (*preIt)->GetBlockHash())
			return false;

		preIt = nowIt;
	}

	return true;
}
bool isRentaled(std::string deviceId)
{
	return true;
}

Json::Value GetBlockChainJsonValue()
{
    Json::Value root;
    int index = 0;
    std::list<ABCDBlock*>::iterator it;
    for(it = blockList.begin(); it != blockList.end(); it++)
        root[index++] = (*it)->GetJsonValue();
    
    return root;
}



void WriteBlockToFile()
{
	std::ofstream outFile;
	outFile.open("ABCDBlockNode.json");
    
    outFile << GetBlockChainJsonValue();
	//std::cout << GetBlockChainJsonValue();
	std::cout << "Complete!" << std::endl;

	outFile.close();
}
bool ReadBlockFromFile()
{
	std::ifstream inFile;
	inFile.open("ABCDBlockNode.json");

	if (inFile.fail())
	{
		std::cout << "ABCDChain : No block node on this program's directory" << std::endl;
		return false;
	}

	Json::Value jsonValue;

	inFile >> jsonValue;
	
	for (int i = 0; i < jsonValue.size(); i++)
	{
		ABCDBlock *tmpBlock = new ABCDBlock(jsonValue[i]);
		blockList.push_back(tmpBlock);
	}

	return true;
}

int main(int argc, const char * argv[]) {

	ABCDBlock GenesisBlock(0, "");
    blockList.push_back(&GenesisBlock);
    GenesisBlock.Determine();
    
	ABCDBlock Block1(blockList.back()->GetBlockId() + 1 , blockList.back()->GetBlockHash());
    blockList.push_back(&Block1);
    blockList.back()->AddTransaction(Transaction(WalletRegistration, "dsadfwegah29i31090sdfsd89a0gs890a", time(0)));
	blockList.back()->Determine();
	
	std::cout << "BlockVerify : " << (BlockVerify() ? "True" : "False") << std::endl;

	ABCDBlock Block2(blockList.back()->GetBlockId() + 1, blockList.back()->GetBlockHash());
	blockList.push_back(&Block2);
	blockList.back()->AddTransaction(Transaction(Issue, "SUUPPPPPEEERRRRkey","dsadfwegah29i31090sdfsd89a0gs890a", 1000, time(0)));
	blockList.back()->Determine();

	std::cout << "BlockVerify : " << (BlockVerify() ? "True" : "False") << std::endl;

	ABCDBlock Block3(blockList.back()->GetBlockId() + 1, blockList.back()->GetBlockHash());
	blockList.push_back(&Block3);
	blockList.back()->AddTransaction(Transaction(Payment, "dsadfwegah29i31090sdfsd89a0gs890a", 30, time(0)));
	blockList.back()->Determine();

	std::cout << "BlockVerify : " << (BlockVerify() ? "True" : "False") << std::endl;

	ABCDBlock Block4(blockList.back()->GetBlockId() + 1, blockList.back()->GetBlockHash());
	blockList.push_back(&Block4);
	blockList.back()->AddTransaction(Transaction(Rental, "dsadfwegah29i31090sdfsd89a0gs890a", "Device1", 50, time(0), time(0) + 60*60*24*3, time(0)));
	blockList.back()->Determine();

	std::cout << "BlockVerify : " << (BlockVerify() ? "True" : "False") << std::endl;

	ABCDBlock Block5(blockList.back()->GetBlockId() + 1, blockList.back()->GetBlockHash());
	blockList.push_back(&Block5);
	blockList.back()->AddTransaction(Transaction(Return, "dsadfwegah29i31090sdfsd89a0gs890a", "Device1", 50, time(0) + 60 * 60 * 24 * 3, time(0), time(0)));
	blockList.back()->Determine();

	std::cout << "BlockVerify : " << (BlockVerify() ? "True" : "False") << std::endl;
    
	std::cout << "Amount : " << GetAmountOfAddress("dsadfwegah29i31090sdfsd89a0gs890a") << std::endl;

    WriteBlockToFile();

    int a;
    std::cin>>a;
    
    return 0;
}
