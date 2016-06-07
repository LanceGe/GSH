#include "gsh.h"
int shell (FILE* script);

int main (int argc, char** argv)
{
    FILE* script = NULL;
    if (argc > 0)
        script = fopen(argv[1], "r");
    shell(script);
    if (script) fclose(script);
    return 0;
}

int shell (FILE* script)
{
    char** command = (char**)malloc(sizeof(char*));
    char** parameters=(char**)malloc(MAXARG *sizeof(char*));
    char prompt[MAX_PROMPT];
    if (script == NULL)
    {
        while (true)
        {
            type_prompt(prompt);
            read_command(command, parameters, prompt);
            if (builtin_command(command, parameters) == 0)
                exec_command(command, parameters);
        }
    }
    else
    {
        while (true)
        {
            read_script(command, parameters, script);
            if (builtin_command(command, parameters) == 0)
                exec_command(command, parameters);
        }
    }
    return 0;
}
