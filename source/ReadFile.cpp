#include "ReadFile.hpp"
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <fstream>

char* BitcoinBalancesFilename;
char* transactionsFilename;
float bitcoinValue;
int senderHashTableNumOfEntries, receiverHashTableNumOfEntries, bucketSize;

using namespace std;

void ReadCmdArgs(int argc, char** argv){
  int arg_index=1;
  int a_flag=0,t_flag=0, v_flag=0, h1_flag=0, h2_flag=0, b_flag=0;

  //scan all arguements to find identifiers
  while(arg_index < argc){
    //make sure there is an argument to check
    if(argv[arg_index] == NULL)
      break;

    if(strcmp(argv[arg_index],"-a")==0 && a_flag==0 && argv[arg_index+1]!=NULL){
      BitcoinBalancesFilename = argv[++arg_index];
      a_flag = 1;
      arg_index++;
      continue;
    }
    if(strcmp(argv[arg_index],"-t")==0 && t_flag==0 && argv[arg_index+1]!=NULL){
      transactionsFilename = argv[++arg_index];
      t_flag = 1;
      arg_index++;
      continue;
    }
    if(strcmp(argv[arg_index],"-v")==0 && v_flag==0 && argv[arg_index+1]!=NULL){
      bitcoinValue = atof(argv[++arg_index]);
      v_flag = 1;
      arg_index++;
      continue;
    }
    if(strcmp(argv[arg_index],"-h1")==0 && h1_flag==0 && argv[arg_index+1]!=NULL){
      senderHashTableNumOfEntries = atoi(argv[++arg_index]);
      h1_flag = 1;
      arg_index++;
      continue;
    }
    if(strcmp(argv[arg_index],"-h2")==0 && h2_flag==0 && argv[arg_index+1]!=NULL){
      receiverHashTableNumOfEntries = atoi(argv[++arg_index]);
      h2_flag = 1;
      arg_index++;
      continue;
    }
    if(strcmp(argv[arg_index],"-b")==0 && b_flag==0 && argv[arg_index+1]!=NULL){
      bucketSize = atoi(argv[++arg_index]);
      b_flag = 1;
      arg_index++;
      continue;
    }
    cerr << "Invalid command line arguments at: " << argv[arg_index] << endl;
    exit(-2);
  }

  if(!a_flag && !t_flag && !v_flag && !h1_flag && !h2_flag && !b_flag){
    cerr << "Missing command line arguments" << endl;
    exit(-3);
  }
}


void ReadBitcoinBalancesFile(char* filename, HashTable<Wallet>& WalletContainer,
HashTable<BitcoinTreeRoot>& BitcoinContainer){
  //open file
  ifstream file(filename);
  if(!file.is_open()){
    cerr << "Can't open " << filename << endl;
    exit(-1);
  }
  //read contents (string by string)
  char last_char_read;
  char* walletID = NULL;
  while((walletID=getStr(file,&last_char_read)) != NULL){
    //make and add a new Wallet
    Wallet* wallet = new Wallet(walletID);
    if(WalletContainer.Insert(walletID, wallet) != 0){
      fprintf(stderr,"Duplicate Wallet %s. Fix this and try again.\n",walletID);
      exit(-5);
    }
    //get bitcoinIDs
    if(last_char_read == '\n'){
      continue;  //no bitcoins for this user, line end
    }
    else{
      char* bitcoinID=NULL;
      while((bitcoinID=getStr(file,&last_char_read)) != NULL){
        BitcoinTreeNode* bitcoin = new BitcoinTreeNode(bitcoinID,wallet,bitcoinValue);
        if(BitcoinContainer.Insert(bitcoinID, bitcoin) != 0){
          fprintf(stderr,"Duplicate Bitcoin %s. Fix this and try again.\n",bitcoinID);
          exit(-5);
        }
        wallet->addBitcoin(bitcoin);
        if(last_char_read == '\n')
          break;  //line end, break and look at next user.
      }
    }
  }

  file.close();
}

void ReadTransactionsFile(char* filename,HashTable<Transaction>& TransactionContainer,
HashTable<Wallet> &WalletContainer,HashTable<TransactionList> &SenderHashTable,
HashTable<TransactionList> &ReceiverHashtable){
  //open file
  ifstream file(filename);
  if(!file.is_open()){
    cerr << "Can't open " << filename << endl;
    exit(-1);
  }
  char last_char_read = 'a';  //just dont be EOF
  /*read 6 strings that describe a transaction
    update bitcoin trees(make payment) and create transaction
    update transaction lists
  */
  while(last_char_read != EOF){
    //get transactionID
    char* transactionID = getStr(file,&last_char_read);
    if(last_char_read == EOF)
      break;
    if (transactionID == NULL || last_char_read == '\n'){
      cout << "Invalid transaction (at transactionID): " << transactionID << endl;
      continue;
    }
    //get other transaction strings
    char *senderID,*receiverID,*value_str,*date,*time;
    if(ReadTransaction(file,&senderID,&receiverID,&value_str,&date,&time,&last_char_read)!=0){
      //transaction couldn't be read properly, continue to next one
      free(transactionID);
      continue;
    }
    Transaction* transaction = CompleteTransaction(transactionID,date,time,
                               senderID,receiverID,(double)atof(value_str),
                               WalletContainer,TransactionContainer);
    if(transaction != NULL){
      //transaction successfull, update sender and receiver lists
      UpdateSenderReceiverLists(SenderHashTable,ReceiverHashtable,transaction);
    }
    else{
      //transaction failed, delete strings
      free(transactionID);
      free(date);
      free(time);
    }
    //cleanup
    free(value_str);
    free(senderID);
    free(receiverID);
  }
}


