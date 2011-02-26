#include <string.h>
#include <stdio.h>
#include <bnd_buf.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#include "zip_crack.h"

#define BND_BUF_SIZE 5

/* number of free slots */
sem_t free_slots;
/* number of slots filled */
sem_t filled_slots;
/* mutex */
sem_t mutex;
int found = 0;
int reader_finished = 0;
struct consumer_params {
	bnd_buf *buffer;
	int number;
	struct zip_archive *archive;
};



void
usage (void)
{
    printf("zipcrack ZIP_FILE [PASSWORD1 [PASSWORD2 ...]]\n");
}

int
read_word(FILE * fp, char * password ) 
/* reads one line from the file, which is put in password 
 * returns 1 for successful operation, 0 otherwise (at EOF) */
{
	char c;
	while(((c=getc(fp))!='\n'))
	{
		if (c==EOF)
		{
			return 0;
		}
		*(password++)=c;
		/* this is necessary to have \0 at the end of the last word of the file when it is read */
		*password='\0';
	}
	return 1;
}
void
read_dico(char * file, bnd_buf * buffer) 
/* opens file path passed in parameter, and reads it line per line 
 * each password should be maximum 29 characters long */
{
	FILE * fp;
	char password[30]="";
	fp=fopen(file, "r");
	while (found==0 && read_word(fp, password))
	{
		sem_wait(&free_slots);
		sem_wait(&mutex);
		bnd_buf_put(buffer,password);
		/*
		printf("PRODUCER put %s\n", password);
		 */
		sem_post(&mutex);
		sem_post(&filled_slots);
	}
	reader_finished = 1;
	/* increment filled_slots once more to unlock wating readers */
	sem_post(&filled_slots);
	printf("PRODUCER: finished\n");
	pthread_exit(NULL);

}

void 
* read_buffer(void * arguments)
{
	char *password;
	bnd_buf * buffer;
	struct consumer_params * args = (struct consumer_params *) arguments;
    buffer	=  args->buffer;
	printf("Reader %i starting\n", args->number);

	while (!found) 
	{
		sem_wait(&filled_slots);
		sem_wait(&mutex);
		if (found || reader_finished)
		{
			/* free mutex and activate possible waiting readers */
			sem_post(&mutex);
			if (reader_finished)
				sem_post(&filled_slots);
			printf("READER %i finished\n", args->number);
			pthread_exit(NULL);
		}
		password = bnd_buf_get(buffer);
		/*
		printf("\t\tREADER %i got %s \n", args->number, password);
		*/
        if(zip_test_password(args->archive, password) == 0) {
			found = 1;
            printf("Password found by %i is: %s\n", args->number, password);
        }
		sem_post(&mutex);
		sem_post(&free_slots);
	}
	printf("READER %i finished\n", args->number);
	pthread_exit(NULL);
}

int 
main (int argc, char const * argv[])
{
    int i;
    struct zip_archive * archive;
	struct consumer_params *params;
	bnd_buf * buffer;
	int thread_status, thread_number = 3;
   	pthread_t * threads;
	threads = malloc(thread_number * sizeof(pthread_t));;
	params = malloc(thread_number * sizeof(struct consumer_params));
	/* number of free slots */
	sem_init(&free_slots, 0, BND_BUF_SIZE);
	/* number of slots filled */
	sem_init(&filled_slots, 0, 0);
	/* mutex initialisation */
	sem_init(&mutex, 0, 1);

	buffer = bnd_buf_alloc(BND_BUF_SIZE);

    if ( (archive = zip_load_archive(argv[1])) == NULL) {
        printf("Unable to open archive %s\n", argv[1]);
        return 2;
    }
	/* consumer threads */
	for (i=0; i<thread_number; i++)
	{
		params[i].buffer=buffer;
		params[i].archive=archive;
		params[i].number=i;
		thread_status = pthread_create(&threads[i], NULL, read_buffer, &params[i] );
	}
	/* producer */
	read_dico("dico.txt", buffer);

	for (i=0; i<thread_number; i++)
	{
		pthread_join(threads[i], NULL);
	}
	printf("AFTER JOIN\n");
    if (argc < 2) {
        usage();
        return 1;
    }

/*
    for (i = 2; i < argc; i++) {
        if(zip_test_password(archive, argv[i]) == 0) {
            printf("Password is: %s\n", argv[i]);
            goto finish;
        }
    }
*/
    printf("Password found. %i\n", found);

	/*
finish:   
*/
    zip_close_archive(archive);
    return 0;
}
