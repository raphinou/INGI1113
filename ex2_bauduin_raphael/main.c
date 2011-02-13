#include "unrar.h"
#include "pthread.h"

#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

struct thread_params {
	char * file;
	char * start_letters;
	char * alphabet;
	int max_length;
};

int found = 0;

static int 
_unrar_test_password_callback(unsigned int msg, long data, long P1, long P2)
{
    switch(msg) {
          case UCM_NEEDPASSWORD:
            *(int *)data = 1;
          break;
          default:
            return 0;
    }
    return 0;
}


static int
unrar_test_password(const char * file, const char * pwd) 
{
    void                      * unrar;
    struct RARHeaderData        headerdata;
    struct RAROpenArchiveData   archivedata;
    int                         password_incorrect;
    
    password_incorrect = 0;
    bzero(&headerdata, sizeof(headerdata));
    bzero(&archivedata, sizeof(archivedata));
    archivedata.ArcName  = (char *) file;
    archivedata.OpenMode = RAR_OM_EXTRACT;
    
    unrar = RAROpenArchive(&archivedata);
    if (!unrar || archivedata.OpenResult)
        return -2;
    
    RARSetPassword(unrar, (char *) pwd);
    
    RARSetCallback(unrar, _unrar_test_password_callback, (long) &password_incorrect);
    
    if (RARReadHeader(unrar, &headerdata) != 0)
        goto error;
    
    if (RARProcessFile(unrar, RAR_TEST, NULL, NULL) != 0)
        goto error;
    
    if (password_incorrect)
        goto error;
    
    RARCloseArchive(unrar);
    return 0;
    
error:
    RARCloseArchive(unrar);
    return -1;
}

void
usage(void)
{
    printf("rarcrack RAR_FILE [PASSWORD ...]\n");
}

void
*test()
{
	printf("in test function");
	pthread_exit(NULL);
}

void  
generate_passwords_list(const char *file, const char* prefix, const char *alphabet, int max_length)
{
    if (found == 1) 
	{
	  return;
	}	
	int i, alphabet_length = strlen(alphabet), prefix_length = strlen(prefix);
	char *current_password;
	char * character_added=malloc(sizeof(char)*2);
	printf("prefix received= %s\n", prefix);
	current_password=malloc(sizeof(char)*(prefix_length+1));
	strcpy(current_password,prefix);
	
	/*
	printf("before call to test_password %s\n", current_password);
	printf("before call to test_password alphabet = %s\n", alphabet);
	printf("before call to test_password alphabet length= %i\n", alphabet_length);
	*/
	
	if(unrar_test_password(file, current_password) == 0) {
		printf("*****************************************************************\n");
		printf("Password for %s is: %s\n", file, current_password);
		printf("*****************************************************************\n");
		found = 1;
		_exit(0);
	}
	/*
	printf("after call to test_password %s, alphabet=%s \n", current_password, alphabet);
	*/

	if (prefix_length==max_length)
	{
		/*
		printf("Got to maximum length, stopping\n");
		*/
		return;
	}
	else
	{
		/* 2 loops to first add short passwords to the list, then add their children passwords */
		for(i=0; i< alphabet_length; i++)
		{
			strcpy(current_password,prefix);
			/*
			printf("current_password: %s\n", current_password);
			*/
			*character_added=*(alphabet+i);
			*(character_added+1)='\0';
			/*
			printf("character added = %s\n", character_added);
			*/
			strcat(current_password, character_added);
			/*
			printf("prefix passed to call: %s\n",current_password);
			*/
			generate_passwords_list(file, current_password, alphabet, max_length);
		}
	}
	free(current_password);
	free(character_added);
}

void
*generate_passwords(void * arg)
{
/*	int i, j, k, start_letters_length;
 *	*/
	printf("starting thread for passwords generation\n");
	struct thread_params* args = (struct thread_params*)arg;
	char * file = args->file;
	char * start_letters = args->start_letters;
	char * alphabet = args->alphabet;
	int max_length = args->max_length;

	int i,start_letters_length, alphabet_length;
	char * start_letter;
	start_letter=malloc(sizeof(char)*2);
	/*
	printf("start letters:%s\n", start_letters);
	printf("file:%s\n", file);
	printf("alphabet:%s\n", alphabet);
	printf("max_length:%i\n", max_length);
	*/
	start_letters_length = strlen(start_letters);
	alphabet_length = strlen(alphabet);
	/*
	printf("start_letter_length = %i\n", start_letters_length);
	*/
	for(i=0; i<start_letters_length;i++)
	{
		/*
		printf("start letter: %c\n", start_letters[i]);
		*/
		*start_letter=*(start_letters+i);
		*(start_letter+1)='\0';
 		printf("will call generate_passwords_list with prefix %s\n", start_letter);
		generate_passwords_list(file, start_letter, alphabet, max_length);
	}
	free(start_letter);
	pthread_exit(NULL);
}
			
