#ifndef TRANSACTIONLIST_HPP
#define TRANSACTIONLIST_HPP

#include "Transaction.hpp"
#include "Mylist.hpp"
#include "Wallet.hpp"

class TransactionList {
    Wallet* wallet; //sender or receiver
  public:
    ListNode<Transaction*>* transaction_list; //first transaction of the list
    TransactionList(Wallet*, Transaction*);
    ~TransactionList();
    void Insert(Transaction* transaction);
    char* getID() const;
};

void UpdateSenderReceiverLists(HashTable<TransactionList> &SenderHashTable,
HashTable<TransactionList> &ReceiverHashtable, Transaction* transaction);

#endif
