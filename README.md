This is a C shell created as part of the operating systems and networks course

# Linux Bash Shell

## Working:

- Using priciples of a Linux Bash Shell in the Linux Operating system,
the User-defined interactive shell program can create and manage new processes.
The shell is able to create a process out of a system program like emacs, vi,
or any user-defined executable

Files

1)main.c

->It contains the main,change_display,checkbgproc,pinfo,execute functions
->Main function runs a while loop unless exit is encountered, It reads commands uses tokenize function 
  from token.c to tokenize the commands and for each tokenized command execute function is called with
  tokenized command array as argument.
->Execute function tokenizes the input string with space and uses if else statements to check the command and implement it
->pinfo gives process information for a given pid through /proc/pid/ directory
->checkbgproc checks if backgrounnd process is running or not and gives respective output if it is exited
->change_display changes the prompt for every directory change

2)ls.c

->Based on the flags -l -a -la -al ls is implemented
->A while loop is used to read the directory files with struct stat and give respective outputs
->For -l localtime function is used retrieve file information

3)token.c

->Using strtok with ; as delimitter commands are tokenized

4)process.c

->Used to create foreground and background processes
->fork is used to create a new process
->For a background process waiting is not required but foreground process waiting is required
->execvp is used

5)history.c

->history is stored and when history command is used history is displayed

You can run the shell from make file and then ./shell