int 
main (int argc, char const * argv[])
{
    int thread_status, thread_join_res, thread_number, process_number, process_count=0, i, j,c ;
	pthread_t *threads;
	char * file;
	pid_t pid;

    while ((c = getopt (argc, argv, "t:p:")) != -1)
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


	if (optind >= argc) 
	{
		usage();
        printf ("non-option ARGV-elements: ");
    }
	else
	{
            file =  argv[optind];
			printf("file = %s", file);
	}


	char * starts[thread_number*process_number];
	threads = malloc(sizeof(threads)*thread_number);
/*    
    if (argc < 2) {
        usage();
        return 1;
    }
*/    
	char  alphabet[]="abcdefghijklmnopqrstuvwxyz";
	char * current_letter = malloc(sizeof(char)*2);
	struct thread_params *params;
	params = malloc(sizeof(*params)*thread_number*process_number);
	for (i=0; i<thread_number*process_number; i++)
	{
		/* dirty code: +2 for possible rounding + \0
		 */
		starts[i]=malloc(sizeof(char)*((26/(thread_number*process_number))+2));
		*starts[i]='\0';
	}
	for (i=0; i<26; i++)
	{
		j=i%(thread_number*process_number);
		/* FIXME: better solution here? */
		current_letter[0] = *(alphabet+i);
		current_letter[1] = '\0';
		starts[j]=strcat(starts[j],current_letter );
	}
	for (i=0; i<thread_number*process_number; i++)
	{
		printf("start for thread %i : %s\n", i, starts[i]);
		params[i].file =  file;
		params[i].start_letters = starts[i];
		printf("start for thread %i : %s\n", i, starts[i]);
		printf("params start for thread %i : %s\n", i,params[i].start_letters);
		params[i].alphabet = alphabet;
		params[i].max_length = 3;
	}
	for (i=0; i<thread_number*process_number; i++)
	{
		printf("from params: start for thread %i : %s\n", i, params[i].start_letters);
	}
	/*
	struct thread_params p1;
	p1.file = (char *) (argv[1]);
    p1.start_letters = s1;
	p1.alphabet = alphabet;
    p1.max_length =	3;

	struct thread_params p2;
	p2.file = (char *) argv[2];
    p2.start_letters = s2;
	p2.alphabet = alphabet;
    p2.max_length =	3;
	*/

	/*
	generate_passwords(argv[1], s1, alphabet, 3);
	generate_passwords(argv[1], s2, alphabet, 1);
	*/
	int first = 1;
	for (i=0; i<process_number-1; i++)
	{
		if (first ==1 || pid==0) {
			first=0;
			/*
			printf("creating process %i\n", i);
			*/
			pid = fork();
			process_count++;
		}
		else
		{
			first=0;
			/*
			printf("for loop %i in pid %i\n", i,pid);
			*/
		}
	}
	if (pid==0)
	{
		process_count=0;
	}
	/*
	printf("for process with pid=%i we have the process_count = %i\n", pid, process_count);
	*/

	for (i=0; i< thread_number; i++)
	{
		/* each process launches the threads with their own params */
		printf("launching thread number %i for process %i working on params %i because process_cound = %i\n", i, pid, i+(process_count*thread_number), process_count);
		printf("start letters %i are %s\n", pid, params[i+(process_count*thread_number)].start_letters);
		thread_status = pthread_create(&threads[i], NULL, generate_passwords, &(params[i+(process_count*thread_number)]));
	}
	/*
	thread_status = pthread_create(&threads[0], NULL, generate_passwords, &p1);
	thread_status = pthread_create(&threads[1], NULL, generate_passwords, &p2);
	*/

	for (i=0; i< thread_number; i++)
	{
		thread_join_res = pthread_join(threads[i], NULL);
	}
	
	/*
	thread_join_res = pthread_join(threads[0], NULL);
	thread_join_res = pthread_join(threads[1], NULL);
	*/
	/*
	thread_status = pthread_create(&threads[0], NULL, test, NULL);
	*/
	/*
    for (i = 2; i <= argc; i++) {
        if(unrar_test_password(argv[1], argv[i]) == 0) {
            printf("Password is: %s\n", argv[i]);
            goto finish;
        }
    }
	*/

    if (found ==0)
	    printf("Password not found\n");
    
}
