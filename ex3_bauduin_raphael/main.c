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
	int counter=0;
	char password[30]="";
	fp=fopen(file, "r");
	while (counter++<20 && read_word(fp, password))
	{
		sem_wait(&free_slots);
		sem_wait(&mutex);
		bnd_buf_put(buffer,password);
		printf("PRODUCER put %s\n", password);
		sem_post(&mutex);
		sem_post(&filled_slots);
	}

}

void 
* read_buffer(void * arguments)
{
	char *password;
	bnd_buf * buffer;
    buffer	= (bnd_buf *) arguments;
	while (1) 
	{
		sleep(2);
		sem_wait(&filled_slots);
		sem_wait(&mutex);
		password = bnd_buf_get(buffer);
		printf("\t\tREADER got %s \n", password);
		sem_post(&mutex);
		sem_post(&free_slots);
	}
}

int 
main (int argc, char const * argv[])
{
    int i;
    struct zip_archive * archive;
	bnd_buf * buffer;
	int thread_status;
   	pthread_t * thread=malloc(sizeof(pthread_t));;
	/* number of free slots */
	sem_init(&free_slots, 0, BND_BUF_SIZE);
	/* number of slots filled */
	sem_init(&filled_slots, 0, 0);
	/* mutex initialisation */
	sem_init(&mutex, 0, 1);

	buffer = bnd_buf_alloc(BND_BUF_SIZE);

	/* producer */
	thread_status = pthread_create(thread, NULL, read_buffer, buffer );
	read_dico("dico.txt", buffer);
	pthread_join(*thread, NULL);

    if (argc < 2) {
        usage();
        return 1;
    }

    if ( (archive = zip_load_archive(argv[1])) == NULL) {
        printf("Unable to open archive %s\n", argv[1]);
        return 2;
    }

    for (i = 2; i < argc; i++) {
        if(zip_test_password(archive, argv[i]) == 0) {
            printf("Password is: %s\n", argv[i]);
            goto finish;
        }
    }

    printf("Password not found\n");

finish:   
    zip_close_archive(archive);
    return 0;
}
