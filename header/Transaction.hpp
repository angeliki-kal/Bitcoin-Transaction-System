#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include "Wallet.hpp"
#include "BitcoinTree.hpp"
#include "HashTable.hpp"

/*A transaction is a set of BitcoinTreeNodes that participated in a bitcoin
value transfer between wallets. Each of these nodes is broken up into two others,
one indicating the receiver and one the remaining value of the bitcoin.*/
class Transaction {
    char* transactionID;
    //bitcoin nodes that participated in this transaction
    int num_transaction_nodes;
    BitcoinTreeNode** TransactionNodes;
  public:
    char* date;
    char* time;
    Transaction(char* id,BitcoinTreeNode** Nodes, int num_nodes);
    Transaction(char* id,char* date,char* time,BitcoinTreeNode** Nodes,int num_nodes);
    ~Transaction();
    char* getID() const;
    //sum of left child value for all TransactionNodes
    double value();
    Wallet* getSender();
    Wallet* getReceiver();
    int HasBitcoinNode(BitcoinTreeNode* node_ptr);
    void print();
};

//Receive a request for a transaction and if approved create a Transaction object
Transaction* CompleteTransaction(char* transactionID,char* date,char* time,
char* senderID,char* receiverID,double value,HashTable<Wallet> &WalletContainer,
HashTable<Transaction> &TransactionContainer);
//overloaded, missing date time
Transaction* CompleteTransaction(char* transactionID,
char* senderID,char* receiverID,double value,HashTable<Wallet> &WalletContainer,
HashTable<Transaction> &TransactionContainer);


int ReadTransaction(std::istream&, char**, char**, char**, char**, char**, char*);
int ReadTransaction(std::istream&, char**, char**, char**, char**, char*);

char* Generate_new_transactionID(HashTable<Transaction> &TransactionContainer);
#endif
