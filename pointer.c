#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

int main()
{ 
    char c;
    int len_max = 60;
    char command[len_max];
    int i=0;
    char *ptr_command;
    char *ptr_instruction;
    char delim[1] = ",";
    while(1){
        i = 0;
        printf("ingrese:\n");
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
            printf("(%d) %s\n", i, ptr_instruction);
            //printf("--->%d : n == %s\n",strcmp("n",ptr_instruction),ptr_instruction);
            //strcmp == 0 cuando es igual
        }
        c = '0';
    }
    return 0;
}

