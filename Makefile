#############################################################################
# Sample: Make gcc file to test DynamicArrays module
# 11/06/2015  Rich Repka   - Added  us-eng-words.txt
# 12/25/2015  Rich Repka   - Added additional targets
# 06/04/2017  Rich Repka   - Switched to c99
# 08/10/2019  Rich Repka   - added .h file dependency and obj files
# 01/29/2020  Rich Repka   - removed lab_d to make lab shorter
# 02/14/2021  Rich Repka   - investigated -fsanitize=address -fsanitize=undefined
#                            not supported on the RIT compiler 
#############################################################################

# File Names
SOURCE  = example.c gifenc.c gigdec.c
PROG    = example
RESULTS = out.gif
OTHERS  = rgb2hsv

OBJS    = $(patsubst %.c,%.o, $(SOURCE))

# Compiler and Directives
CC       = gcc

# Remove the sanitize and other options for production code
#CFLAGS   = -O3 -Wall -std=c99 -pedantic 
CFLAGS   = -O1 -g -Wall -std=c99 -pedantic -fsanitize=address -fsanitize=undefined

# sanitize does the same job as valgrind
#VALGRIND = valgrind --tool=memcheck --leak-check=yes --track-origins=yes

# Here is where the action occurs
.SILENT:
.PHONY: all test mem help clean

all: $(PROG) others

others: $(OTHERS)

lab_a: lab_a.c
	echo "Building others: $@.c"
	$(CC) $(CFLAGS) -O0 $@.c -o $@
   
lab_b: lab_b.c
	echo "Building others: $@.c"
	$(CC) $(CFLAGS) -O0 $@.c -o $@
   
lab_c: lab_c.c
	echo "Building others: $@.c"
	$(CC) $(CFLAGS) -O0 $@.c -o $@

test: $(PROG)
	@echo "Making test..."
	./$(PROG)   >> $(RESULTS) 2>&1
	cat $(RESULTS)

# Link the object files
$(PROG): $(OBJS)
	@echo "Linking: $(OBJS) to $(PROG)"
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG)

# Build object files
.c.o:
	@echo "Compiling: $*.c"
	$(CC) $(CFLAGS) -c $*.c

help:
	@echo "make commands: all, test, mem, help, clean, others"
	@echo "  all  - builds the binary"
	@echo "  test - runs the test"
	@echo "  help - this help"
	@echo "  clean- removes unnecessary files"
	@echo "  others- additional student files"

clean:
	-rm -f $(RESULTS)
	-rm -f $(PROG) $(OTHERS)
	-rm -f $(OBJS)
	-rm -f $(OUT) 

