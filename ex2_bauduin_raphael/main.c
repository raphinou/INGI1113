#include "unrar.h"
#include "pthread.h"

#include <strings.h>
#include <string.h>
#include <stdio.h>

struct node{
	char * password;
	struct node *next;
};

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
generate_passwords_list(const char* prefix, const char *alphabet, int max_length)
{
	int i, alphabet_length = strlen(alphabet), prefix_length = strlen(prefix);
	char *current_password;
	char * character_added=malloc(sizeof(char));
	current_password=malloc(sizeof(char)*(prefix_length+1));
	printf("- %s\n", prefix);
	if (prefix_length==max_length)
	{
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
			/*
			printf("character added = %s\n", character_added);
			*/
			strcat(current_password, character_added);
			/*
			printf("- %s\n",current_password);
			*/
			generate_passwords_list(current_password, alphabet, max_length);
		}
	}
}

void
generate_passwords(const char *start_letters, const char * alphabet, int max_length)
{
/*	int i, j, k, start_letters_length;
 *	*/
	int i,start_letters_length, alphabet_length;
	char * start_letter=malloc(sizeof(char));
	start_letter=malloc(sizeof(char));
	printf("start of passwords generation\n");
	printf("start letters:%s\n", start_letters);
	start_letters_length = strlen(start_letters);
	alphabet_length = strlen(alphabet);
	printf("start_letter_length = %i\n", start_letters_length);
	for(i=0; i<start_letters_length;i++)
	{
		/*
		printf("start letter: %c\n", start_letters[i]);
		*/
		*start_letter=*(start_letters+i);
		generate_passwords_list(start_letter, alphabet, max_length);
		printf("**************************************\n");
	}
}
			
int 
main (int argc, char const * argv[])
{
    int i, thread_status;
	pthread_t threads[2];
/*    
    if (argc < 2) {
        usage();
        return 1;
    }
*/    
	char  s1[]="abc",s2[]="def",alphabet[]="abcdef";
	generate_passwords(s1, alphabet, 3);
	
	printf("%s,%s,%s", s1,s2,alphabet);
	thread_status = pthread_create(&threads[0], NULL, test, NULL);
    for (i = 2; i <= argc; i++) {
        if(unrar_test_password(argv[1], argv[i]) == 0) {
            printf("Password is: %s\n", argv[i]);
            goto finish;
        }
    }

    printf("Password not found\n");
    
finish:    
    return 0;
}
