

#::::::::::::::::::::::::::::::::::::::::::
#
# This is an example Makefile for a countwords program.  This
# program uses both the scanner module and a counter module.
# Typing 'make' or 'make count' will create the executable file.
#

# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#
# for C++ define  CC = g++
CC = gcc
CFLAGS  = -g -Wall

# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
# you can name this target entry anything, but "default" or "all"
# are the most commonly used names by convention
#
default: ifish

# To create the executable file count we need the object files
# countwords.o, counter.o, and scanner.o:
#
ifish:  ifish.o safefork.o memorymanage.o
	$(CC) $(CFLAGS) -o ifish ifish.o safefork.o memorymanage.o

# To create the object file countwords.o, we need the source
# files countwords.c, scanner.h, and counter.h:
#
ifish.o:  ifish.c safefork.h
	$(CC) $(CFLAGS) -c ifish.c

# To create the object file counter.o, we need the source files
# counter.c and counter.h:
#
safefork.o:  safefork.c safefork.h
	$(CC) $(CFLAGS) -c safefork.c

memorymanage.o: memorymanage.c memorymanage.h
	$(CC) $(CFLAGS) -c memorymanage.c
.PHONY: clean
	
clean: 
	$(RM) ifish safefork *.o *~