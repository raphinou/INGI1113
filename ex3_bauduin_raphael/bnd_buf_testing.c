#include "bnd_buf.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct bnd_buf {
	char ** array;
	int size;
	int elements;
	int first;
	int last;
};

bnd_buf *
get_test_buffer()
{
	bnd_buf * buffer = bnd_buf_alloc(2);
	bnd_buf_put(buffer,"new password");
	bnd_buf_put(buffer,"second password");
	bnd_buf_put(buffer,"third password");
	return buffer;
}

void
test_bnd_buf_alloc()
{
	bnd_buf * buffer;
	buffer = bnd_buf_alloc(5);
	assert(bnd_buf_size(buffer)==5);
	assert(bnd_buf_free_slots(buffer)==5);
	bnd_buf_free(buffer);
}

void
test_bnd_buf_put()
{
	bnd_buf * buffer;
	int ret;
	buffer = bnd_buf_alloc(2);
	ret = bnd_buf_put(buffer,"new password");
	assert(bnd_buf_free_slots(buffer)==1);
	assert(ret==1);
	ret = bnd_buf_put(buffer,"second password");
	assert(bnd_buf_free_slots(buffer)==0);
	assert(ret==1);
	ret = bnd_buf_put(buffer,"third password");
	assert(bnd_buf_free_slots(buffer)==0);
	assert(ret==0);
	/* FIXME: bnd_buf_free problem occurs in this call */
	bnd_buf_free(buffer);
}

void 
test_bnd_buf_get()
{
	int ret;
	char * s;
	bnd_buf * buffer = get_test_buffer();
	assert(bnd_buf_free_slots(buffer)==0);
	assert(bnd_buf_items_count(buffer)==2);
	s=bnd_buf_get(buffer);
	printf("passord: %s\n",s);
	assert(strcmp(s,"new password")==0);
	assert(bnd_buf_free_slots(buffer)==1);
	ret = bnd_buf_put(buffer,"third password");
	assert(ret==1);
	s=bnd_buf_get(buffer);
	assert(strcmp(s,"second password")==0);
	assert(bnd_buf_free_slots(buffer)==1);
	assert(bnd_buf_items_count(buffer)==1);
	s=bnd_buf_get(buffer);
	assert(strcmp(s,"third password")==0);
	assert(bnd_buf_free_slots(buffer)==2);
	assert(bnd_buf_items_count(buffer)==0);


}



int 
main(int argc, char **argv)
{
	test_bnd_buf_alloc();
	test_bnd_buf_put();
	test_bnd_buf_get();
	printf("Tests have succeeded\n");
	return 0;
}
