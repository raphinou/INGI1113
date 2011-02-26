#include "bnd_buf.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * size is the usable size of the buffer, ie size of the arry-1
 * elements is the number of elements currently in the buffer
 * first is a pointer the the first element in the buffer if buffer not empty
 * last is a pointer to the next element to be filled by a put operation
 */
struct bnd_buf {
	char ** array;
	int size;
	int elements;
	int first;
	int last;
};

bnd_buf *
bnd_buf_alloc(int size)
{
	/* we have to malloc size+1 because there is one lement in the array that
	 * cannot be used when full (or we couldn't distinct an empty buffer from a
	 * full one)
	 */
	bnd_buf * buffer;
	buffer=malloc(sizeof(bnd_buf));
	buffer->array = malloc((size)*sizeof(char *));
	buffer->size = size;
	buffer->elements=0;
	buffer->first = 0;
	buffer->last = 0;
	return buffer;
}

void 
bnd_buf_free(bnd_buf * buffer)
{
	/* free stored strings memory */
	int i;
	for (i=buffer->first; i<buffer->last; i++)
	{
		/* FIXME: BOUM! 
		printf("freeing array element %i\n", i);
		free(*(buffer->array[i]));
		*/
	}
	/* free the array */
	free(buffer->array);
	/* free the buffer */
	free(buffer);
}

int 
bnd_buf_put(bnd_buf *buffer, char * s)
{
	if (bnd_buf_free_slots(buffer)==0)
	{
		return 0;
	}
	buffer->array[buffer->last]=s;
	buffer->last=(buffer->last+1)%(buffer->size);
	buffer->elements++;
	return 1;
}

char *
bnd_buf_get(bnd_buf * buffer){
	char * s;
	/* QUESTION: good idea to return a NULL pointer when no item available?
	 * */
	if (buffer->elements == 0)
		return NULL;
	s=buffer->array[buffer->first];
	buffer->first=(buffer->first+1)%(buffer->size);
	buffer->elements--;
	return s;
}

int 
bnd_buf_size(bnd_buf * buffer)
{
	return buffer->size;
}

int
bnd_buf_free_slots(bnd_buf * buffer)
{
	return buffer->size-buffer->elements;
}

int 
bnd_buf_items_count(bnd_buf *buffer)
{
	return buffer->elements;
}
