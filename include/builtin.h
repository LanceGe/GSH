#ifndef BUILTIN_H
#define BUILTIN_H
int builtin_command (char** command, char** parameters);
int cd (char** parameters);
int cp (char** parameters);
int cmp (char** parameters);
int wc (char** parameters);
int quit (void);
int philosophy (void);
#endif
