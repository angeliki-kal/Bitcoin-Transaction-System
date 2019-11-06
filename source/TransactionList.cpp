#include "TransactionList.hpp"

TransactionList::TransactionList(Wallet* w, Transaction* transaction)
:wallet(w){
  transaction_list = new ListNode<Transaction*>(transaction);
}

TransactionList::~TransactionList(){
  transaction_list->DeleteList();
}

char* TransactionList::getID() const{
  return wallet->getID();
}

void TransactionList::Insert(Transaction* transaction){
  transaction_list->Insert(transaction);
}

void UpdateSenderReceiverLists(HashTable<TransactionList> &SenderHashTable,
HashTable<TransactionList> &ReceiverHashTable,Transaction* transaction){
  Wallet* sender = transaction->getSender();
  Wallet* receiver = transaction->getReceiver();
  //search for this users transaction list on HashTable
  TransactionList* sender_transaction_list = SenderHashTable.Find(sender->getID());
  if(sender_transaction_list == NULL) //list not found, create a new one and insert
    SenderHashTable.Insert(sender->getID(),new TransactionList(sender,transaction));
  else  //list found, add a transaction to it
    sender_transaction_list->Insert(transaction);
  TransactionList* receiver_transaction_list = ReceiverHashTable.Find(receiver->getID());
  if(receiver_transaction_list == NULL) //list not found, create a new one and insert
    ReceiverHashTable.Insert(receiver->getID(),new TransactionList(receiver,transaction));
  else  //list found, add a transaction to it
    receiver_transaction_list->Insert(transaction);
}
