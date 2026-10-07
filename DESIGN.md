# Engineering Design Document: Simple Line Editor in C

**Course:** Portfolio Building (B25CS0311) — Activity 7  
**Team / Author:** Ashwin Rajesh Sam (SRN: R25EJ019)  
**Platform:** Terminal / CLI (C11, GCC)  

---

## 1. Problem Understanding
Modern text editors (VS Code, Vim, Nano) present 2D cursor addressing across terminal viewports. A classic **line editor** operates strictly along 1D line addressing: users invoke declarative commands targeting specific line indices (or global ranges) without arbitrary cursor motion.

---

## 2. Data Structure Selection & Justification

### Options Considered:
1. **Singly/Doubly Linked List of Lines:**
   - *Pros:* $O(1)$ line insertion and deletion once pointer is located.
   - *Cons:* $O(N)$ random access lookup to navigate to line $k$, severe pointer overhead (16 bytes per node on 64-bit), and memory fragmentation causing CPU cache misses during sequential document rendering.
2. **Dynamic Array of String Pointers (`char **lines`):** (Selected)
   - *Pros:*
     - $O(1)$ instantaneous random access to any line number `lines[k - 1]`.
     - High spatial cache locality when rendering documents (`doc_display`) or computing global word statistics.
     - Minimal memory overhead per line (only a single 8-byte pointer).
     - Amortized $O(1)$ reallocation during growth (`capacity = capacity * 2`).
   - *Trade-off Mitigation:* Inserting or deleting in the middle requires shifting pointer addresses ($O(N)$), but moving 8-byte pointers in memory via fast CPU cache transfers is negligible for documents with thousands of lines, far outperforming linked list traversals.

---

## 3. Architecture & Modular Breakdown

### Key Structs:
```c
typedef struct {
    char **lines;       // Array of dynamically allocated string buffers
    size_t count;       // Current number of lines
    size_t capacity;    // Allocated line pointer capacity
} Document;

typedef struct {
    DocumentSnapshot snapshots[MAX_UNDO_DEPTH];
    int top;
} UndoStack;
```

### Function Signatures:
- `Document* doc_create(void);`
- `bool doc_insert(Document *doc, size_t line_num, const char *text);`
- `bool doc_delete(Document *doc, size_t line_num);`
- `void doc_display(const Document *doc);`
- `bool doc_save(const Document *doc, const char *filename);`
- `bool doc_load(Document *doc, const char *filename);`
- `void doc_search(const Document *doc, const char *query);`
- `int doc_replace(Document *doc, size_t line_num, const char *find_str, const char *rep_str);`
- `void doc_stats(const Document *doc);`
- `bool undo_pop(UndoStack *stack, Document *doc);`

---

## 4. Robustness & Defensive Programming
- Bounds validation prevents segmentation faults on non-existent line numbers or empty document deletes.
- Memory leak prevention: complete deep free of all allocated lines upon document destruction or re-loading.
- Buffer overflow immunity via controlled `fgets` reading and dynamic string allocation.
