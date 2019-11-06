#ifndef BITCOINTREE_HPP
#define BITCOINTREE_HPP

class Wallet;

class BitcoinTreeNode {
    char* bitcoinID;  //only for root, NULL if not root
    //left is always the receiver, right is the same owner as parent
  public:
    Wallet* owner;      //walletID of the owner
    double value;     //in USD$$$
    BitcoinTreeNode *left,*right,*parent;

    BitcoinTreeNode(char* bitcoinID, Wallet* owner, double val);
    ~BitcoinTreeNode();
    char* getID() const;
    //split a bitcoin node because a value was transfered
    void split(Wallet* receiver, double trans_value);
    Wallet* get_owner() const;
    int number_of_transactions() const;
    double unspent_value() const;
};


typedef BitcoinTreeNode BitcoinTreeRoot;

#endif
