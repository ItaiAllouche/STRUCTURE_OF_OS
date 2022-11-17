# Makefile for the smash program
CC = g++
CFLAGS = -g -Wall c++11
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o jobs.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	$(CCLINK) -o smash $(OBJS)
# Creating the object files
commands.o: commands.cpp 
smash.o: smash.cpp 
signals.o: signals.cpp 
jobs.o:  jobs.cpp 
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*

