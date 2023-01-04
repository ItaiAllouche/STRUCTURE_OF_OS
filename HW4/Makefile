CC = g++
CCLINK = $(CC)
FLAGS = -g -std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG -pthread
OBJS = bank.o atm.o account.o
RM = rm -f

Bank: $(OBJS)
	$(CCLINK) $(FLAGS) -o Bank $(OBJS)

bank.o: bank.cpp
	$(CCLINK) $(FLAGS) -c bank.cpp
atm.o: atm.cpp
	$(CCLINK) $(FLAGS) -c atm.cpp
account.o: account.cpp
	$(CCLINK) $(FLAGS) -c account.cpp

clean:
	$(RM) *.o  Bank log.txt
