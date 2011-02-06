FILES = str_buf.c
TEST_FILES = testing.c
CC_FLAGS =  -Wall -Werror -std=c99
#build: test_executable str_buf.o 

#str_buf.o: 
#	gcc $(CC_FLAGS) -o str_buf.o $(FILES)
test_executable:
	gcc $(CC_FLAGS) -o test_executable $(FILES) $(TEST_FILES)
tests:
	./test_executable
clean:
	rm test_executable
