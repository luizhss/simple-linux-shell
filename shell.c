#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <signal.h>
#include "functions.h"

// utils.c
void whereami(char *);
int parse_args(char *, char **, int);
int get_type_process(char*);
void _add_history(char*, char*);

// builtin.c
void _export(char**, void *);
void _cd(char**, void*);
void _history(char *);
void _kill(char **, void*);
void _jobs(node*);
void _fg(char**, void*);
void _bg(char**, void*);
void _echo(char **);
void _set();

int pid_changed = -1, pid_action = -1;

// Signal handlers
void handle_sigint(int sig){
}

void handle_sigtstp(int sig){
}

void handle_sigchld(int sig){
	pid_t pid; 
	int status;
	pid = waitpid(-1, &status, (WNOHANG | WUNTRACED | WCONTINUED));

	if(pid > 0){
		if(WIFSTOPPED(status)){
			pid_changed = pid;
			pid_action = 1;
		}else if(WIFEXITED(status) || WIFSIGNALED(status)){
			pid_changed = pid;
			pid_action = 0;
		}else if(WIFCONTINUED(status)){
			pid_changed = pid;
			pid_action = 2;
		}
	}
}

extern char **environ;

int main(int argc, char **argv, char **envp){

	pid_t pid;
	char cmd[100], *args[100], wami[200], cmd_aux[200], hpath[500];
	char aux[200], aux2[200], *sptr1;
	void *shmem;
	bool bg = false;
	int t_process;
	int len;

	// Initialize jobs list
	node* jobs = NULL;	

	// Create shared memory
	shmem = mmap(NULL, 200, (PROT_READ | PROT_WRITE), (MAP_SHARED | MAP_ANONYMOUS), -1, 0);

	// Defining signal handles
	signal(SIGINT, handle_sigint);
	signal(SIGTSTP, handle_sigtstp);

	// MYPATH
	setenv("MYPATH", getenv("PATH"), 1);

	// MYPS1
	setenv("MYPS1", "tecii$ ", 1);

	// History path

	if(access(".history", F_OK) != 0){
		FILE* fp = fopen(".history", "w");
		fclose(fp);
	}

	strcpy(hpath, getenv("PWD"));
	strcat(hpath, "/.history");

	// Defining pgid
	pid = getpid();
	setpgid(pid, pid);
	tcsetpgrp(0, pid);

	while(1){

		// Update jobs linked list
		if(pid_changed != -1){
			if(pid_action == 1){
				update_status(&jobs, pid_changed, 0);
			}else if(pid_action == 0){
				del(&jobs, pid_changed);
			}else if(pid_action == 2){
				update_status(&jobs, pid_changed, 1);
			}
			pid_changed = pid_action = -1;
		}

		// Start new shell input
		//whereami(wami);
		fprintf(stdout, "\033[38;5;46m%s\033[m", getenv("MYPS1"));

		// Finish the shell, if Ctrl+D is pressed
		if(fgets(cmd, 100, stdin) == NULL){
			kill_jobs_and_free_memory(jobs);
			exit(0);
		}

		len = strlen(cmd);
		cmd[--len] = '\0';

		// Ignore empty cmds
		if(len == 0) continue;

		// Add cmd to .history file
		_add_history(cmd, hpath);

		// Finish sheel, when 'exit' command
		if (strncmp(cmd, "exit", 4) == 0){
			kill_jobs_and_free_memory(jobs);
			exit(0);
		}

		// Check background execution
		if(cmd[len-1] == '&'){
			cmd[--len] = '\0';
			bg = true;
		}else{
			bg = false;
		}

		// Create empty linked list to parse commands
		cnode *cmds, *ptr;
		cmds = NULL;

		// Parse commands and insert it on a Linked List
		char * _cmd;
		strcpy(cmd_aux, cmd);
		_cmd = strtok_r(cmd_aux, "|", &sptr1);
		while(_cmd != NULL){
			add_cmd(&cmds, _cmd);
			_cmd = strtok_r(NULL, "|", &sptr1);
		}

		// Execute commands
		int fdd = 0;
		int fd[2];
		ptr = cmds;
		while(ptr != NULL){
			// Create pipe between process
			pipe(fd);

			t_process = get_type_process(ptr->cmd);

			pid = fork();
			if(pid < 0){
				perror("fork");
				exit(0);
			}else if(pid == 0){
				//Child

				//Signals
				signal(SIGINT, SIG_DFL);
				signal(SIGQUIT, SIG_DFL);
				signal(SIGTSTP, SIG_DFL);
				signal(SIGTTIN, SIG_DFL);
				signal(SIGTTOU, SIG_DFL);
				signal(SIGCHLD, SIG_DFL);

				//PGID
				setpgid(0, pid);

				// Child process gets input from output of previous process
				// if exists, else it gets input from stdin
				dup2(fdd, 0);

				// If there is a next process, change it output to the pipe
				// between them, else output is stdout
				if(ptr->prox != NULL){
					dup2(fd[1], 1);
				}

				// Close reading pipe 
				close(fd[0]);

				// Arguments parser
				if(parse_args(ptr->cmd, args, t_process) < 0){
					perror(args[0]);
					exit(0);
				}

				// Process command
				switch(t_process){
					case 1:
						_export(args, shmem);
						break;
					case 2:
						_cd(args, shmem);
						break;
					case 3:
						_history(hpath);
						break;
					case 4:
						_kill(args, shmem);
						break;
					case 5:
						_jobs(jobs);
						break;
					case 6:
						_fg(args, shmem);
						break;
					case 7:
						_bg(args, shmem);
						break;
					case 8:
						_echo(args);
						break;
					case 9:
						_set();
						break;
					case 0:
						if(execv(args[0], args) < 0){
							perror(args[0]);
							exit(0);
						}
						break;
				}

			}else{
				//Parent

				//SIGNALS
				signal(SIGCHLD, handle_sigchld);

				//PGID
				setpgid(pid, pid);
				
				if(bg){
					// Background

					insert(&jobs, pid, 1);
					fprintf(stdout, "[%d]\n", pid);

				}else{
					// Foreground

					// Send job to fg
					tcsetpgrp(0, pid);

					// Wait it finish
					int status;
					waitpid(pid, &status, WUNTRACED);

					// return shell to fg
					signal(SIGTTOU, SIG_IGN);
				        tcsetpgrp(0, getpid());
				        signal(SIGTTOU, SIG_DFL);

					// Close writing pipe
					close(fd[1]);

					// Save the actual pipe for the next command parsed
					fdd = fd[0];

					if(WIFEXITED(status)){

						if(t_process == 1){
							// export

							char* s = (char*) shmem;
							int sz = strlen(s);
							int i = 0, j = 0;

							for(; i < sz && s[i] != '='; i++){
								aux[i] = s[i];
							}
							aux[i++] = '\0';
							for(; i < sz; i++, j++){
								aux2[j] = s[i];
							}
							aux2[j] = '\0';

							if(setenv(aux, aux2, 1) < 0){
								perror("export");
							}

						}else if(t_process == 2){
							// cd

							if(chdir(shmem) != -1){
								setenv("PWD", shmem, 1);
							}else{
								perror("cd");
							}

						}else if(t_process == 4){
							// kill

							int pid_sig, sig;
							sscanf(shmem, "%d %d", &pid_sig, &sig);

							if(kill(pid_sig, sig) < 0){
								perror("kill");
							}

							sleep(1);

						}else if(t_process == 6){
							// fg

							int pid_fg;
							sscanf(shmem, "%d", &pid_fg);

							if(kill(pid_fg, SIGCONT) == 0){
								del(&jobs, pid_fg);

								// Send job to fg
								tcsetpgrp(0, pid_fg);

								// wait it finish
								waitpid(pid_fg, &status, WUNTRACED);

								if(WIFSTOPPED(status)){
									fprintf(stdout, "PID [%d] stopped\n", pid_fg);
									insert(&jobs, pid_fg, 0);
									if(pid_changed == pid_fg){
										pid_changed = pid_action = -1;
									}
								}

								// Return shell to fg
								signal(SIGTTOU, SIG_IGN);
								tcsetpgrp(0, getpid());
								signal(SIGTTOU, SIG_DFL);
							}else{
								perror("fg");
							}

							sleep(1);

						}else if(t_process == 7){
							// bg
							int pid_bg;
							sscanf(shmem, "%d", &pid_bg);

							if(kill(pid_bg, SIGCONT) == 0){
								update_status(&jobs, pid_bg, 1);
							}else{
								perror("bg");
							}

							sleep(1);
						}

					}else if(WIFSTOPPED(status)){
						fprintf(stdout, "PID [%d] stopped\n", pid);
						insert(&jobs, pid, 0);
					}
				}

				// Go to next command parsed
				ptr = ptr->prox;
			}
		}

		clear_cmd(cmds);
	}

	return 0;
}
