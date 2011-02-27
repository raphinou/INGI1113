#include <string.h>
#include <stdio.h>
#include <bnd_buf.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h> /* For the O_ constants, flag passed to sem_open */

#include "zip_crack.h"

#define BND_BUF_SIZE 5

/* number of free slots */
sem_t *free_slots;
/* number of slots filled */
sem_t *filled_slots;
/* mutex */
sem_t *mutex;
int *found;
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
read_dico(char * file, bnd_buf * buffer, int threads) 
/* opens file path passed in parameter, and reads it line per line 
 * each password should be maximum 29 characters long */
{
	FILE * fp;
	int free_slots_value;
	char password[30]="";
	fp=fopen(file, "r");
	while (*found==0 && read_word(fp, password))
	{
		sem_getvalue(free_slots, &free_slots_value);
		/*
		printf("Producer will wait on free slots, %i \n", free_slots_value);
		*/
		sem_wait(free_slots);
		/*
		printf("Producer done wait on free slots\n");
		printf("Producer will wait on mutex\n");
		*/
		sem_wait(mutex);
		/*
		printf("Producer done wait on mutex\n");
		*/
		bnd_buf_put(buffer,password);
		/*
		printf("PRODUCER put %s\n", password);
		*/
		sem_post(mutex);
		sem_post(filled_slots);
	}
	reader_finished = 1;
	/* increment filled_slots once more to unlock wating readers */
	sem_post(filled_slots);
	printf("PRODUCER: finished\n");
	if (threads>0)
	{
		pthread_exit(NULL);
	}

}

void 
read_buffer(bnd_buf * buffer, int number, struct zip_archive *archive)
{
	char *password;
	printf("Reader %i starting\n", number);
	while (!*found) 
	{
		/*
		printf("Reader %i will wait on filled slots\n", number);
		*/
		sem_wait(filled_slots);
		/*
		printf("Reader %i done wait on filled slots\n", number);
		printf("Reader %i will wait on mutex\n", number);
		*/
		sem_wait(mutex);
		/*
		printf("Reader %i done wait on mutex\n", number);
		printf("Reader %i will work\n", number);
		*/
		if (*found || reader_finished)
		{
			/* free mutex and activate possible waiting readers */
			sem_post(mutex);
			if (reader_finished)
				sem_post(filled_slots);
			printf("READER %i finished\n", number);
			return;
		}
		password = bnd_buf_get(buffer);
		/*
		printf("\t\tREADER %i got %s \n", args->number, password);
		*/
        if(zip_test_password(archive, password) == 0) {
			*found = 1;
            printf("Password found by %i is: %s\n", number, password);
        }
		sem_post(mutex);
		sem_post(free_slots);
		free(password);
	}
	printf("READER %i finished\n", number);
}

void 
* read_buffer_thread(void * arguments)
{
	struct consumer_params * args = (struct consumer_params *) arguments;
	read_buffer(args->buffer, args->number, args->archive);
	pthread_exit(NULL);
}


int 
main (int argc, char const * argv[])
{
    int i, c, pid=0;
    struct zip_archive * archive;
	struct consumer_params *params;
	bnd_buf * buffer;
	int thread_status, thread_number = 3, process_number=1;
	int status; /* for wait */
	int free_slots_value;
	int shmid;

   	pthread_t * threads;

	/* read parameters passed as arguments */
    while ((c = getopt (argc, (char * const*) argv, "t:p:")) != -1)
	{
		switch (c)
		{
			case 't':
				thread_number=atoi(optarg);
				break;
			case 'p':
				process_number=atoi(optarg);
				break;
			default:
				abort();
		}
	}


	buffer = bnd_buf_alloc(BND_BUF_SIZE, process_number-1);

    if ( (archive = zip_load_archive(argv[optind])) == NULL) {
        printf("Unable to open archive %s\n", argv[1]);
        return 2;
    }
	if (process_number>1) {
		shmid=shmget(IPC_PRIVATE, sizeof(int), 0777|IPC_CREAT);
		found = (int *) shmat(shmid, NULL,0);
		*found = 0;
		/* initialise semaphores shared by threads */
		/* number of free slots */

		/*free_slots = malloc(sizeof(sem_t *));
		 */
		shmid = shmget(IPC_PRIVATE,sizeof(sem_t *),0777|IPC_CREAT);
		free_slots=(sem_t *)shmat(shmid, NULL, 0);
		sem_init(free_slots, process_number-1, BND_BUF_SIZE);
		/* number of slots filled */
		/*
		filled_slots = malloc(sizeof(sem_t *));
		*/
		shmid = shmget(IPC_PRIVATE,sizeof(sem_t *),0777|IPC_CREAT);
		filled_slots=(sem_t *)shmat(shmid, NULL, 0);
		sem_init(filled_slots, process_number-1, 0);
		/* mutex initialisation */
		/*
		mutex = malloc(sizeof(sem_t *));
		*/
		shmid = shmget(IPC_PRIVATE,sizeof(sem_t *),0777|IPC_CREAT);
		mutex=(sem_t *)shmat(shmid, NULL, 0);
		sem_init(mutex, process_number-1, 1);

		sem_getvalue(free_slots, &free_slots_value);
		printf("free_slots semaphore initialised with value %i \n", free_slots_value);

		for (i=0; i<process_number; i++)
		{
			if (pid==0)
			{
				pid=fork();
			}

		}
		/* initialise semaphores */
		/*name semaphores */
		/*
		free_slots = sem_open("/free_slots", O_CREAT, 0666, BND_BUF_SIZE);
		sem_getvalue(free_slots, &free_slots_value);
		printf("/free_slots semaphore initialised with value %i \n", free_slots_value);
		filled_slots = sem_open("/filled_slots", O_CREAT, 0666, 0);
		mutex = sem_open("/mutex", O_CREAT, 0666, 1);
		*/
		if (pid==0)
		{
			/* producer */
			read_dico("dico.txt", buffer, 0);
			printf("will wait children");
			wait(&status);
			printf("done waiting children");
		}
		else
		{
			read_buffer(buffer,pid,archive);

		}
	}
	else
	{
		found = malloc(sizeof(int));
		*found = 0;
		/* initialise semaphores shared by threads */
		/* number of free slots */
		free_slots = malloc(sizeof(sem_t *));
		sem_init(free_slots, process_number-1, BND_BUF_SIZE);
		/* number of slots filled */
		filled_slots = malloc(sizeof(sem_t *));
		sem_init(filled_slots, process_number-1, 0);
		/* mutex initialisation */
		mutex = malloc(sizeof(sem_t *));
		sem_init(mutex, process_number-1, 1);

		threads = malloc(thread_number * sizeof(pthread_t));;
		params = malloc(thread_number * sizeof(struct consumer_params));
		/* consumer threads */
		for (i=0; i<thread_number; i++)
		{
			params[i].buffer=buffer;
			params[i].archive=archive;
			params[i].number=i;
			thread_status = pthread_create(&threads[i], NULL, read_buffer_thread, &params[i] );
		}
		/* producer */
		read_dico("dico.txt", buffer, thread_number);

		/* join threads */
		for (i=0; i<thread_number; i++)
		{
			pthread_join(threads[i], NULL);
		}
	}
    if (argc < 2) {
        usage();
        return 1;
    }

    printf("Password found. %i\n", *found);

    zip_close_archive(archive);
    return 0;
}
