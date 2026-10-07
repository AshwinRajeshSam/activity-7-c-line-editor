#ifndef EDITOR_H
#define EDITOR_H

#include <stddef.h>
#include <stdbool.h>

#define MAX_LINE_LEN 1024
#define MAX_UNDO_DEPTH 20

typedef struct {
    char **lines;
    size_t count;
    size_t capacity;
} Document;

typedef struct {
    char **lines;
    size_t count;
} DocumentSnapshot;

typedef struct {
    DocumentSnapshot snapshots[MAX_UNDO_DEPTH];
    int top;
} UndoStack;

/* Document lifecycle */
Document* doc_create(void);
void doc_free(Document *doc);

/* Core features */
bool doc_insert(Document *doc, size_t line_num, const char *text);
bool doc_delete(Document *doc, size_t line_num);
void doc_display(const Document *doc);
bool doc_save(const Document *doc, const char *filename);
bool doc_load(Document *doc, const char *filename);

/* Bonus features */
void doc_search(const Document *doc, const char *query);
int doc_replace(Document *doc, size_t line_num, const char *find_str, const char *replace_str);
void doc_stats(const Document *doc);

/* Undo management */
void undo_init(UndoStack *stack);
void undo_push(UndoStack *stack, const Document *doc);
bool undo_pop(UndoStack *stack, Document *doc);
void undo_clear(UndoStack *stack);

#endif /* EDITOR_H */
