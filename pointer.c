#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

void get_from_screen(char *);

int main()
{ 
    char c;
    int len_max = 60;
    char command[len_max];
    int i=0;
    char *ptr_command;
    char *ptr_instruction;
    char delim[1] = ",";
    while (( c = getchar() ) != '\n'){
        command[i++] = (char)c;
        if (i == len_max)
            break;
    }
    command[i] = '\0';
    printf("%s -->len = %zd\n",command,strlen(command));

    for (i = 1, ptr_command = command; ; i++, ptr_command = NULL)
    {
        ptr_instruction = strtok(ptr_command,delim);
        if (ptr_instruction == NULL)
            break;
        printf("%d: %s\n", i, ptr_instruction);

    }
    return 0;
}

void get_from_screen(char *pStr){
    int len_max = 128;
    int current_size = 0;
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
        //        free(pStr);
        //        pStr = NULL;


    }
}
