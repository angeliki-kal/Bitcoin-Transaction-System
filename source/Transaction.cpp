#include "Transaction.hpp"
#include "ReadFile.hpp"
#include <ctime>
#include <cstdio>

using namespace std;

Transaction* latest_transaction=NULL;

Transaction::Transaction(char* id,BitcoinTreeNode** Nodes, int num_nodes)
:transactionID(id),date(CurrentDate()),time(CurrentTime()),num_transaction_nodes(num_nodes),TransactionNodes(Nodes)
{
  latest_transaction = this;
  putc('\t',stdout);
  this->print();
}


Transaction::Transaction(char* id,char* date,char* time,BitcoinTreeNode** Nodes, int num_nodes)
:transactionID(id),date(date),time(time),num_transaction_nodes(num_nodes),TransactionNodes(Nodes)
{
  latest_transaction = this;
  putc('\t',stdout);
  this->print();
}

Transaction::~Transaction(){
  free(transactionID);
  free(date);
  free(time);
  //free transaction nodes array, but not the nodes themselves
  free(TransactionNodes);
}

char* Transaction::getID() const{
  return transactionID;
}

//sum of left child value for all TransactionNodes of this transaction
double Transaction::value(){
  int i;
  double sum = 0;
  for(i=0; i<num_transaction_nodes; i++){
      sum += TransactionNodes[i]->left->value;
  }
  return sum;
}

Wallet* Transaction::getSender(){
  return TransactionNodes[0]->get_owner();
}

Wallet* Transaction::getReceiver(){
  return TransactionNodes[0]->left->get_owner();
}

int Transaction::HasBitcoinNode(BitcoinTreeNode* node_ptr){
  for(int i=0; i<num_transaction_nodes; i++){
    if(TransactionNodes[i] == node_ptr)
      return 1; //found it
  }
  return 0; //not found
}

void Transaction::print(){
  printf("%s %s %s %f %s %s\n", transactionID, getSender()->getID(), getReceiver()->getID(), value(),date,time);
}

Transaction* CompleteTransaction(char* transactionID,
char* senderID,char* receiverID,double value,HashTable<Wallet> &WalletContainer,
HashTable<Transaction> &TransactionContainer){
  if(TransactionContainer.Find(transactionID) != NULL){
    cout << "Transaction: " << transactionID << "already exists. Aborting" << endl;
    return NULL;
  }
  if(value <= 0){
    cout << "Transaction" << transactionID << " aborted. Invalid value: " << value << "$" << endl;
    return NULL;
  }
  if(strcmp(senderID,receiverID)==0){
    cout << "Transaction" << transactionID << " aborted. Sender and Receiver must be different." << endl;
    return NULL;
  }
  //find Wallet objects by ID
  Wallet* sender = WalletContainer.Find(senderID);
  Wallet* receiver = WalletContainer.Find(receiverID);
  if(sender == NULL){
    cout << "Transaction" << transactionID << " aborted. Sender: " << senderID << " not existent" << endl;
    return NULL;
  }
  if(receiver == NULL){
    cout << "Transaction" << transactionID << " aborted. Receiver: " << receiverID << " not existent" << endl;
    return NULL;
  }
  //sender will pay the receiver and return the bitcoin nodes he used to pay with
  int num_transaction_nodes;
  BitcoinTreeNode** used_nodes = sender->Pay(receiver,value,&num_transaction_nodes);
  if(used_nodes == NULL){
    cout << "Transaction" << transactionID << " aborted. Sender couldn't pay. value:" << value << " balance:" <<  sender->balance() << endl;
    return NULL;
  }
  //create and insert transaction
  Transaction* transaction = new Transaction(transactionID,used_nodes,num_transaction_nodes);
  TransactionContainer.Insert(transactionID,transaction);
  return transaction;
}


