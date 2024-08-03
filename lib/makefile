# set the C compiler
CC = gcc

# set the path to the include directory
INCDIR = ../include

# set the flags for the C compiler to give lots of warnings
CFLAGS = -g -I$(INCDIR) -I/opt/local/include -O2 -Wall -Wstrict-prototypes -Wnested-externs -Wmissing-prototypes -Wmissing-declarations

# library tool defs
AR = ar
ARFLAGS = rv
RANLIB = ranlib

# path to the object file directory
ODIR = obj

# path to the lib directory
LIBDIR = .

# path to the bin directory
BINDIR = ../bin

# put all of the relevant include files here
_DEPS = image.h graphics.h color.h

# convert them to point to the right place
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))

# automatically find all .c files in the current directory
_SRC = $(wildcard *.c)

# convert them to object files in the obj directory
OBJ = $(patsubst %.c,$(ODIR)/%.o,$(_SRC))

# pattern for compiling source code
$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# output library
OUTPUT_LIB = libimageIO.a

# make rules
all: $(OUTPUT_LIB)

$(OUTPUT_LIB): $(OBJ)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $(OBJ)
	$(RANLIB) $@

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
