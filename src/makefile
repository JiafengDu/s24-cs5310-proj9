# set the C compiler, for C++ use g++
# use this setting on most sane machines
CC = gcc

# use this line on the engin linux machines
# CC = gcc296

# set the path to the include directory
INCDIR =../include

# set the flags for the C and C++ compiler to give lots of warnings
CFLAGS = -I$(INCDIR) -I/opt/local/include -O2 -Wall -Wstrict-prototypes -Wnested-externs -Wmissing-prototypes -Wmissing-declarations
CPPFLAGS = $(CFLAGS)

# path to the object file directory
ODIR = obj

# path to the lib directory
LIBDIR =../lib

# path to the bin directory
BINDIR =../bin

# libraries to include
LIBS = -limageIO -lm
LFLAGS = -L$(LIBDIR) -L/opt/local/lib

# put all of the relevant include files here
_DEPS = image.h color.h graphics.h

# convert them to point to the right place
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))

# put a list of the executables here
EXECUTABLES = test7a

# put a list of all the object files here for all executables (with .o endings)
_OBJ = test7a.o

# convert them to point to the right place
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# patterns for compiling source code
# $< is the file that caused the action to occur
$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.C $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


all:
	for i in $(EXECUTABLES); do (make $$i) ; done


# put the specific executable rules here
test9a: $(ODIR)/test9a.o
	$(CC) -o $(BINDIR)/$@ $^ $(LFLAGS) $(LIBS)

test9b: $(ODIR)/test9b.o
	$(CC) -o $(BINDIR)/$@ $^ $(LFLAGS) $(LIBS)

test9c: $(ODIR)/test9c.o
	$(CC) -o $(BINDIR)/$@ $^ $(LFLAGS) $(LIBS)

test9d: $(ODIR)/test9d.o
	$(CC) -o $(BINDIR)/$@ $^ $(LFLAGS) $(LIBS)

cube: $(ODIR)/cube.o
	$(CC) -o $(BINDIR)/$@ $^ $(LFLAGS) $(LIBS)

portfolio: $(ODIR)/portfolio.o
	$(CC) -o $(BINDIR)/$@ $^ $(LFLAGS) $(LIBS)

testLighting_shading: $(ODIR)/testLighting_shading.o
	$(CC) -o $(BINDIR)/$@ $^ $(LFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 