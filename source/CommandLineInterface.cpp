#include "CommandLineInterface.hpp"
#include "ReadFile.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>

using namespace std;

void CommandLineInterface(HashTable<Wallet> &WalletContainer,
HashTable<Transaction> &TransactionContainer,
HashTable<BitcoinTreeRoot> &BitcoinContainer,
HashTable<TransactionList> &SenderTransactionListHashTable,
HashTable<TransactionList> &ReceiverTransactionListHashTable){
	char last_char_read,*command_str;
	cout << "CommandLineInterface open." << endl << "$";
  //read command_string and determine which function to call for the command
	while((command_str = getStr(cin,&last_char_read)) != NULL){
		if(!strcmp(command_str,"/requestTransaction")){
			requestTransaction(WalletContainer,TransactionContainer,
			SenderTransactionListHashTable,ReceiverTransactionListHashTable);
			free(command_str);
			cout << "$";
			continue;
		}
		if(!strcmp(command_str,"/findEarnings")){
			if(last_char_read == '\n')
				printf("Bad formating for findEarnings.\n");
			else
				findPayments(ReceiverTransactionListHashTable);
			free(command_str);
			cout << "$";
			continue;
		}
		if(!strcmp(command_str,"/findPayments")){
			if(last_char_read == '\n')
				printf("Bad formating for findPayments.\n");
			else
				findPayments(SenderTransactionListHashTable);
			free(command_str);
			cout << "$";
			continue;
		}
		if(!strcmp(command_str,"/walletStatus")){
			if(last_char_read == '\n')
				printf("Bad formating for walletStatus.\n");
			else
				walletStatus(WalletContainer);
			free(command_str);
			cout << "$";
			continue;
		}
		if(!strcmp(command_str,"/bitCoinStatus")){
			if(last_char_read == '\n')
				printf("Bad formating for bitCoinStatus.\n");
			else
				bitCoinStatus(BitcoinContainer);
			free(command_str);
			cout << "$";
			continue;
		}
		if(!strcmp(command_str,"/traceCoin")){
			if(last_char_read == '\n')
				printf("Bad formatting for traceCoin.\n");
			else
				traceCoin(BitcoinContainer,SenderTransactionListHashTable);
			free(command_str);
			cout << "$";
			continue;
		}
		if(!strcmp(command_str,"/exit")){
			printf("exiting...\n");
			free(command_str);
			break;
		}
		//didn't match any command. Read junk until newline
		printf("Invalid command: \"%s", command_str);
		if(last_char_read != '\n'){
			char* temp = ReadTillNewline(cin);
			if(temp != NULL){
				printf(" %s", temp);
				free(temp);
			}
		}
		printf("\". Try again\n");
		cout << "$";
		free(command_str);
	}
}

void requestTransaction(HashTable<Wallet> &WalletContainer,
HashTable<Transaction> &TransactionContainer,
HashTable<TransactionList> &SenderHashTable,
HashTable<TransactionList> &ReceiverHashTable){
  char *str, *senderID, *receiverID, *amount, *date, *time, last_char_read;
  int first_run=1;
  if((str = getStr(cin,&last_char_read)) == NULL){
    printf("Bad formatting for /requestTransaction\n");
    return;
  }

  switch (last_char_read) {
    case '\n':  //transaction filename from command line
                ReadTransactionsFile(str,TransactionContainer,WalletContainer,
                SenderHashTable,ReceiverHashTable);
                free(str);
                break;
    default:    //handwritten transaction from command line
                do{
                  if(first_run){
										first_run = 0;
                    senderID=str;
										if(ReadTransaction(cin,&receiverID,&amount,&date,&time,&last_char_read)!=0){
											free(senderID);
											break; //bad formatting for transaction
										}
                  }
                  else
                  	if(ReadTransaction(cin,&senderID,&receiverID,&amount,&date,&time,&last_char_read)!=0)
											break;	//bad formatting for transaction
                  //got all strings, now try to implement transaction
									char* transactionID = Generate_new_transactionID(TransactionContainer);
									Transaction* transaction;
									if(date==NULL || time==NULL){
										transaction = CompleteTransaction(transactionID,
											senderID,receiverID,(double)atof(amount),
											WalletContainer,TransactionContainer);
									}
									else{
										transaction = CompleteTransaction(transactionID,date,time,
											senderID,receiverID,(double)atof(amount),
											WalletContainer,TransactionContainer);
									}
							    if(transaction != NULL){
							      //transaction successfull, update sender and receiver lists
							      UpdateSenderReceiverLists(SenderHashTable,ReceiverHashTable,transaction);
										//cleanup
										free(senderID);
										free(receiverID);
										free(amount);
							    }
							    else{
							      //transaction failed, delete strings
							      free(transactionID);
										if(date != NULL) free(date);
							      if(time != NULL) free(time);
										free(senderID);
	                  free(receiverID);
	                  free(amount);
										break;
							    }
                }while(last_char_read==';');
  }//switch stmt end
}

