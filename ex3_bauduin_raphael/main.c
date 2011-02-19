#include <string.h>
#include <stdio.h>

#include "zip_crack.h"

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
read_dico(char * file) 
/* opens file path passed in parameter, and reads it line per line 
 * each password should be maximum 29 characters long */
{
	FILE * fp;
	int counter=0;
	char password[30]="";
	fp=fopen(file, "r");
	while (counter++<20 && read_word(fp, password))
		printf("PASSSWORD READ is %s\n", password);

}

int 
main (int argc, char const * argv[])
{
    int i;
    struct zip_archive * archive;

	read_dico("dico.txt");

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
