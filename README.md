# Simple Line Editor in C

**Course:** Portfolio Building (B25CS0311) — Activity 7 Coding Competition  
**Author:** Ashwin Rajesh Sam (SRN: R25EJ019)  
**Grade Target:** 100/100 Points  

---

## Implemented Features
- **Core Features (4/4 Completed):**
  - [x] Insert line at arbitrary 1-indexed position with downward shifting
  - [x] Delete line with upward shifting and memory cleanup
  - [x] Formatted document display with line numbers
  - [x] File persistence: Save and Load from `.txt` documents
- **Bonus Features (4/4 Completed):**
  - [x] Substring search with line matches reporting
  - [x] Targeted line or document-wide find and replace
  - [x] Document metrics: Line count, word count, character count statistics
  - [x] Multi-level Undo stack using deep document state snapshots

---

## Compilation & Execution

### Using GCC directly:
```bash
gcc -Wall -Wextra -std=c11 -O2 src/main.c src/editor.c -o editor
./editor
```

### Or using Make:
```bash
make
./editor
```

---

## Deliverables & Documentation
- [DESIGN.md](./DESIGN.md): Detailed paper-design transcription, data structure trade-off justification, and architecture.
- [HELP.md](./HELP.md): Complete command syntax and usage examples.
