#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include "functions.h"

extern char **environ;
extern node* jobs;

void _export(const char **args, void *shmem){

	char out[200], aux[200];
	memset(out, 0, sizeof(out));

	int len, j = -1, p = 0;

	bool ok = false, qmark = false;;

	for(int arg = 1; args[arg] != NULL; arg++){
		len = strlen(args[arg]);
		for(int i = 0; i < len; i++){
			if(args[arg][i] == '\"'){
				qmark = !qmark;
				if(!ok) exit(0);
			}else if(args[arg][i] == '$' && !qmark){
				j++;
				if(!ok) exit(0);
			}else if(args[arg][i] == ':' && !qmark){
				aux[j] = '\0';

				char *s = getenv(aux);
				if(s != NULL){
					strcat(out, s);
					p = strlen(out);
					out[p++] = ':';
				}else{
					exit(0);
				}
				j = -1;

			}else if(j >= 0 && !qmark){
				aux[j++] = args[arg][i];
			}else{
				out[p++] = args[arg][i];
			}

			if(args[arg][i] == '=') ok = true;
		}
		if(qmark) out[p++] = ' ';
	}


	memcpy(shmem, out, sizeof(out));
	exit(0);
}

void _cd(const char** args, void* shmem){
	int len;
	char path[500];

	if(strncmp(args[1], "..", 2) == 0){
		strcpy(path, getenv("PWD"));
		len = strlen(path);

		while(path[len-1] != '/'){
			path[--len] = '\0';
		}
		if(len > 1)
			path[--len] = '\0';
	}else if(args[1][0] == '/'){
		strcpy(path, args[1]);
	}else{
		strcpy(path, getenv("PWD"));
		if(strcmp(path, "/") != 0)
			strcat(path, "/");
		strcat(path, args[1]);
	}

	memcpy(shmem, path, sizeof(path));
	exit(0);
}

void _jobs(node* jobs){
	node *ptr = jobs;
	while(ptr != NULL){
		fprintf(stdout, "PID = [%d] - ", ptr->pid);
		if(ptr->status){
			fprintf(stdout, "Running\n");
		}else{
			fprintf(stdout, "Stopped\n");
		}

		ptr = ptr->prox;
	}
	exit(0);
}

//Printa historico de comandos - Max 50
void _history(const char *hpath){
	FILE *file = fopen(hpath, "r");

	char* line = NULL;
	size_t len = 0;
	int idx = 1;
	if(file){
		while(getline(&line, &len, file) != -1){
			fprintf(stdout, "%4d  %s", idx++, line);
		}
		fclose(file);
	}else{
		perror("history");
	}
	exit(0);
}

void _kill(const char **args, void* shmem){
	if(args[1] == NULL || args[2] == NULL){
		fprintf(stderr, "Missing arguments.\n");
		exit(1);
	}

	pid_t pid = 0;
	int signal = 0, len;

	//Catching pid number
	len = strlen(args[1]);
	for(int i = 0; i < len; i++){
		if(isdigit(args[1][i])){
			pid = pid * 10 + (args[1][i]-'0');
		}
	}

	//Catching signal number
	len = strlen(args[2]);
	for(int i = 0; i < len; i++){
		if(isdigit(args[2][i])){
			signal = signal * 10 + (args[2][i]-'0');
		}
	}

	if(pid == 0 || signal == 0){
		fprintf(stderr, "Missing values\n");
	}else{
		sprintf(shmem, "%d %d", pid, signal);
	}

	exit(0);
}

void _echo(const char **args){

	const char **s;
	char aux[100], *env;
	
	s = args+1;
	while(*s != NULL){
		if(**s == '$'){
			int i = 0, len = strlen(*s);
			for(; i < len-1; i++){
				aux[i] = (*s)[i+1];
			}
			aux[i] = '\0';

			env = getenv(aux);
			if(env){
				fprintf(stdout, "%s", env);
			}else{
				fprintf(stdout, "%s", *s);
			}
		}else{
			fprintf(stdout, "%s", *s);
		}
		s++;
		if(s != NULL) fprintf(stdout, " ");
	}

	fprintf(stdout, "\n");
	exit(0);
}

void _set(){
	for(char ** env = environ; *env != NULL; env++){
		fprintf(stdout, "%s\n", *env);
	}
	exit(0);
}

void _bg(const char *args[100], void* shmem){
	pid_t pid = 0;
	int len = strlen(args[1]);
	for(int i = 0; i < len; i++){
		if(isdigit(args[1][i])){
			pid = pid*10 + (args[1][i]-'0');
		}
	}

	sprintf(shmem, "%d", pid);
	exit(0);
}

void _fg(const char *args[100], void* shmem){
	pid_t pid = 0;
	int len = strlen(args[1]);
	for(int i = 0; i < len; i++){
		if(isdigit(args[1][i])){
			pid = pid*10 + (args[1][i]-'0');
		}
	}

	sprintf(shmem, "%d", pid);
	exit(0);
}