/*Get cstring from istream. Programmers responsiblity to free said cstring.*/
char* getStr(istream &data, char* last_char_read){
  char c;
  size_t buffer_size = 10*sizeof(char);
  char* buffer = (char*)malloc(buffer_size);
  int buffer_len = 0;

  while((c=data.get()) != EOF){
    if(buffer_len==0 && isspace(c)){
      //ignore and existent whitespace before string
      continue;
    }
    //buffer too small, realloc x2
    if(buffer_len >= buffer_size-1){
      buffer_size *= 2;
      buffer = (char*)realloc(buffer,buffer_size);
    }
    //found whitespace =>  string ended
    if(isspace(c) || c==';'){
      break;
    }
    //add char to string
    buffer[buffer_len++] = c;
  }
  *last_char_read = c;
  if(buffer_len == 0){  //exception when nothing was read
    free(buffer);
    return NULL;
  }
  //trim string to actual size
  buffer = (char*)realloc(buffer,(buffer_len+1)*sizeof(char));
  buffer[buffer_len] = '\0';
  return buffer;
}

char* ReadTillNewline(istream &data){
  char c;
  size_t buffer_size = 10*sizeof(char);
  char* buffer = (char*)malloc(buffer_size);
  int buffer_len = 0;

  while((c=data.get()) != '\n'){
    //buffer too small, realloc x2
    if(buffer_len >= buffer_size-1){
      buffer_size *= 2;
      buffer = (char*)realloc(buffer,buffer_size);
    }
    //add char to string
    buffer[buffer_len++] = c;
  }
  if(buffer_len == 0){  //exception when nothing was read
    free(buffer);
    return NULL;
  }
  //trim string to actual size
  buffer = (char*)realloc(buffer,(buffer_len+1)*sizeof(char));
  buffer[buffer_len] = '\0';
  return buffer;
}

/*Reads time-range format from stream.
Either as: time1 time2
			 or: time1 year1 time2 year2
In each case the value that were not found are set to NULL.*/
void getTime(istream &data, char** time1, char** time2, char** year1, char** year2,char* last_char_read){
	*time1 = NULL;
	*time2 = NULL;
	*year1 = NULL;
	*year2 = NULL;
	if((*time1=getStr(data,last_char_read))==NULL){
		printf("Bad formatting for time\n");
		return;
	}
	if((*time2=getStr(data,last_char_read))==NULL){
			printf("Bad formatting for time\n");
			free(*time1);
			return;
	}
	if(*last_char_read != '\n'){
		//command format: walletID time1 year1 time2 year2
		*year1 = *time2;
		if((*time2=getStr(data,last_char_read))==NULL){
				printf("Bad formatting for time\n");
				free(*time1);
				free(*year1);
				return;
		}
		if((*year2=getStr(data,last_char_read))==NULL){
				printf("Bad formatting for time\n");
				free(*time1);
				free(*year1);
				free(*time2);
				return;
		}
    ReverseDateFormat(year1);
    ReverseDateFormat(year2);
	}
	return;
}

/*Make sure time1<time2*/
int Time1BeforeTime2(char* time1,char* time2){
  if(strcmp(time1,time2)<=0){
  //  printf("%s <= %s\n", time1,time2);
    return 1;
  }
  //printf("%s > %s\n", time1,time2);
  return 0;
}

/*Make sure time1<time2 and year1<year2*/
int Time1BeforeTime2(char* time1,char* year1,char* time2,char* year2){
  if(strcmp(year1,year2)==0){ //year1 == year2
    if(strcmp(time1,time2)<=0)  //but time1<=time2
      return 1;
  }
  else if(strcmp(year1,year2)<=0) //year1 <= year2
    return 1;
  return 0;
}

char* CurrentDate(){
  char* buffer = (char*)malloc((strlen("yyyy-mm-dd")+1)*sizeof(char));
  time_t now = time(NULL);
  strftime(buffer, strlen("yyyy-mm-dd")+1, "%Y-%m-%d", localtime(&now));
  return buffer;
}

char* CurrentTime(){
  char* buffer = (char*)malloc((strlen("hh:mm")+1)*sizeof(char));
  time_t now = time(NULL);
  strftime(buffer, strlen("hh:mm")+1, "%H:%M", localtime(&now));
  return buffer;
}

/*Reverse date format from dd-mm-yyyy to yyyy-mm-dd*/
void ReverseDateFormat(char** date){
  //parse date
  char day[3],month[3],year[5];
  sscanf(*date, "%[^-]-%[^-]-%[^-]", day,month,year);
  //create new date
  char* reversed_date = (char*)malloc((strlen("yyyy-mm-dd")+1)*sizeof(char));
  sprintf(reversed_date, "%s-%s-%s", year,month,day);
  //replace old date
  free(*date);
  *date = reversed_date;
}