void findEarnings(HashTable<TransactionList> &ReceiverHashTable){
	//reads walletID from cin
	char last_char_read;
	char* walletID = getStr(cin,&last_char_read);
	if (walletID == NULL){
		printf("Bad formatting for /findEarnings\n");
		return;
	}
	//find transaction list from receiver hashtable
	double sum = 0;
	TransactionList* ReceiverTransactionList = ReceiverHashTable.Find(walletID);
	free(walletID);
	if(ReceiverTransactionList == NULL){
		printf("Earnings: %f$ \n", sum);
		return;
	}
	//traverse list, find and add values
  ListNode<Transaction*>* curr_node = ReceiverTransactionList->transaction_list;
  while (curr_node != NULL){
    sum += curr_node->data->value();
    curr_node = curr_node->next;
  }
	printf("Earnings: %f$ \n", sum);
}

void findPayments(HashTable<TransactionList> &SenderHashTable){
	//reads walletID from cin
	char last_char_read;
	char* walletID = getStr(cin,&last_char_read);
	if (walletID == NULL){
		printf("Bad formatting for /findPayments\n");
		return;
	}
	//find transaction list from receiver hashtable
	double sum = 0;
	TransactionList* SendTransactionList = SenderHashTable.Find(walletID);
	free(walletID);
	if(SendTransactionList == NULL){
		printf("Payments: %f$ \n", sum);
		return;
	}
	//traverse list, find and add values
  ListNode<Transaction*>* curr_node = SendTransactionList->transaction_list;
  while (curr_node != NULL){
    sum += curr_node->data->value();
    curr_node = curr_node->next;
  }
	printf("Payments: %f$ \n", sum);
}

void walletStatus(HashTable<Wallet> &WalletContainer){
	char last_char_read;
	char* walletID = getStr(cin,&last_char_read);
	if (walletID == NULL){
		printf("Bad formatting for /walletStatus\n");
		return;
	}
	else{
		Wallet* wallet = WalletContainer.Find(walletID);
		if (wallet == NULL){
			printf("This walletID doesn't exist.\n");
			return;
		}
		double sum = wallet->balance();
		printf("The wallet status is:%f$\n", sum);
		free(walletID);
	}
}

void bitCoinStatus(HashTable<BitcoinTreeRoot> &BitcoinContainer){
	char last_char_read;
	char* bitcoinID = getStr(cin,&last_char_read);
	if (bitcoinID == NULL){
		printf("Bad formatting for /bitCoinStatus\n");
		return;
	}
	BitcoinTreeRoot* bitcoin = BitcoinContainer.Find(bitcoinID);
	if (bitcoin == NULL){
		printf("This bitCoinID doesn't exist.\n");
		return;
	}
	printf("%s %f$ %d %f$\n", bitcoinID, bitcoin->value,bitcoin->number_of_transactions(), bitcoin->unspent_value());
	free (bitcoinID);
}

void traceCoin(HashTable<BitcoinTreeRoot> &BitcoinContainer,
HashTable<TransactionList> &SenderTransactionListHashTable){
	char last_char_read;
	//get bitcoinID
	char* bitcoinID = getStr(cin,&last_char_read);
	if (bitcoinID == NULL){
		printf("Bad formatting for /bitCoinStatus\n");
		return;
	}
	//get bitcoin root node by IDs
	BitcoinTreeRoot* bitcoin = BitcoinContainer.Find(bitcoinID);
	if(bitcoin == NULL){
		printf("Bitcoin with ID %s. Not found.\n", bitcoinID);
		free(bitcoinID);
		return;
	}
	//get sender transaction list
	TransactionList* sender_trans_list = SenderTransactionListHashTable.Find(bitcoin->owner->getID());
	if(sender_trans_list == NULL){
		printf("%s bitcoin was never used in a transaction.\n", bitcoinID);
		free(bitcoinID);
		return;
	}
	//traverse the whole list in order to find the transaction that created this node
	ListNode<Transaction*>* curr_node = sender_trans_list->transaction_list;
	while(curr_node != NULL){
		Transaction* transaction = curr_node->data;
		if(transaction->HasBitcoinNode(bitcoin)){
			transaction->print();	//found the transaction, print it
			break;
		}
		curr_node = curr_node->next;	//next node of the list, please
	}
	free(bitcoinID);
}
