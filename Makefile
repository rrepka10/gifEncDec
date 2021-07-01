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
SOURCE  = example.c gifenc.c gifdec.c
PROG    = example
OTHERS  = rgb2hsv

OBJS    = $(patsubst %.c,%.o, $(SOURCE))

# Compiler and Directives
CC       = gcc

# Remove the sanitize and other options for production code
#CFLAGS   = -O3 -Wall -std=c99 -pedantic 
CFLAGS   =  -g -fsanitize=address -fsanitize=undefined

# sanitize does the same job as valgrind
#VALGRIND = valgrind --tool=memcheck --leak-check=yes --track-origins=yes

# Here is where the action occurs
.SILENT:
.PHONY: all tests help clean

all: $(PROG) $(OTHERS)

others:$(OTHERS)

$(OTHERS): $(OTHERS).c
	@echo "Compiling: $(OTHERS).c to $(OTHERS)"
	$(CC) $(CFLAGS) $(OTHERS).c -lm -o $(OTHERS)

tests: $(PROG)
	@echo "Running tests..."
	./$(PROG) r comic.gif 
	./$(PROG) w out.gif
	./$(PROG) c comic.gif copy.gif

# Link the object files
$(PROG): $(SOURCE)
	@echo "Compiling: $(SOURCE) to $(PROG)"
	$(CC)  $(SOURCE) $(CFLAGS) -o $(PROG) 

help:
	@echo "make commands: all, tests, help, clean"
	@echo "  all  - builds the binary"
	@echo "  tests - runs the tests"
	@echo "  help - this help"
	@echo "  clean- removes unnecessary files"
	@echo "  others- additional files"

clean:
	-rm -f $(PROG) $(OTHERS) out.ppm copy.gif out.gif outc.ppm outw.ppm outr.ppm

