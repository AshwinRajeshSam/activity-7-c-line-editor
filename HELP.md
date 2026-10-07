# Help Manual — C Line Editor

This guide details all commands supported by the C Line Editor with syntax and examples.

---

### 1. `display`
Prints the entire document with 1-indexed line numbers.
```
editor> display
--- Document Contents (2 lines) ---
   1 | Welcome to C Line Editor.
   2 | Written by Ashwin Rajesh Sam.
--------------------------------------
```

### 2. `insert <line_number> <text>`
Inserts text at the designated line number, pushing existing lines downward.
```
editor> insert 1 First introductory line.
Inserted line 1.
```

### 3. `delete <line_number>`
Removes the line at the specified index and shifts subsequent lines up.
```
editor> delete 2
Deleted line 2.
```

### 4. `save <filename>`
Persists the in-memory document to a standard `.txt` file.
```
editor> save notes.txt
Successfully saved 3 lines to 'notes.txt'.
```

### 5. `load <filename>`
Loads an existing file from disk into memory, replacing current document contents.
```
editor> load notes.txt
Successfully loaded 3 lines from 'notes.txt'.
```

### 6. `search <phrase>`
Finds and displays all lines matching the query string.
```
editor> search Ashwin
Search results for 'Ashwin':
  Line 2: Written by Ashwin Rajesh Sam.
Total matches: 1
```

### 7. `replace <line_number|0> <old_word> <new_word>`
Replaces occurrences of `old_word` with `new_word`. Pass `0` for document-wide replacement.
```
editor> replace 0 Draft Final
Replaced 3 occurrence(s).
```

### 8. `stats`
Displays total line, word, and character counts.
```
editor> stats
=== Document Statistics ===
  Lines     : 5
  Words     : 32
  Characters: 198
===========================
```

### 9. `undo`
Reverts the document to the state prior to the most recent modification.
```
editor> undo
Undo successful. Document restored to previous state.
```

### 10. `exit` or `quit`
Safely cleans up all allocated memory and exits the program.
