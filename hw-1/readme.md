# Homework 1

## Artiukhov Dmitrii

### Task 1:

**Solution**: we must change the line inside `strsplit` function.
We need to allocate `(cnt + 2)` (because when we split the string by n delimeters then we get (n+1) different string parts, in addition to that we need the NULL pointer) pointers with the size of `char*` for each pointer.

So it should be like this: `char **splitv = calloc(cnt + 2, sizeof(char*));`

### Task 2:

**Text segment**:
- all instructions

**Data segment**:
- `static char m[]`

**Heap segments**:
- `p = (char *)malloc(len + 1)` in `strndup` function

**Stack segments**:
- Inside `main` function:
    - `len`
    - `n` in for-loop
    - `char* p` pointer
- Inside `strndup` function
    - `const char* s` parameter
    - `n` parameter
    - `char* p` pointer
    - `len` varibale inside if-statement
    - return address to `main`


### Task 3:

See the `3.c` file. Build it with `make` command and then run with `./make-build/env [args]`