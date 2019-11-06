INCLUDE_DIR = ./header
SOURCE_DIR = ./source
OBJECT_DIR = ./object

CC = g++
CFLAGS=-I$(INCLUDE_DIR) -g3 -fmax-errors=1
OUT = bitcoin

_DEPS = BitcoinTree.hpp CommandLineInterface.hpp HashTable.hpp Mylist.hpp ReadFile.hpp Transaction.hpp TransactionList.hpp Wallet.hpp
#pattern matching from  _DEPS to include directory
DEPS = $(patsubst %,$(INCLUDE_DIR)/%,$(_DEPS))

_OBJ = BitcoinTree.o CommandLineInterface.o main.o ReadFile.o Transaction.o TransactionList.o Wallet.o
#same pattern matching principe
OBJ = $(patsubst %,$(OBJECT_DIR)/%,$(_OBJ))

.PHONY: clean

############goals#######################

#default goal
$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

#general rule for all object files
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

count:
	wc $(SOURCE_DIR)/*.cpp $(DEPS)

clean:
	rm -f $(OBJECT_DIR)/*.o ./bitcoin
