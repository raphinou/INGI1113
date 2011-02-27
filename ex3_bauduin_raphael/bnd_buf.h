#ifndef __BND_BUF_H__
#define __BND_BUF_H__

struct bnd_buf ;

typedef struct bnd_buf bnd_buf;

/*
 * Creates and returns a bounded buffer of size size
 */
bnd_buf *
bnd_buf_alloc(int size, int shared);

/*
 * Frees the memory used by a buffer 
 * */
void
bnd_buf_free(bnd_buf * buffer);

/*
 * Put a string in the bounded buffer.
 * Returns 0 if the buffer is full and operation cannot continue. 
 * Returns 1 if operation successful.
 */
int
bnd_buf_put(bnd_buf * buffer, char * s);

/*
 * returns the first element of the buffer, and removes it from the buffer
 * QUESTION: better to return a status of the value? Eg when buffer empty, wait,
 * or return status BUFFER_EMPTY?
 */
char * 
bnd_buf_get(bnd_buf * buffer);

/*
 * Returns the size of the buffer
 */
int 
bnd_buf_size(bnd_buf * buffer);

/*
 * returns the number of free slots
 */
int
bnd_buf_free_slots(bnd_buf * buffer);

/*
 * reutns the number of items in the buffer
 */
int
bnd_buf_items_count(bnd_buf * buffer);

#endif
