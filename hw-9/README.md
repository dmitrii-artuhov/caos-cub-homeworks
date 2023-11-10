# Homework 9

## Artiukhov Dmitrii

### Project structure:
- My solution source code is in `src` folder.
- JSON library `jansson` sources are in `3rd-party` folder.

### Build & Run:
```bash
make lib # build & install jansson library
make # build my solution 
./make-build/quiz # run my solution
```

### Build details:

If you already have `jansson` installed on your system, then `make lib` may fail at the end of execution (when `install` stage is happening). You might ignore this error if you installed version of `jansson` is at least `2.14`. Otherwise, run all three commands with `sudo` (it will remove previously installed `jannson` from your system and override it with mine).