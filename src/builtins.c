#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"



// ====== Command execution =====

/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd) {
    ssize_t cmd_num = 0;
    while (cmd_num < BUILTINS_COUNT &&
           strncmp(BUILTINS[cmd_num], cmd, MAX_STR_LEN) != 0) {
        cmd_num += 1;
    }
    return BUILTINS_FN[cmd_num];
}


// ===== Builtins =====

/* Prereq: tokens is a NULL terminated sequence of strings.
 * Return 0 on success and -1 on error ... but there are no errors on echo. 
 */
ssize_t bn_echo(char **tokens) {
    ssize_t index = 1;

    if (tokens[index] != NULL) {
        while (tokens[index] != NULL) {
            size_t i = 0;

            while (tokens[index][i] != '\0') {
                // char expanded_value[MAX_STR_LEN];
                // expand_variable(tokens[index], expanded_value);  // 展开变量
    
                // // 显示解析后的内容
                // display_message(expanded_value);
    

                if (tokens[index][i] == '$') {
                    if (tokens[index][i+1]=='\0'){
                        display_message("$");
                        break;
                    }
                    // Start of variable expansion
                    size_t var_start = i + 1;
                    size_t var_end = var_start;
    
                    // Find where the variable name ends (whitespace, another $, or end of string)
                    while (tokens[index][var_end] != '\0' && 
                           tokens[index][var_end] != ' ' && 
                           tokens[index][var_end] != '$') {
                        var_end++;
                    }
    
                    // Extract the variable name
                    char var_name[MAX_STR_LEN];
                    strncpy(var_name, tokens[index] + var_start, var_end - var_start);
                    var_name[var_end - var_start] = '\0';  // Null-terminate the variable name
    
                    // Retrieve the value of the variable
                    char *value = getVar(var_name);
                    // display_message("variable, name: ");
                    // display_message(var_name);
                    // display_message(" value: ");
                    display_message(value);
                    // display_message("\n");
    
                    // Move the index to the end of the variable
                    i = var_end;
                } else {
                    // Display the current character if not part of a variable
                    // display_message("plain txt: ");
                    char txt[2] = {tokens[index][i], '\0'}; 
                    display_message(txt);
                    // display_message("\n");
                    i++;
                }
            }
            if (tokens[index + 1] != NULL) {
                display_message(" ");
            }
            index += 1;
        }
    }

    display_message("\n");

    return 0;
}

void list_directory(const char *path, const char *filter, int depth, int max_depth) {

    if (max_depth != -1 && depth > max_depth) {
        return;
    }

    DIR *dir = opendir(path);
    if (!dir) {
        display_error("ERROR: Invalid path"," ");
        display_error("ERROR: Builtin failed: ls"," ");

        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            // 如果在顶层，并且 (filter==NULL 或名字里包含filter)，就打印
            if (depth == 0 && (!filter || strstr(entry->d_name, filter))) {
                display_message(entry->d_name);
                display_message("\n");
            }
            // 不进入子目录，直接跳过
            continue;
        }

        if (!filter || strstr(entry->d_name, filter)) {
            display_message(entry->d_name);
            display_message("\n");
        }

        if (entry->d_type == DT_DIR) {


            // 构造子目录路径
            char sub_path[MAX_STR_LEN];
            int ret = snprintf(sub_path, MAX_STR_LEN, "%s/%s", path, entry->d_name);
            if (ret < 0 || ret >= MAX_STR_LEN) {
                // 额外错误处理
                display_error("ERROR: Path too long: ", sub_path);
                closedir(dir);
                display_error("ERROR: Builtin failed: ls"," ");
                return;
            }
            // 递归进去
            if (max_depth == -1 || depth < max_depth) {
                list_directory(sub_path, filter, depth + 1, max_depth);
            }
        }

    }
    closedir(dir);
}

static void expand_path(const char *in_path, char *expanded_path) {
    size_t i = 0;         // 扫描输入字符串的游标
    size_t out_pos = 0;   // 写入输出字符串的游标
    
    // 确保输出起始为空
    expanded_path[0] = '\0';
    
    while (in_path[i] != '\0' && out_pos < (MAX_STR_LEN - 1)) {
        if (in_path[i] == '$') {
            // 如果 '$' 后面没有字符，直接输出 '$' 并结束
            if (in_path[i + 1] == '\0') {
                expanded_path[out_pos++] = '$';
                break;
            } else {
                // 提取变量名
                size_t var_start = i + 1;
                size_t var_end = var_start;
                
                // 找到变量名的结束位置：空格、下一个 '$' 或字符串结尾
                while (in_path[var_end] != '\0' &&
                       in_path[var_end] != ' ' &&
                       in_path[var_end] != '$') {
                    var_end++;
                }
                
                // 拷贝变量名
                char var_name[MAX_STR_LEN];
                size_t var_len = var_end - var_start;
                if (var_len >= MAX_STR_LEN) {
                    var_len = MAX_STR_LEN - 1;  // 截断处理
                }
                strncpy(var_name, in_path + var_start, var_len);
                var_name[var_len] = '\0';
                
                // 获取变量值
                char *value = getVar(var_name);
                if (value == NULL) {
                    // 若变量不存在，可视需求处理，这里直接当作空字符串
                    value = "";
                }
                
                // 将变量值拷贝到 expanded_path
                size_t value_len = strlen(value);
                if (out_pos + value_len < MAX_STR_LEN) {
                    strcpy(expanded_path + out_pos, value);
                    out_pos += value_len;
                } else {
                    // 超过缓冲区长度，截断或报错
                    size_t space_left = MAX_STR_LEN - 1 - out_pos;
                    strncpy(expanded_path + out_pos, value, space_left);
                    out_pos += space_left;
                    expanded_path[out_pos] = '\0';
                    break; // 缓冲区已满，结束处理
                }
                
                // 移动输入游标到变量名结尾处
                i = var_end;
            }
        } else {
            // 普通字符，直接拷贝
            expanded_path[out_pos++] = in_path[i++];
        }
    }
    // 追加字符串结束符
    expanded_path[out_pos] = '\0';
}


