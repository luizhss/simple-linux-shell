# Simple implementation of a Linux Shell in C

## How to execute
  
  Clone the repository, navigate to its path and use the following command in terminal:
  
      make all
      ./shell
      
## Commands supported:
  
   Builtin commands:
   
   `fg <PID>`: 
    put a process on foreground.
    
   `bg <PID>`: 
    put a process on background.

   `jobs`: 
    list jobs executing on background.
    
   `kill <PID> <SIGNAL_NUMBER>`:
    send a signal to a process.
    
   `history`:
    list the last 50 commands.
    
   `export <ENV_VARIABLE>=<NEW_VALUE>`:
    set a environment variable.
   
   `cd <PATH>`:
    change current directory. One can pass the relative or full path.
    
   `set`:
    list environment variables.
    
   `echo <STRING>`:
     display the string to stdout.
  
  One can execute programs that are on PATH and use 'Pipe' between consecutive commands using the vertical bar `|`.
