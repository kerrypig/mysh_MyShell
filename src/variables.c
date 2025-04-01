#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"



var *vars = NULL;
var *newVar(char *name, char *value){
    // printf("creating new variable:%s,%s\n",name,value);
    var *new_var = malloc(sizeof(var));
    new_var->name=strdup(name);
    new_var->value=strdup(value);
    new_var->next=NULL;
    return new_var;
}
void setVar(char *name, char *value){
    var *currVars = vars;
    char expanded_value[MAX_STR_LEN];
    if (strlen(value) >= MAX_STR_LEN) {
        value[MAX_STR_LEN - 1] = '\0'; // Ensure null termination
    }
    expand_variable(value, expanded_value);
    
    while(currVars!=NULL){
        if(strcmp(currVars->name,name)==0){
            // founded
            // printf("founded: name:%s, initial:%s, final:%s\n",name,currVars->value,expanded_value);
            free(currVars->value);
            currVars->value=strdup(expanded_value);
            return;
        }
        currVars = currVars->next;
    }
    //not founed
    // printf("key:%s,value:%s\n",name,expanded_value);
    //key-value

    var *new=newVar(name,expanded_value);
    new->next=vars;
    vars=new;
}
char *getVar(char *name){
    var *currVars = vars;
    while(currVars!=NULL){
        if(strcmp(currVars->name,name)==0){
            return currVars->value;
        }
        currVars = currVars->next;
    }
    return "";

}
void expand_variable(const char *input, char *output) {
    size_t i = 0, res_index = 0;
    output[0] = '\0';  // 清空输出

    while (input[i] != '\0') {
        // printf("%c\n", input[i]);
        if (input[i] == '$') {
            size_t var_start = i + 1;
            size_t var_end = var_start;

            // 找到变量名的结束位置（空格、另一个 $ 或字符串结束）
            while (input[var_end] != '\0' && 
                   input[var_end] != ' ' && 
                   input[var_end] != '$') {
                var_end++;
            }

            // 提取变量名
            char var_name[MAX_STR_LEN]; 
            strncpy(var_name, input + var_start, var_end - var_start);
            var_name[var_end - var_start] = '\0';
            // printf("%s\n", var_name);


            // 获取变量值
            char *value = getVar(var_name);
            if (value != NULL) {
                strncat(output, value, MAX_STR_LEN - strlen(output) - 1);
            }
            // printf("%s\n", value);
            // printf("%s\n", output);



            i = var_end;  // 移动索引到变量后面
        } else {
            // 直接复制非 $ 的字符
            output[res_index++] = input[i++];
        }
    }

    // output[res_index] = '\0';
    // printf("%s\n", output);

}
//123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
// length of 120
// \0