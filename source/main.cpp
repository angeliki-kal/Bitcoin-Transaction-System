#include "ReadFile.hpp"
#include "HashTable.hpp"
#include "Wallet.hpp"
#include "Transaction.hpp"
#include "BitcoinTree.hpp"
#include "TransactionList.hpp"
#include "CommandLineInterface.hpp"
#include <cstdlib>
#include <ctime>

using namespace std;


int main(int argc, char** argv){
	srand(time(NULL));
	ReadCmdArgs(argc, argv);

	HashTable<Wallet> WalletContainer(8,64);
	HashTable<Transaction> TransactionContainer(8,16);
	HashTable<BitcoinTreeRoot> BitcoinContainer(8,32);
	HashTable<TransactionList> SenderTransactionListHashTable(senderHashTableNumOfEntries,bucketSize);
	HashTable<TransactionList> ReceiverTransactionListHashTable(receiverHashTableNumOfEntries,bucketSize);

	ReadBitcoinBalancesFile(BitcoinBalancesFilename,WalletContainer,BitcoinContainer);

	ReadTransactionsFile(transactionsFilename,TransactionContainer,WalletContainer,
	SenderTransactionListHashTable,ReceiverTransactionListHashTable);

	//CommandLineInterface for user
	CommandLineInterface(WalletContainer,TransactionContainer,BitcoinContainer,
	SenderTransactionListHashTable,ReceiverTransactionListHashTable);

	return 0;
}
