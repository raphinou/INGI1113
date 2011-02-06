#include "str_buf.h"

#include <assert.h>
#include <stdio.h>

void 
test_str_buf_concat() 
{
    str_buf * b1;
    str_buf * b2;
    str_buf * b3;
    str_buf * b4;
    str_buf * b5;
    str_buf * b6;
    str_buf * b7;
		
	b1 = str_buf_alloc(10);
	b2 = str_buf_alloc_str("abc");
	b3 = str_buf_alloc_str("wxyz");

	b4 = str_buf_concat(b1, b2);
	assert(str_buf_len(b4) == 3);
	assert(str_buf_equals(b4, b2));
	
	b5 = str_buf_concat(b2, b1);
	assert(str_buf_equals(b5, b2));
	assert(str_buf_len(b5) == 3);
	
	b6 = str_buf_concat(b4, b3);
	b7 = str_buf_alloc_str("abcwxyz");
	assert(str_buf_equals(b6, b7));
	str_buf_free(b1);
	str_buf_free(b2);
	str_buf_free(b3);
	str_buf_free(b4);
	str_buf_free(b5);
	str_buf_free(b6);
	str_buf_free(b7);	
}

// add your tests here

int main (int argc, char const *argv[])
{
	// ... (other test calls)
	test_str_buf_concat();
	
	printf("Tests have succeeded\n");
    return 0;
}
