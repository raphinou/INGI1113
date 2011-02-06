FILES = str_buf.c
TEST_FILES = testing.c
CC_FLAGS =  -Wall -Werror -std=c99
#build: test_executable str_buf.o 

build: str_buf.o
	gcc $(CC_FLAGS) -o test_executable  $(TEST_FILES) str_buf.o
str_buf.o: 
	gcc $(CC_FLAGS) -o str_buf.o -c $(FILES)
tests: build
	./test_executable
clean:
	rm -f test_executable str_buf.o
