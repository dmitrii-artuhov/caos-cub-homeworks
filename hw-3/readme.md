# Homework 3

## Artiukhov Dmitrii

### Task 1:

- **(a)**: reader code is incorrect, because the last line of code where we check whether the writer semaphore should be released is done out of critical section of the reader:
    ```c
    void reader() { 
        down(&mutex); 
        readcount = readcount + 1;
        if (readcount == 1) down(&writer); 
        up(&mutex); 
        read_shared_object(&data);
        down(&mutex);
        readcount = readcount - 1;
        up(&mutex);
        if (readcount == 0) up(&writer); // this line must be inside a critical section
    }
    ```
    If we have such incorrect code, then the possible situation where it fails is as follows: when first reader finishes reading the shared data, it will decrease the `readcount` variable to 0, then `up(&mutex);` command will release the reader semaphore. The new reader might acquire the `mutex` and increase the `readcount` variable, this will lead to the the deadlock, because first reader will run the if-statement `if (readcount == 0) up(&writer);`, which is not going to release the writer semaphore and the second reader will try to acquire not released semaphore.
- **(b)**: this code releases the writer semaphore in the void reader() {...} function outside of the reader critical section. But it will not lead to the failure in this case, because the rest of the threads when they run reader() of write() functions will eventually wait the current reader to release the write semaphore and no deadlock will happen.
- **(c)**: here the writer code acquire the reader semaphore in the order which might lead to the deadlock. The way it is possible is as follows: reader code runs `down(&mutex);` acquiring the reader semaphore, then other thread runs `down(&writer);` acquiring the writer semaphore. Then both threads try to acquire the ther type of semaphores (reader tries to get writer semaphore and vice versa) which leads to the deadlock.

    ```c
    void writer() {
        down(&writer);
        down(&mutex); // reader's semaphore acquire after the writer's
        write_shared_object(&data);
        up(&mutex);
        up(&writer);
    }
    ```

### Task 2:

Solution is in the zip archive. File names `2.c`. Makefile is also provided.

The plot is saved to file `plot.png`.