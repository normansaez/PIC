#include <stdio.h>
#include <stdlib.h>

char *get_from_screen();

int main()
{
    char *command = malloc(128);
    command = (char *) get_from_screen();
    printf("--> %s", command);
    return 0;
}

char *get_from_screen(){
    int len_max = 128;
    int current_size = 0;

    char *pStr = malloc(len_max);
    current_size = len_max;

    printf("\nEnter a very very very long String value:  ");

    if(pStr != NULL)
    {
        int c = EOF;
        int i = 0;
        //accept user input until hit enter or end of file
        while (( c = getchar() ) != '\n' && c != EOF)
        {
            pStr[i++]=(char)c;
            //if i reached maximize size then realloc size
            if(i == current_size)
            {
                current_size = i+len_max;
                pStr = realloc(pStr, current_size);
            }
            printf("%d\n",i);
        }

        pStr[i] = '\0';

        printf("\nLong String value:%s \n\n",pStr);
        free(pStr);
        pStr = NULL;


    }
    return pStr;
}
