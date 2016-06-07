#include "gsh.h"

int exec_command(char** command, char** parameters)
{
    int status = 0;
    extern int errno;
    pid_t fpid;
    if ((fpid = fork())<0)
        exit(1);
    else if (fpid == 0)
    {
        if (execvp(command[0], parameters) < 0)
        {
            puts(strerror(errno));
            exit(1);
        }
    }
    else
    { 
        while (wait(&status) != fpid)
            continue;
    }
    /*
    if (fork() != 0)
        waitpid(-1, &status, 0);
    else
        execvp(command[0], parameters);
    return status;
    */
    return status;
}
/*
void error_notfound (void)
{
    puts("Command Not Found");
}

void error_fail (void)
{
    puts("Execution Failure");
}
*/
