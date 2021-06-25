# Simple implementation of a Linux Shell in C

## How to execute
  
Clone the repository, navigate to its path and use the following command in terminal:

```bash
make all
./shell
```
  
## Functionalities:
  
   Builtin commands:
   
   * `fg <PID>`: 
    put a process in foreground.
    
   * `bg <PID>`: 
    put a process in background.

   * `jobs`: 
    list jobs executing in background.
    
   * `kill <PID> <SIGNAL_NUMBER>`:
    send a signal to a process.
    
   * `history`:
    list the last 50 commands.
    
   * `export <ENV_VARIABLE>=<NEW_VALUE>`:
    set a environment variable. You may use it to add directories to `MYPATH`. Example:
    
    `export $MYPATH=$MYPATH:/foo`.
   
   * `cd <PATH>`:
    change current directory. One can pass the relative or absolute path.
    
   * `set`:
    list environment variables.
    
   * `echo <STRING>`:
    display the string to stdout. 
    You may use it to print value of environment variables (example `echo $MYPATH`).
    
   * `exit`:
    close shell (You can press `<CTRL+D>` as well).
  
  You can execute programs that are on PATH and use 'Pipe' between consecutive commands using the vertical bar `|`. Multiple pipes are allowed.
  
  Input and output redirection implemented (`<` for stdin, `>` for stdout, `2>` for stderr).
  
  Execute in background with `&` at the end of the command.
  
  `SIGCHILD` handler implemented. This avoids zombie process.
  
  `SIGINT` handler implemented. This means that you can't close the shell pressing `<CTRL+C>`, however you can kill the current program executing in foreground.
  
  `SIGTSTP` handler implemented. This means that you can't stop shell process by pressing `<CTRL+Z>`, however you can stop the current program executing in foreground. You can see the stopped program on `jobs`.
  
  Environment variables `MYPATH` and `MYPS1` are equivalent to `PATH` and `PS1`, respectively. At the beginning, `MYPATH` value is equal to `PATH` and `PS1` is equal to `tecii$ `.
