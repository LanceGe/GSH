#ifndef PROMPT_H
#define PROMPT_H
#include "color.h"

#define MAX_PROMPT 1024
#define MAX_PATH_LEN 1024

struct passwd* pwd;

void type_prompt(char* pmt);
#endif
