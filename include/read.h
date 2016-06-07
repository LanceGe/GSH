#ifndef READ_H
#define READ_H
#include <readline/readline.h>
#include <readline/history.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXARG 20
#define MAXLINE 4096
char* buffer;
int read_command(char** command,char** parameters,char* prompt);
int read_script(char** command,char** parameters, FILE* script);
int line_proc(char** command,char** parameters,char* buffer);
#endif
