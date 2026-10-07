#include "editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help(void) {
    printf("\nSupported Commands:\n");
    printf("  display                   : Show all lines with line numbers\n");
    printf("  insert <line> <text>      : Insert text at given line number\n");
    printf("  delete <line>             : Delete the specified line\n");
    printf("  save <filename>           : Save document to disk\n");
    printf("  load <filename>           : Load document from disk\n");
    printf("  search <text>             : Search for text across document\n");
    printf("  replace <line> <old> <new>: Replace in specific line (use 0 for all lines)\n");
    printf("  stats                     : Show line, word, and character counts\n");
    printf("  undo                      : Undo the last modification\n");
    printf("  help                      : Print this help message\n");
    printf("  exit / quit               : Quit the editor\n\n");
}

int main(int argc, char *argv[]) {
    printf("====================================================\n");
    printf("  Simple Line Editor in C (Studio Course B25CS0311) \n");
    printf("  Author: Ashwin Rajesh Sam (SRN: R25EJ019)         \n");
    printf("  Type 'help' for command list, 'exit' to quit.     \n");
    printf("====================================================\n");

    Document *doc = doc_create();
    UndoStack undo_stack;
    undo_init(&undo_stack);

    if (argc > 1) {
        doc_load(doc, argv[1]);
    }

    char input[MAX_LINE_LEN];
    while (1) {
        printf("editor> ");
        if (!fgets(input, sizeof(input), stdin)) break;

        // Strip newline
        size_t len = strlen(input);
        if (len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r')) input[--len] = '\0';
        if (len > 0 && input[len - 1] == '\r') input[--len] = '\0';

        if (len == 0) continue;

        char cmd[64] = {0};
        char arg1[MAX_LINE_LEN] = {0};
        char arg2[MAX_LINE_LEN] = {0};
        char arg3[MAX_LINE_LEN] = {0};

        int parsed = sscanf(input, "%63s %s %s %s", cmd, arg1, arg2, arg3);

        if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0) {
            printf("Exiting line editor. Goodbye!\n");
            break;
        } else if (strcmp(cmd, "help") == 0) {
            print_help();
        } else if (strcmp(cmd, "display") == 0) {
            doc_display(doc);
        } else if (strcmp(cmd, "stats") == 0) {
            doc_stats(doc);
        } else if (strcmp(cmd, "undo") == 0) {
            if (undo_pop(&undo_stack, doc)) {
                printf("Undo successful. Document restored to previous state.\n");
                doc_display(doc);
            } else {
                printf("Nothing to undo.\n");
            }
        } else if (strcmp(cmd, "insert") == 0) {
            if (parsed < 2) {
                printf("Usage: insert <line_number> <text>\n");
                continue;
            }
            size_t line_num = (size_t)atoi(arg1);
            // Extract the rest of line after line_num
            char *text_start = strstr(input, arg1);
            if (text_start) {
                text_start += strlen(arg1);
                while (*text_start == ' ') text_start++;
                undo_push(&undo_stack, doc);
                if (doc_insert(doc, line_num, text_start)) {
                    printf("Inserted line %zu.\n", line_num);
                }
            }
        } else if (strcmp(cmd, "delete") == 0) {
            if (parsed < 2) {
                printf("Usage: delete <line_number>\n");
                continue;
            }
            size_t line_num = (size_t)atoi(arg1);
            undo_push(&undo_stack, doc);
            if (doc_delete(doc, line_num)) {
                printf("Deleted line %zu.\n", line_num);
            }
        } else if (strcmp(cmd, "save") == 0) {
            if (parsed < 2) {
                printf("Usage: save <filename>\n");
                continue;
            }
            doc_save(doc, arg1);
        } else if (strcmp(cmd, "load") == 0) {
            if (parsed < 2) {
                printf("Usage: load <filename>\n");
                continue;
            }
            undo_push(&undo_stack, doc);
            doc_load(doc, arg1);
        } else if (strcmp(cmd, "search") == 0) {
            char *query = input + 6;
            while (*query == ' ') query++;
            if (*query) {
                doc_search(doc, query);
            } else {
                printf("Usage: search <phrase>\n");
            }
        } else if (strcmp(cmd, "replace") == 0) {
            if (parsed < 4) {
                printf("Usage: replace <line_num|0> <old_word> <new_word>\n");
                continue;
            }
            size_t line_num = (size_t)atoi(arg1);
            undo_push(&undo_stack, doc);
            int count = doc_replace(doc, line_num, arg2, arg3);
            printf("Replaced %d occurrence(s).\n", count);
        } else {
            printf("Unknown command '%s'. Type 'help' for instructions.\n", cmd);
        }
    }

    undo_clear(&undo_stack);
    doc_free(doc);
    return 0;
}
