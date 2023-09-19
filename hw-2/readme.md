# Homework 2

## Artiukhov Dmitrii

### Task 1:

```asm
main:
    addi sp,sp,-48       # Allocate space for the stack frame by subtracting 48 from the stack pointer (sp).
    sd s0,40(sp)         # Save the value of s0 (saved register) at offset 40 from the stack pointer (sp).
    addi s0,sp,48        # Set s0 to point to the beginning of the stack frame (48 bytes below the current sp).
    mv a5,a0             # Move the value of a0 (function argument) to a5 (temporary register).
    sd a1,-48(s0)        # Store the value of a1 (function argument) at offset -48 from s0 in the stack frame.
    sw a5,-36(s0)        # Store the value of a5 (previously a0) at offset -36 from s0 in the stack frame.
    sw zero,-20(s0)      # Store the value of zero (constant 0) at offset -20 from s0 in the stack frame.
    lw a5,-20(s0)        # Load the value at offset -20 from s0 into a5 (loading the stored zero value).
    mv a0,a5             # Move the value of a5 (zero) to a0 (function return value).
    ld s0,40(sp)         # Restore the original value of s0 by loading it from offset 40 from sp.
    addi sp,sp,48        # Deallocate the stack frame by adding 48 to the stack pointer (sp).
    jr ra                # Jump to the return address stored in ra, effectively returning from the function.
```


Diagram:

```
    High Address
+-------------------+
|                   |
|   ...             |
|                   |
|-------------------|
|  Saved s0 (40)    |  40(sp)
|-------------------|
|  Local a1 (-48)   |  32(sp)
|-------------------|
|  Local a5 (-36)   |  24(sp)
|-------------------|
|  Local zero (-20) |  16(sp)
|-------------------|  <-- s0 (Stack Frame Pointer)
|                   |
|                   |
|                   |
|                   |
|-------------------|  <-- sp (Stack Pointer)
|  Return Address   |  -4(sp)
|-------------------|
|  Saved Registers  |  (various offsets from sp)
|   ...             |
|-------------------|
|  Function Args    |  (negative offsets from sp)
|   ...             |
|-------------------|
     Low Address
```

### Task 2:

Solution is in the zip archive.