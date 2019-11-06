#ifndef TRANSACTIONHASHTABLE_HPP
#define TRANSACTIONHASHTABLE_HPP

#include <cstring>
#include <cstdlib>
#include <iostream>

template<class Record>
class Bucket {
    int max_num_records, num_records;
    Bucket<Record> *nextBucket; //next bucket of the chain
    Record** Records;    //this buckets records
  public:
    //Constructor
    Bucket(int b_size)
    :max_num_records(b_size/sizeof(Record*)),num_records(0),nextBucket(NULL){
      Records = (Record**)malloc(max_num_records*sizeof(Record*));
    };

    //Destructor
    ~Bucket(){
      //delete next bucket
      if(nextBucket != NULL)
        delete nextBucket;
      //delete records
      for(int i=0; i<num_records; i++){
        delete Records[i];
      }
      //free Record array
      free(Records);
    };

    //Insert record to bucket
    void Insert(char* recID, Record* rec){
      if(num_records < max_num_records){
        Records[num_records++] = rec;
      }
      else{
        //overflow, create next Bucket
        if(nextBucket == NULL)
          nextBucket = new Bucket(max_num_records*sizeof(Record*));
        nextBucket->Insert(recID,rec);
      }
    };
    //Find and return record in bucket chain. NULL if not found
    Record* Find(char* recID){
      for(int i=0; i<num_records; i++){
        if(strcmp(recID,Records[i]->getID())==0){
          return Records[i];
        }
      }
      //not in this bucket, search next
      if(nextBucket == NULL)
        return NULL;
      else
        nextBucket->Find(recID);
    };
    //Print buckets contents
    void print(){
      std::cout << "\t\t" << num_records << " records" << std::endl;
      for(int i=0; i<max_num_records; i++){
        if(i>=num_records){
          std::cout << "\t\t" << i <<  ")---" << std::endl;
        }
        else{
          std::cout << "\t\t" << i << ")";
          printf("%s\n", Records[i]->getID());
        }
      }
      if(nextBucket != NULL){
        std::cout << "\tOverflow Bucket with:" << std::endl;
        nextBucket->print();
      }
    };
};




template<class Record>
class HashTable {
    int table_size;   //total num of buckets in hashtable
    Bucket<Record> **Buckets;  //all the buckets of the hashtable
  public:
    //Constructor
    HashTable(int t_size, int bucket_size)
    :table_size(t_size)
    {
      //init buckets
      Buckets = (Bucket<Record>**)malloc(table_size*sizeof(Bucket<Record>*));
      for(int i=0; i<table_size; i++){
        Buckets[i] = new Bucket<Record>(bucket_size);
      }
    };

    //Destructor
    ~HashTable(){
      for(int i=0; i<table_size; i++)
        delete Buckets[i];
      free(Buckets);
    };

    //https://en.wikipedia.org/wiki/Jenkins_hash_function
    uint32_t HashFunction(char* recordID){
      size_t length = strlen(recordID);
      size_t i = 0;
      uint32_t hash = 0;
      while (i != length) {
        hash += recordID[i++];  //add every char of recordID
        hash += hash << 10;
        hash ^= hash >> 6;
      }
      hash += hash << 3;
      hash ^= hash >> 11;
      hash += hash << 15;
      return hash%table_size;
    };

    //Find and return transaction list of walletID
    Record* Find(char* recordID){
      Buckets[HashFunction(recordID)]->Find(recordID);
    };

    //Insert new record
    int Insert(char* recID, Record* rec){
      //check if record already in HashTable
      if(Find(recID) != NULL)
        return -1;
      Buckets[HashFunction(recID)]->Insert(recID,rec);
      return 0; //success
    };

    //Print conents of each bucket
    void print(){
      std::cout << "Hashtable with " << table_size << " buckets." << std::endl;
      for(int i=0; i<table_size; i++){
        std::cout << "\tBucket" << i << " with:" << std::endl;
        Buckets[i]->print();
      }
    };
};



#endif
