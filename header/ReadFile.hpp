#ifndef READFILE_HPP
#define READFILE_HPP

#include <fstream>
#include "HashTable.hpp"
#include "Wallet.hpp"
#include "Transaction.hpp"
#include "BitcoinTree.hpp"
#include "TransactionList.hpp"


extern char* BitcoinBalancesFilename;
extern char* transactionsFilename;
extern float bitcoinValue;
extern int senderHashTableNumOfEntries, receiverHashTableNumOfEntries, bucketSize;

void ReadCmdArgs(int argc, char** argv);

void ReadBitcoinBalancesFile(char*,HashTable<Wallet>&,HashTable<BitcoinTreeRoot>&);

void ReadTransactionsFile(char* filename,HashTable<Transaction>& TransactionContainer,
HashTable<Wallet> &WalletContainer,HashTable<TransactionList> &SenderHashTable,
HashTable<TransactionList> &ReceiverHashtable);

/********************************Utility***************************************/

/*Get cstring from ifstream. Programmers responsiblity to free said cstring.*/
char* getStr(std::istream &data,char* last_char_read);
/*Read until you find a '\n'. Return the string ptr or NULL of fail*/
char* ReadTillNewline(std::istream &data);

/*Reads time-range format from stream.
Either as: time1 time2
			 or: time1 year1 time2 year2
In each case the value that were not found are set to NULL.*/
void getTime(std::istream &data, char** time1, char** time2, char**, char**,char*);

/*Make sure time1<time2*/
int Time1BeforeTime2(char* time1,char* time2);

/*Make sure time1<time2 and year1<year2*/
int Time1BeforeTime2(char* time1,char* year1,char* time2,char* year2);

char* CurrentTime();

char* CurrentDate();

/*Reverse date format from dd-mm-yyyy to yyyy-mm-dd*/
void ReverseDateFormat(char** date);

#endif
