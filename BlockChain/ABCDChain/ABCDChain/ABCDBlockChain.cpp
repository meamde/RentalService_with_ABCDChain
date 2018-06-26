//
//  main.cpp
//  Private_Blockchain_forDB
//
//  Created by HwaSeok Shin on 2018. 6. 22..
//  Copyright © 2018년 HwaSeok Shin. All rights reserved.
//

#include <iostream>
#include "ABCDBlock.hpp"
#include "Transaction.hpp"
#include "json/json.h"
#include <list>

std::list<ABCDBlock> blockList;

double GetAmountOfAddress(std::string address)
{
    std::list<ABCDBlock>::iterator blockIt;
    std::list<Transaction>::iterator transIt;
    double amount = 0;
    
    for(blockIt = blockList.begin(); blockIt != blockList.end(); blockIt++)
    {
        for(transIt = blockIt->GetTransactionList().begin();
            transIt != blockIt->GetTransactionList().end();
            transIt++)
        {
            if(transIt->GetTransactionType() == Issue &&
               transIt->GetAddress2() == address)
                amount += transIt->GetAmount();
            else if(transIt->GetTransactionType() == Payment &&
                    transIt->GetAddress1() == address)
                amount -= transIt->GetAmount();
        }
    }
    
    return amount;
}
bool TransactionVerify(Transaction transaction)
{
    std::list<ABCDBlock>::iterator blockIt;
    std::list<Transaction>::iterator transIt;
    
    double amount = 0;
    bool isRentaled = false;
	std::string RentalAddress;
    
    switch (transaction.GetTransactionType()) {
            
        case WalletRegistration:
            for(blockIt = blockList.begin(); blockIt != blockList.end(); blockIt++)
            {
                for(transIt = blockIt->GetTransactionList().begin();
                    transIt != blockIt->GetTransactionList().end();
                    transIt++)
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
                for(transIt = blockIt->GetTransactionList().begin();
                    transIt != blockIt->GetTransactionList().end();
                    transIt++)
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
                for(transIt = blockIt->GetTransactionList().begin();
                    transIt != blockIt->GetTransactionList().end();
                    transIt++)
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
                for(transIt = blockIt->GetTransactionList().begin();
                    transIt != blockIt->GetTransactionList().end();
                    transIt++)
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

int main(int argc, const char * argv[]) {
	//ABCDBlock BlockOne;


    return 0;
}