#include "unrar.h"

#include <strings.h>
#include <stdio.h>

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

int 
main (int argc, char const * argv[])
{
    int i;
    
    if (argc < 2) {
        usage();
        return 1;
    }
    
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
