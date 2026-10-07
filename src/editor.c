#include "editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Document* doc_create(void) {
    Document *doc = (Document*)malloc(sizeof(Document));
    if (!doc) return NULL;
    doc->capacity = 16;
    doc->count = 0;
    doc->lines = (char**)malloc(sizeof(char*) * doc->capacity);
    if (!doc->lines) {
        free(doc);
        return NULL;
    }
    return doc;
}

void doc_free(Document *doc) {
    if (!doc) return;
    for (size_t i = 0; i < doc->count; i++) {
        free(doc->lines[i]);
    }
    free(doc->lines);
    free(doc);
}

static bool doc_ensure_capacity(Document *doc) {
    if (doc->count >= doc->capacity) {
        size_t new_cap = doc->capacity * 2;
        char **new_lines = (char**)realloc(doc->lines, sizeof(char*) * new_cap);
        if (!new_lines) return false;
        doc->lines = new_lines;
        doc->capacity = new_cap;
    }
    return true;
}

bool doc_insert(Document *doc, size_t line_num, const char *text) {
    if (!doc || !text) return false;
    // 1-indexed insertion: valid range 1 to doc->count + 1
    if (line_num < 1 || line_num > doc->count + 1) {
        fprintf(stderr, "Error: Invalid line number %zu. Valid range: 1 to %zu.\n", line_num, doc->count + 1);
        return false;
    }
    if (!doc_ensure_capacity(doc)) {
        fprintf(stderr, "Error: Out of memory.\n");
        return false;
    }

    size_t idx = line_num - 1;
    // Shift elements down
    for (size_t i = doc->count; i > idx; i--) {
        doc->lines[i] = doc->lines[i - 1];
    }
    doc->lines[idx] = strdup(text);
    doc->count++;
    return true;
}

bool doc_delete(Document *doc, size_t line_num) {
    if (!doc || doc->count == 0) {
        fprintf(stderr, "Error: Document is empty.\n");
        return false;
    }
    if (line_num < 1 || line_num > doc->count) {
        fprintf(stderr, "Error: Invalid line number %zu. Document has %zu lines.\n", line_num, doc->count);
        return false;
    }

    size_t idx = line_num - 1;
    free(doc->lines[idx]);
    for (size_t i = idx; i < doc->count - 1; i++) {
        doc->lines[i] = doc->lines[i + 1];
    }
    doc->count--;
    return true;
}

void doc_display(const Document *doc) {
    if (!doc || doc->count == 0) {
        printf("--- (Document is currently empty) ---\n");
        return;
    }
    printf("--- Document Contents (%zu lines) ---\n", doc->count);
    for (size_t i = 0; i < doc->count; i++) {
        printf("%4zu | %s\n", i + 1, doc->lines[i]);
    }
    printf("--------------------------------------\n");
}

bool doc_save(const Document *doc, const char *filename) {
    if (!doc || !filename) return false;
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Unable to open file '%s' for writing.\n", filename);
        return false;
    }
    for (size_t i = 0; i < doc->count; i++) {
        fprintf(fp, "%s\n", doc->lines[i]);
    }
    fclose(fp);
    printf("Successfully saved %zu lines to '%s'.\n", doc->count, filename);
    return true;
}

bool doc_load(Document *doc, const char *filename) {
    if (!doc || !filename) return false;
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Unable to open file '%s' for reading.\n", filename);
        return false;
    }

    // Clear existing document lines
    for (size_t i = 0; i < doc->count; i++) {
        free(doc->lines[i]);
    }
    doc->count = 0;

    char buffer[MAX_LINE_LEN];
    while (fgets(buffer, sizeof(buffer), fp)) {
        // Strip trailing newline
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[--len] = '\0';
        }
        if (len > 0 && buffer[len - 1] == '\r') {
            buffer[--len] = '\0';
        }
        doc_insert(doc, doc->count + 1, buffer);
    }
    fclose(fp);
    printf("Successfully loaded %zu lines from '%s'.\n", doc->count, filename);
    return true;
}

void doc_search(const Document *doc, const char *query) {
    if (!doc || !query || strlen(query) == 0) {
        printf("Search query cannot be empty.\n");
        return;
    }
    size_t matches = 0;
    printf("Search results for '%s':\n", query);
    for (size_t i = 0; i < doc->count; i++) {
        if (strstr(doc->lines[i], query) != NULL) {
            printf("  Line %zu: %s\n", i + 1, doc->lines[i]);
            matches++;
        }
    }
    if (matches == 0) {
        printf("  No matches found.\n");
    } else {
        printf("Total matches: %zu\n", matches);
    }
}

