#include "BitcoinTree.hpp"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

BitcoinTreeNode::BitcoinTreeNode(char* bitcoin_id, Wallet* owner, double val)
:bitcoinID(bitcoin_id),owner(owner),value(val),left(NULL),right(NULL),parent(NULL)
{}

BitcoinTreeNode::~BitcoinTreeNode(){
  if(left != NULL)  //has left node
    delete left;
  if(right != NULL) //has right node
    delete right;
  if(parent == NULL) //if root
    free(bitcoinID);  //only root frees the bitcoinID

}

char* BitcoinTreeNode::getID() const{
  return bitcoinID;
}

Wallet* BitcoinTreeNode::get_owner() const {
  return owner;
}

//finds sum of payments of a bitCoinID
int BitcoinTreeNode::number_of_transactions() const {
  if (left != NULL && right != NULL) //if there are right and left children
    return left->number_of_transactions() + right->number_of_transactions() + 1;
  return 0;
}

//finds the unspent value of a bitcoinID that's left
double BitcoinTreeNode::unspent_value() const {
  if (right == NULL && left == NULL) //we need the last transaction
    return value;
  return right->unspent_value();
}

void BitcoinTreeNode::split(Wallet* receiver, double trans_value){
  BitcoinTreeNode* rightNode = new BitcoinTreeNode(bitcoinID,owner,value-trans_value);
  rightNode->parent = this;
  BitcoinTreeNode* leftNode = new BitcoinTreeNode(bitcoinID,receiver,trans_value);
  leftNode->parent = this;
  //update children
  this->right = rightNode;
  this->left = leftNode;
}
