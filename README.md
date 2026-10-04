# IPC lab 1: float sum via anonymous pipe (variant 2)

This example shows how to:

- Create new process (`fork`)
- Replace process image (`exec`)
- Create a communication channel (`pipe`)
- Replace file descriptors (`dup2`)
- `write` to / `read` from file descriptors
- `open` / `close` files
- `wait` until child process exits (`waitpid`)

Parent reads the output filename from the first line of terminal input, then
forwards user commands (space-separated floats) via pipe1 connected to the
child's stdin. Child sums the numbers and writes the result to the file.

## Compiling

Bare minimum commands to compile the example:

    cc -o parent parent.c
    cc -o child child.c

Or with cmake:

    cmake -S . -B build && cmake --build build

## Running

Run parent, type the filename on the first line, then type commands until
you press `Ctrl-D`:

    ./parent
    out.txt
    1.5 2.5
    3
    <Ctrl-D>
    cat out.txt

**IMPORTANT**: parent expects the child executable to be named `child` and be
located in the current working directory, otherwise exec fails and parent
reports an error. If you'd like to rename the child executable, you must also
change the path in the `execl` call in `parent.c`.
