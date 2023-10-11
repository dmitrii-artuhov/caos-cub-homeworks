# Homework 4

## Artiukhov Dmitrii

### Task 1:

Since I am tossing coins with the following function (where `rand()` is pretty expensive operation) I got slower results compared to the version provided in the task description.

```c
void flip_coin(int i) {
    ...
    // random coin toss
    if (rand() & 1) { // checking if the random number is odd
        coins[i] = 'O';
    }
    else { // if even
        coins[i] = 'X';
    }
}
```

The measurements:

```txt
threads: 100, flips: 10000

coins: OOOOOOOOOOOOOOOOOOOO (state - global lock)
coins: OOXOOXXXXOXOXOXOXOOO (end - global lock)
100 threads x 10000 flips: 270.646ms
coins: OOOOOOOOOOOOOOOOOOOO (state - iteration lock)
coins: OXXOXOOOOOOOXXOXXXOX (end - iteration lock)
100 threads x 10000 flips: 7444.508ms
coins: OOOOOOOOOOOOOOOOOOOO (state - coin lock)
coins: OOOOXOOXOXXXOOOXOOOO (end - coin lock)
100 threads x 10000 flips: 25639.162ms
```