Transaction* CompleteTransaction(char* transactionID,char* date,char* time,
char* senderID,char* receiverID,double value,HashTable<Wallet> &WalletContainer,
HashTable<Transaction> &TransactionContainer){
  if(TransactionContainer.Find(transactionID) != NULL){
    cout << "Transaction: " << transactionID << "already exists. Aborting" << endl;
    return NULL;
  }
  //check if date time is after the latest transaction
  if(latest_transaction != NULL){
    if(!Time1BeforeTime2(latest_transaction->time,latest_transaction->date,time,date)){
      printf("Transaction %s time must be after the latest_transaction time\n", transactionID);
      return NULL;
    }
  }
  if(value <= 0){
    cout << "Transaction" << transactionID << " aborted. Invalid value: " << value << "$" << endl;
    return NULL;
  }
  if(strcmp(senderID,receiverID)==0){
    cout << "Transaction" << transactionID << " aborted. Sender and Receiver must be different." << endl;
    return NULL;
  }
  //find Wallet objects by ID
  Wallet* sender = WalletContainer.Find(senderID);
  Wallet* receiver = WalletContainer.Find(receiverID);
  if(sender == NULL){
    cout << "Transaction" << transactionID << " aborted. Sender: " << senderID << " not existent" << endl;
    return NULL;
  }
  if(receiver == NULL){
    cout << "Transaction" << transactionID << " aborted. Receiver: " << receiverID << " not existent" << endl;
    return NULL;
  }
  //sender will pay the receiver and return the bitcoin nodes he used to pay with
  int num_transaction_nodes;
  BitcoinTreeNode** used_nodes = sender->Pay(receiver,value,&num_transaction_nodes);
  if(used_nodes == NULL){
    cout << "Transaction" << transactionID << " aborted. Sender couldn't pay. value:" << value << " balance:" <<  sender->balance() << endl;
    return NULL;
  }
  //create and insert transaction
  Transaction* transaction = new Transaction(transactionID,date,time,used_nodes,num_transaction_nodes);
  TransactionContainer.Insert(transactionID,transaction);
  return transaction;
}

int ReadTransaction(istream &data, char** senderID,char** receiverID,char** amount,char** date,char** time,char* last_char_read){
  *senderID = getStr(data,last_char_read);
  if(*senderID == NULL || (*last_char_read!=' ' && *last_char_read!=';')){
    printf("Bad formatting for Transaction at senderID\n");
    return -1;
  }
  *receiverID = getStr(data,last_char_read);
  if(*receiverID == NULL || (*last_char_read!=' ' && *last_char_read!=';')){
    printf("Bad formatting for Transaction at receiverID\n");
    free(*senderID);
    return -2;
  }
  *amount = getStr(data,last_char_read);
  if(*amount == NULL){
    printf("Bad formatting for Transaction at amount\n");
    free(*senderID);
    free(*receiverID);
    return -3;
  }
  if(*last_char_read!=';' && *last_char_read != '\n'){
    //date time are optional no need to check
    *date = getStr(data,last_char_read);
    ReverseDateFormat(date);
    *time = getStr(data,last_char_read);
  }
  else{
    *date = NULL;
    *time = NULL;
  }
  return 0;
}

int ReadTransaction(istream &data,char** receiverID,char** amount,char** date,char** time,char* last_char_read){
  *receiverID = getStr(data,last_char_read);
  if(*receiverID == NULL || (*last_char_read!=' ' && *last_char_read!=';')){
    printf("Bad formatting for Transaction at receiverID\n");
    return -2;
  }
  *amount = getStr(data,last_char_read);
  if(*amount == NULL){
    printf("Bad formatting for Transaction at amount\n");
    free(*receiverID);
    return -3;
  }
  if(*last_char_read!=';' && *last_char_read!='\n'){
    //date time are optional no need to check
    *date = getStr(data,last_char_read);
    ReverseDateFormat(date);
    *time = getStr(data,last_char_read);
  }
  else{
    *date = NULL;
    *time = NULL;
  }
  return 0;
}

char* Generate_new_transactionID(HashTable<Transaction> &TransactionContainer){
  //create new id randomly
  int id_size=12;
  char* new_id = (char*)malloc((id_size+1)*sizeof(char));
  new_id[id_size] = '\0';
  for(int i=0; i<id_size; i++){
    new_id[i] = '0'+rand()%10;
  }
  //check that the new_id doesn't already exist
  while(TransactionContainer.Find(new_id)!=NULL){
    //if it exists then change a random character of the new id
    new_id[rand()%id_size] = '0'+rand()%10;
  }
  return new_id;
}
