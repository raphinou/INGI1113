#include "str_buf.h"

#include <assert.h>
#include <stdio.h>

//void 
//test_str_buf_alloc() 
//{
//	// comment tester?
//}


void
test_str_buf_alloc_str()
{
	str_buf * b1;
	b1 = str_buf_alloc_str("abc");
	//test size
	assert((*b1).size == 3);
	//test length
	assert((*b1).len==3);
	//test each character
	assert(((*b1).string)[0]=='a');
	assert(((*b1).string)[1]=='b');
	assert(((*b1).string)[2]=='c');
	assert(((*b1).string)[3]=='\0');
	str_buf_free(b1);

}

void
test_str_buf_alloc_substr()
{

	str_buf * b1;
	b1 = str_buf_alloc_substr("abcdef",3);
	//test size
	assert((*b1).size == 3);
	//test length
	assert((*b1).len==3);
	//test each character
	assert(((*b1).string)[0]=='a');
	assert(((*b1).string)[1]=='b');
	assert(((*b1).string)[2]=='c');
	assert(((*b1).string)[3]=='\0');
	str_buf_free(b1);

}

// comment tester?
//void
//test_str_buf_free()


void
test_str_buf_head()
{
	str_buf * b1;
	b1 = str_buf_alloc_str("abc");
	const char * s = str_buf_head(b1);
	//test each character
	assert(s[0]=='a');
	assert(s[1]=='b');
	assert(s[2]=='c');
	assert(s[3]=='\0');
	str_buf_free(b1);
}


void
test_str_buf_len()
{
	str_buf * b1;
	//non-empty string
	b1 = str_buf_alloc_str("abc");
	assert(str_buf_len(b1)==3);
	//test empty string
	b1 = str_buf_alloc_str("");
	assert(str_buf_len(b1)==0);
	str_buf_free(b1);
}

void
test_str_buf_size()
{
	str_buf * b1;
	//non-empty string
	b1 = str_buf_alloc_str("abc");
	assert(str_buf_size(b1)==3);
	//test empty string
	b1 = str_buf_alloc_str("");
	assert(str_buf_size(b1)==0);
	str_buf_free(b1);
}

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

void
test_str_buf_equals()
{
	str_buf * b1;
	str_buf * b2;
   	str_buf * b3;
	b1 = str_buf_alloc_str("abc");
	b2 = str_buf_alloc_str("abc");
	b3 = str_buf_alloc_str("abcd");
	// test equality, independent of arguments ordery
	assert(str_buf_equals(b1,b2));
	assert(str_buf_equals(b2,b1));
	// test inequality, independent of arguments order
	assert(!str_buf_equals(b1,b3));
	assert(!str_buf_equals(b2,b3));
	assert(!str_buf_equals(b3,b1));
	assert(!str_buf_equals(b3,b2));
	str_buf_free(b1);
	str_buf_free(b2);
	str_buf_free(b3);

}

void
test_str_buf_put_buf()
{
	str_buf * b1;
	str_buf * b2;
	str_buf * b3;
	str_buf * b4;
	str_buf * final_buf;

	b1 = str_buf_alloc(6);
	// b2 and b3 can be put in b1, but after that b4 cannot fit in.
	b2 = str_buf_alloc_str("abc");
	b3 = str_buf_alloc_str("de");
	b4 = str_buf_alloc_str("fg");
	// what we should have at the end
	final_buf = str_buf_alloc_str("abcde");
	//put b2
	str_buf_put_buf(b1,b2);
	assert(str_buf_len(b1)==3);
	assert(str_buf_equals(b1,b2));
	//put b3
	str_buf_put_buf(b1,b3);
	assert(str_buf_len(b1)==5);
	assert(str_buf_equals(b1,final_buf));
	//put b4
	str_buf_put_buf(b1,b4);
	assert(str_buf_len(b1)==5);
	assert(str_buf_equals(b1,final_buf));
}

void
test_str_buf_put_str()
{
	str_buf * b1;
	str_buf * b2;
	str_buf * final_buf;

	b1 = str_buf_alloc(6);
	// b2 and b3 can be put in b1, but after that b4 cannot fit in.
	b2 = str_buf_alloc_str("abc");
	// what we should have at the end
	final_buf = str_buf_alloc_str("abcde");
	//put "abc"
	str_buf_put_str(b1,"abc");
	assert(str_buf_len(b1)==3);
	assert(str_buf_equals(b1,b2));
	//put "de"
	str_buf_put_str(b1,"de");
	assert(str_buf_len(b1)==5);
	assert(str_buf_equals(b1,final_buf));
	//put "fg"
	str_buf_put_str(b1,"fg");
	assert(str_buf_len(b1)==5);
	assert(str_buf_equals(b1,final_buf));
  

}

void 
test_str_buf_put_substr()
{
	str_buf * b1;
	str_buf * b2;
	str_buf * final_buf;

	b1 = str_buf_alloc(6);
	// b2 and b3 can be put in b1, but after that b4 cannot fit in.
	b2 = str_buf_alloc_str("abc");
	// what we should have at the end
	final_buf = str_buf_alloc_str("abcde");
	//put "abc"
	str_buf_put_substr(b1,"abcdefghij",3);
	assert(str_buf_len(b1)==3);
	assert(str_buf_equals(b1,b2));
	//put "de"
	str_buf_put_substr(b1,"de",2);
	assert(str_buf_len(b1)==5);
	assert(str_buf_equals(b1,final_buf));
	//put "fg"
	str_buf_put_substr(b1,"fghij",2);
	assert(str_buf_len(b1)==5);
	assert(str_buf_equals(b1,final_buf));
}

int main (int argc, char const *argv[])
{
	// ... (other test calls)
	test_str_buf_concat();
	test_str_buf_alloc_str();
	test_str_buf_alloc_substr();
	test_str_buf_head();
	test_str_buf_len();
	test_str_buf_size();
	test_str_buf_equals();
	test_str_buf_put_buf();
	test_str_buf_put_str();
	test_str_buf_put_substr();
	
	printf("Tests have succeeded\n");
    return 0;
}
