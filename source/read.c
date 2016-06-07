#include "gsh.h"
//#define DEBUG
int read_command(char** command,char** parameters,char* prompt)
{
    free(buffer);
    buffer = readline(prompt);
    if (feof(stdin))
    {
        printf("\n");
        exit(0);
    }

    return line_proc(command, parameters, buffer);
}

int read_script(char** command,char** parameters, FILE* script)
{
    free(buffer);
    buffer = (char*) malloc(MAXLINE * sizeof(char));
    fgets(buffer, MAXLINE, script);
    if (feof(script))
    //{
    //    printf("\n");
        exit(0);
    //}

    return line_proc(command, parameters, buffer);
}

int line_proc(char** command, char** parameters, char* buffer)
{
    if (buffer == NULL)
    {
        printf("\n");
        exit(0);
    }
    if (buffer[0] == '\0' || buffer[0] == '\n')
        return -1;
    char *begin, *end;
    int count = 0;
    bool finish = false;
    begin = end = buffer;
    while(! finish)
    {
        while (isblank(*begin) && isblank(*end))
        {
            begin++;
            end++;
        }
        if (*end == '\0' || *end == '\n')
        {
            if (count == 0)
                return -1;
            break;
        }
        while(isgraph(*end))
            end++;
        if (count == 0)
        {
            char* p = end;
            command[0] = begin;
            while(p != begin && *p != '/')
                p--;
            parameters[0] = p;
            count += 2;
#ifdef DEBUG
            printf("\ncommand:%s\n",*command);
#endif
        }
        else if (count <= MAXARG)
        {
            parameters[count-1] = begin;
            count++;
        }
        else break;
        if (*end == '\0' || *end == '\n')
        {
            *end = '\0';
            finish = true;
        }
        else
        {
            *end = '\0';
            end++;
            begin = end;
        }
    }

    parameters[count-1] = NULL;
#ifdef DEBUG
    /*input analysis*/
    printf("input analysis:\n");
    printf("pathname:[%s]\ncommand:[%s]\nparameters:\n",*command,parameters[0]);
    int i;
    for(i=0;i<count-1;i++)
        printf("[%s]\n",parameters[i]);
#endif
    return count;
}
