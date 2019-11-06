#ifndef WALLET_HPP
#define WALLET_HPP

#include "BitcoinTree.hpp"
#include "Mylist.hpp"



/*A wallet contains a set of BitcoinTreeNodes. These nodes specify exactly how
much of each bitcoin a user has. Note: there mαy be one or more nodes for each
Bitcoin.*/
class Wallet {
    char* walletID; //unique
    //list of pointers to BitcoinTreeNodes (of bitcoins this wallet owns)
    ListNode<BitcoinTreeNode*>* bitcoin_nodes;
  public:
    Wallet(char* walletID);
    ~Wallet();
    void addBitcoin(BitcoinTreeNode* coin);
    char* getID() const;
    BitcoinTreeNode** Pay(Wallet* receiver,double value,int* num_used_nodes);
    void Payoff(BitcoinTreeNode** new_bitcoin_nodes, int num_nodes);
    double balance();
};


#endif
