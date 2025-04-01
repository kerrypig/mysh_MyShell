#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"




// You can remove __attribute__((unused)) once argc and argv are used.
int main(__attribute__((unused)) int argc, 
         __attribute__((unused)) char* argv[]) {
    char *prompt = "mysh$ "; // TODO Step 1, Uncomment this.

    char input_buf[MAX_STR_LEN + 1];
    input_buf[MAX_STR_LEN] = '\0';
    char *token_arr[MAX_STR_LEN] = {NULL};

    while (1) {
        // Prompt and input tokenization

        // TODO Step 2:
        // Display the prompt via the display_message function.
        display_message(prompt);


        int ret = get_input(input_buf);
        size_t token_count = tokenize_input(input_buf, token_arr);

        // Clean exit
        // TODO: The next line has a subtle issue.
        if (ret != -1 && (token_count == 0 || strcmp("exit", token_arr[0]) == 0)) {
            break;
        }

        // Command execution
        if (token_count >= 1) {
            bn_ptr builtin_fn = check_builtin(token_arr[0]);
            if (builtin_fn != NULL) {
                ssize_t err = builtin_fn(token_arr);
                if (err == - 1) {
                    display_error("ERROR: Builtin failed: ", token_arr[0]);
                }
            } else {
                if (token_count == 1 && strchr(token_arr[0], '=') != NULL){
                    // printf("set variable\n");
                    char *equal_pos = strchr(token_arr[0], '=');
                    *equal_pos = '\0';
                    char *key = token_arr[0];
                    char *value = equal_pos + 1;
                    // printf("key:%s,value:%s\n",key,value);

    
                    setVar(key, value);
                }else{
                    display_error("ERROR: Unknown command: ", token_arr[0]);

                }
                
            }
        }

    }

    return 0;
}