ssize_t bn_ls(char **tokens) {
    char *path = ".";  // Default to current directory
    char *filter = NULL;
    int recursive = 0;
    int max_depth = -1; // -1 means no depth limit
    char expanded_path[MAX_STR_LEN] = ".";

    // Parse arguments
    for (int i = 1; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "--f") == 0) {
            if (tokens[i + 1] == NULL) {
                display_error("ERROR: Too many arguments: ", "--f requires a substring");
                return -1;
            }
            filter = tokens[++i];
        } else if (strcmp(tokens[i], "--rec") == 0) {
            // display_message("--rec detect\n");

            recursive = 1;
        } else if (strcmp(tokens[i], "--d") == 0) {
            // display_message("--d detect\n");
            if (tokens[i + 1] == NULL) {
                display_error("ERROR: Too many arguments: ", "--d requires a depth value");
                return -1;
            }
            max_depth = atoi(tokens[++i]);

            // char buffer[20];  // 用于存储转换后的字符串
            // sprintf(buffer, "Max depth: %d", max_depth);
            // display_message(buffer);
            // display_message("\n");


            if (max_depth <= 0) {
                display_error("ERROR: Invalid depth: ", tokens[i]);
                return -1;
            }
        } else {
            path = tokens[i];
            expand_path(path, expanded_path);
            
            // 注意：expanded_path 是局部缓冲，如果后面还要使用，
            // 这里可以 strdup() 到 path 中。也可直接用 expanded_path。

        }
    }

    // Validate --d without --rec
    if (max_depth != -1 && !recursive) {
        display_error("ERROR: Invalid usage: ", "--d must be used with --rec");
        return -1;
    }

    // Call directory listing function

    list_directory(expanded_path, filter, 0, max_depth);
    return 0;
}

ssize_t bn_cd(char **tokens) {
    if (tokens[1] == NULL) {
        display_error("ERROR: Missing argument: ", "cd requires a path");
        return -1;
    }

    char *expanded_path = tokens[1];

    // Handle ... and .... manually
    if (strcmp(expanded_path, "...") == 0) {
        expanded_path = "../..";
    } else if (strcmp(expanded_path, "....") == 0) {
        expanded_path = "../../..";
    }

    if (chdir(expanded_path) != 0) {
        display_error("ERROR: Invalid path: ", expanded_path);
        return -1;
    }
    return 0;
}
ssize_t bn_cat(char **tokens) {
    if (tokens[1] == NULL) {
        display_error("ERROR: ", "No input source provided");
        return -1;
    }

    FILE *file = fopen(tokens[1], "r");
    if (!file) {
        display_error("ERROR: ", "Cannot open file");
        return -1;
    }

    char buffer[MAX_STR_LEN];
    while (fgets(buffer, MAX_STR_LEN, file)) {
        display_message(buffer);
    }
    fclose(file);
    return 0;
}

ssize_t bn_wc(char **tokens) {
    if (tokens[1] == NULL) {
        display_error("ERROR: ", "No input source provided");
        return -1;
    }

    FILE *file = fopen(tokens[1], "r");
    if (!file) {
        display_error("ERROR: ", "Cannot open file");
        return -1;
    }

    int words = 0, characters = 0, newlines = 0;
    int in_word = 0;
    int c;

    while ((c = fgetc(file)) != EOF) {
        characters++;
        if (c == '\n') newlines++;
        if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            words++;
        }
    }
    fclose(file);

    char output[MAX_STR_LEN];
    snprintf(output, MAX_STR_LEN, "word count %d\ncharacter count %d\nnewline count %d\n", words, characters, newlines);
    display_message(output);
    return 0;
}


// ssize_t bn_cd(char **tokens){
    
// }
// ssize_t bn_cat(char **tokens){
    
// }
// ssize_t bn_wc(char **tokens){
    
// }