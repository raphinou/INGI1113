#include "bnd_buf.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define SHM_BUF_ID 0xe414ba17
#define SHM_BUF_ARRAY_ID 0xaf2cc0f2
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
bnd_buf_alloc(int size, int shared)
{
	/* we have to malloc size+1 because there is one lement in the array that
	 * cannot be used when full (or we couldn't distinct an empty buffer from a
	 * full one)
	 */
	bnd_buf * buffer;
	int i, shmid;
	char *s;
	char ** shm_array;

	if (shared>0) 
	{
		shmid = shmget(SHM_BUF_ID,sizeof(bnd_buf),0777|IPC_CREAT);
		if (shmid==-1)
		{
			printf("ERROR on shmget buffer: %s\n", strerror(errno));
			return NULL;
		}
		if ((buffer=(bnd_buf *)shmat(shmid, NULL, 0)) == (bnd_buf *) -1)
		{
			printf("error shmat buffer: %s\n", strerror(errno));
			return NULL;
		}

		shmid=shmget(SHM_BUF_ARRAY_ID, size*sizeof(char *), IPC_CREAT);
		if (shmid==-1)
		{
			printf("ERROR on shmid");
			return NULL;
		}
		printf("shpmid = %i\n", shmid);
		shm_array = shmat(shmid, NULL,0);
		buffer->size=4;
		buffer->array = shm_array;
		for (i=0; i<size; i++) 
		{
			shmid = shmget(IPC_PRIVATE, (50*sizeof(char)), IPC_CREAT);
			if (shmid==-1)
			{
				printf("ERROR on shmid array element");
				return NULL;
			}
			s=shmat(shmid , NULL, 0);
			if (s == (char * ) -1)
			{
				printf("error shmat array element: %s\n", strerror(errno));
				return NULL;
			}
			buffer->array[i]=s;
		}
		buffer->size = size;
		buffer->elements=0;
		buffer->first = 0;
		buffer->last = 0;
		return buffer;
	} 
	else
	{
		buffer=malloc(sizeof(bnd_buf));
		buffer->array = malloc((size)*sizeof(char *));
		for (i=0; i<size; i++) 
		{
			buffer->array[i]=malloc(50*sizeof(char));
		}
		buffer->size = size;
		buffer->elements=0;
		buffer->first = 0;
		buffer->last = 0;
		return buffer;
	}
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
	strcpy(buffer->array[buffer->last], s);
	buffer->last=(buffer->last+1)%(buffer->size);
	buffer->elements++;
	return 1;
}

char *
bnd_buf_get(bnd_buf * buffer){
	char * s=malloc(sizeof(char)*50);
	/* QUESTION: good idea to return a NULL pointer when no item available?
	 * */
	if (buffer->elements == 0)
		return NULL;
	
	strcpy(s,buffer->array[buffer->first]);
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
