# Makefile for the smash program
CC = g++
CFLAGS = -g -Wall -std=c++11
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o jobs.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	g++ -std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG *.cpp -o smash
# Creating the object files
commands.o: commands.cpp commands.h 
smash.o: smash.cpp commands.h 
signals.o: signals.cpp signals.h
jobs.o:  jobs.cpp jobs.h
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.* smash

