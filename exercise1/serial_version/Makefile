# Compiler
CC=mpicc

# Directories
INCDIR=include
OBJDIR=obj
SRCDIR=src
SNAPDIR=snaps

# Flags
CFLAGS=-I$(INCDIR) -Wall -c -g # -fopenmp
OBJFLAGS=-Wall -g # -fopenmp -O3 -march=native

# Files
OBJECTS=$(OBJDIR)/main.o $(OBJDIR)/init.o $(OBJDIR)/read_write_pgm_image.o $(OBJDIR)/run.o $(OBJDIR)/static_evolution.o $(OBJDIR)/ordered_evolution.o $(OBJDIR)/live_or_die.o

main.x: $(OBJECTS)
	$(CC) $(OBJFLAGS) -o $@ $^
	
$(OBJDIR)/main.o: main.c
	$(CC) $(CFLAGS) -o $@ $^	
	
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
		rm -f $(OBJDIR)/*.o *.x
image:
		rm -f *.pgm
		rm -f $(SNAPDIR)/*

