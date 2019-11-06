#include "Wallet.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;

Wallet::Wallet(char* wID)
:walletID(wID),bitcoin_nodes(NULL){}

Wallet::~Wallet(){
  free(walletID);
  if(bitcoin_nodes != NULL)
    bitcoin_nodes->DeleteList();
}

void Wallet::addBitcoin(BitcoinTreeNode* coin){
  if(bitcoin_nodes != NULL)
    bitcoin_nodes->Insert(coin);
  else
    bitcoin_nodes = new ListNode<BitcoinTreeNode*>(coin);
}

char* Wallet::getID() const {
  return walletID;
}

BitcoinTreeNode** Wallet::Pay(Wallet* receiver,double value, int* num_used_nodes){
  //choose which TransactionsNodes to split
  *num_used_nodes = 0;
  BitcoinTreeNode** used_nodes = NULL;  //used bitcoins by transaction

  if(balance() < value){  //not enough money
    return NULL;
  }
  ListNode<BitcoinTreeNode*>* curr_node = bitcoin_nodes;
  while(value > 0 && curr_node!=NULL){  //got cash, lets pay!
    BitcoinTreeNode* bitcoin = curr_node->data;
    if(bitcoin->value > value){
      //this bitcoin can cover the value amount and still have spare left
      bitcoin->split(receiver,value); //split bitcoin to receiver and owner
      value = 0;
      //replace bitcoin node with the one that was produced by the split
      curr_node->data = bitcoin->right;
      //remember the bitcoin that was used
      used_nodes = (BitcoinTreeNode**)realloc(used_nodes,(*num_used_nodes+1)*sizeof(BitcoinTreeNode*));
      used_nodes[(*num_used_nodes)++] = bitcoin;
    }
    else{
      /*this bitcoin covers only a partial or equal of total amount of value
       and will be spent whole in order to cover it*/
      bitcoin->split(receiver,bitcoin->value); //send the whole bitcoin to receiver
      value -= bitcoin->value;             //remainder value
      //this wallet no longer has this bitcoin, delete it from the wallet list
      bitcoin_nodes = curr_node->Delete();
      //remember the bitcoin that was used
      used_nodes = (BitcoinTreeNode**)realloc(used_nodes,(*num_used_nodes+1)*sizeof(BitcoinTreeNode*));
      used_nodes[(*num_used_nodes)++] = bitcoin;
    }
  }
  //transfer bitcoins to receiver
  receiver->Payoff(used_nodes,*num_used_nodes);
  return used_nodes;
}

void Wallet::Payoff(BitcoinTreeNode** used_nodes, int num_nodes){
  for(int i=0; i<num_nodes; i++){
    addBitcoin(used_nodes[i]->left);
  }
}


//Go through bitcoin_nodes list, and add sum of bitcoin values
double Wallet::balance(){
  double sum = 0;
  ListNode<BitcoinTreeNode*>* curr_node = bitcoin_nodes;
  while (curr_node != NULL){
    sum += curr_node->data->value;
    curr_node = curr_node->next;
  }
  return sum;
}
