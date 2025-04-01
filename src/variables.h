#ifndef __VARIABLES_H__
#define __VARIABLES_H__

typedef struct var
{
    /* data */
    char *name;
    char *value;
    struct var *next;
}var;
void setVar(char *name, char *value);
char *getVar(char *name);
void expand_variable(const char *input, char *output);

#endif
//pro and con
//dynamic