static char* replace_in_string(const char *orig, const char *find_str, const char *rep_str, int *replacements) {
    char buffer[MAX_LINE_LEN * 2] = {0};
    const char *p = orig;
    size_t find_len = strlen(find_str);
    char *dest = buffer;

    while (*p) {
        if (strstr(p, find_str) == p) {
            strcpy(dest, rep_str);
            dest += strlen(rep_str);
            p += find_len;
            (*replacements)++;
        } else {
            *dest++ = *p++;
        }
    }
    *dest = '\0';
    return strdup(buffer);
}

int doc_replace(Document *doc, size_t line_num, const char *find_str, const char *replace_str) {
    if (!doc || !find_str || !replace_str || strlen(find_str) == 0) return 0;
    int count = 0;

    if (line_num == 0) {
        // Replace globally across document
        for (size_t i = 0; i < doc->count; i++) {
            if (strstr(doc->lines[i], find_str)) {
                char *new_line = replace_in_string(doc->lines[i], find_str, replace_str, &count);
                free(doc->lines[i]);
                doc->lines[i] = new_line;
            }
        }
    } else if (line_num <= doc->count) {
        size_t idx = line_num - 1;
        if (strstr(doc->lines[idx], find_str)) {
            char *new_line = replace_in_string(doc->lines[idx], find_str, replace_str, &count);
            free(doc->lines[idx]);
            doc->lines[idx] = new_line;
        }
    } else {
        fprintf(stderr, "Error: Line number %zu is out of range.\n", line_num);
    }
    return count;
}

void doc_stats(const Document *doc) {
    if (!doc) return;
    size_t lines = doc->count;
    size_t words = 0;
    size_t chars = 0;

    for (size_t i = 0; i < doc->count; i++) {
        const char *p = doc->lines[i];
        chars += strlen(p);
        bool in_word = false;
        while (*p) {
            if (isspace((unsigned char)*p)) {
                in_word = false;
            } else if (!in_word) {
                in_word = true;
                words++;
            }
            p++;
        }
    }

    printf("=== Document Statistics ===\n");
    printf("  Lines     : %zu\n", lines);
    printf("  Words     : %zu\n", words);
    printf("  Characters: %zu\n", chars);
    printf("===========================\n");
}

/* Undo Implementation */
void undo_init(UndoStack *stack) {
    stack->top = -1;
}

void undo_push(UndoStack *stack, const Document *doc) {
    if (!stack || !doc) return;
    // If full, free oldest
    if (stack->top == MAX_UNDO_DEPTH - 1) {
        for (size_t i = 0; i < stack->snapshots[0].count; i++) {
            free(stack->snapshots[0].lines[i]);
        }
        free(stack->snapshots[0].lines);
        for (int i = 0; i < MAX_UNDO_DEPTH - 1; i++) {
            stack->snapshots[i] = stack->snapshots[i + 1];
        }
        stack->top--;
    }

    stack->top++;
    DocumentSnapshot *snap = &stack->snapshots[stack->top];
    snap->count = doc->count;
    snap->lines = (char**)malloc(sizeof(char*) * (doc->count > 0 ? doc->count : 1));
    for (size_t i = 0; i < doc->count; i++) {
        snap->lines[i] = strdup(doc->lines[i]);
    }
}

bool undo_pop(UndoStack *stack, Document *doc) {
    if (!stack || stack->top < 0 || !doc) return false;

    // Free current doc lines
    for (size_t i = 0; i < doc->count; i++) {
        free(doc->lines[i]);
    }

    DocumentSnapshot *snap = &stack->snapshots[stack->top];
    if (snap->count >= doc->capacity) {
        doc->capacity = snap->count + 8;
        doc->lines = (char**)realloc(doc->lines, sizeof(char*) * doc->capacity);
    }
    doc->count = snap->count;
    for (size_t i = 0; i < snap->count; i++) {
        doc->lines[i] = snap->lines[i];
    }
    free(snap->lines);
    stack->top--;
    return true;
}

void undo_clear(UndoStack *stack) {
    while (stack->top >= 0) {
        DocumentSnapshot *snap = &stack->snapshots[stack->top];
        for (size_t i = 0; i < snap->count; i++) {
            free(snap->lines[i]);
        }
        free(snap->lines);
        stack->top--;
    }
}
