#include "gsh.h"

void type_prompt(char* prompt)
{
    extern struct passwd* pwd;
    char pathname[MAX_PATH_LEN];
    int length = 0;
    pwd = getpwuid(getuid());
    getcwd(pathname,MAX_PATH_LEN);
    //sprintf(prompt, "%s\u2642%s ", 
    //        T_COLOR_LIGHT_BLUE, T_COLOR_LIGHT_GREEN);

    if(strlen(pathname) < strlen(pwd->pw_dir) || 
        strncmp(pathname,pwd->pw_dir,strlen(pwd->pw_dir))!=0)
        sprintf(prompt+length,"%s%s%s", T_COLOR_LIGHT_BLUE, 
                pathname, T_COLOR_NONE);
    else
        sprintf(prompt+length,"%s~%s%s", T_COLOR_LIGHT_BLUE,
                pathname+strlen(pwd->pw_dir), T_COLOR_NONE);

    length = strlen(prompt);

    if(geteuid()==0)
        sprintf(prompt+length,"%s\u2640%s ", 
                T_COLOR_LIGHT_PURPLE, T_COLOR_NONE);
    else
        sprintf(prompt+length,"%s\u2642%s ", 
                T_COLOR_CYAN, T_COLOR_NONE);
}
