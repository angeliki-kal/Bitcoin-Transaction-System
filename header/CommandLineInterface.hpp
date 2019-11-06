#ifndef COMMANDLINEINTERFACE_HPP
#define COMMANDLINEINTERFACE_HPP

#include "HashTable.hpp"
#include "Wallet.hpp"
#include "Transaction.hpp"
#include "BitcoinTree.hpp"
#include "TransactionList.hpp"

void requestTransaction(HashTable<Wallet> &WalletContainer,
HashTable<Transaction>&,HashTable<TransactionList>&,HashTable<TransactionList>&);

void findEarnings(HashTable<TransactionList>&);

void findPayments(HashTable<TransactionList>&);

void walletStatus(HashTable<Wallet> &WalletContainer);

void bitCoinStatus(HashTable<BitcoinTreeRoot> &BitcoinContainer);

void traceCoin(HashTable<BitcoinTreeRoot> &BitcoinContainer,
HashTable<TransactionList> &SenderTransactionListHashTable);

void CommandLineInterface(HashTable<Wallet> &WalletContainer,
HashTable<Transaction> &TransactionContainer,
HashTable<BitcoinTreeRoot> &BitcoinContainer,
HashTable<TransactionList> &SenderTransactionListHashTable,
HashTable<TransactionList> &ReceiverTransactionListHashTable);

#endif
