#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "functions.h"

int get_type_process(const char* cmd){
	if(strncmp(cmd, "export", 6) == 0){
		return 1;
	}else if(strncmp(cmd, "cd", 2) == 0){
		return 2;
	}else if(strncmp(cmd, "history", 7) == 0){
		return 3;
	}else if(strncmp(cmd, "kill", 4) == 0){
		return 4;
	}else if(strncmp(cmd, "jobs", 4) == 0){
		return 5;
	}else if(strncmp(cmd, "fg", 2) == 0){
		return 6;
	}else if(strncmp(cmd, "bg", 2) == 0){
		return 7;
	}else if(strncmp(cmd, "echo", 4) == 0){
		return 8;
	}else if(strncmp(cmd, "set", 3) == 0){
		return 9;
	}
	return 0;
}

//Add commands to .history
void _add_history(const char cmd[100], const char hpath[500]){
	char hist[52][100];
	int cnt_lines = 0;
	char* line = NULL;
	size_t len = 0, sz_line;
	
	FILE* file = fopen(hpath, "r");
	if(file){
		while(getline(&line, &len, file) != -1){
			strcpy(hist[cnt_lines], line);
			sz_line = strlen(line);
			hist[cnt_lines][sz_line-1] = '\0';
			cnt_lines++;
		}
		free(line);
		fclose(file);

		if(cnt_lines == 50){
			for(int i = 0; i < cnt_lines-1; i++)
				strcpy(hist[i], hist[i+1]);
			strcpy(hist[49], cmd);
		}else{
			strcpy(hist[cnt_lines++], cmd);
		}

		file = fopen(hpath, "w");
		for(int i = 0; i < cnt_lines; i++){
			fprintf(file, "%s\n", hist[i]);
		}
		fclose(file);
	}else{
		perror("history");
	}
}

void whereami(char path[100]){
	strcpy(path, getenv("PWD"));
	
	int i, j = 0, cnt = 0, len = strlen(path);
	
	for(i = 0; i < len && cnt < 3; i++){
		cnt += (path[i]=='/');
	}

	if(cnt < 3)	return;

	i--;

	path[j++] = '~';
	for(; i < len; i++, j++){
		path[j] = path[i];
	}
	path[j]= '\0';
}

int parse_args(char cmd[200], char *args[100], int t_process){
	char parser[50][100];
	memset(parser, 0, sizeof(parser));

	char *pch, *path;
	char *sptr1, *sptr2;

	char aux[200], path_aux[200], cmd_aux[200], pg[100];
	FILE *file;

	//Parse cmd
	strcpy(cmd_aux, cmd);
	pch = strtok_r(cmd_aux, " ", &sptr2);

	int argc = 0, cod = 0;
	while(pch != NULL){
		if(strncmp(pch, ">", 1) == 0){
			cod = 1;		
		}else if(strncmp(pch, "<", 1) == 0){
			cod = 2;
		}else if(strncmp(pch, "2>", 2) == 0){
			cod = 3;
		}else{
			strcpy(parser[argc+cod], pch);
			if(cod == 0){
				argc++;
			}				
		}
		pch = strtok_r(NULL," ", &sptr2);
	}

	if(parser[argc+1][0] != '\0'){
		file = fopen(parser[argc+1], "w");
		if(file != NULL){
			dup2(fileno(file), STDOUT_FILENO);
			fclose(file);
		}else{
			fprintf(stderr, "Error opening file\n");
			exit(1);
		}
	}

	if(parser[argc+2][0] != '\0'){
		file = fopen(parser[argc+2], "r");
		if(file != NULL){
			dup2(fileno(file), STDIN_FILENO);
			fclose(file);
		}else{
			fprintf(stderr, "Error opening file\n");
			exit(1);
		}
	}

	if(parser[argc+3][0] != '\0'){
		file = fopen(parser[argc+3], "w");
		if(file != NULL){
			dup2(fileno(file), STDERR_FILENO);
			fclose(file);
		}else{
			fprintf(stderr, "Error opening file\n");
			exit(1);
		}
	}

	for(int i = 0; i < argc; i++){
		args[i] = parser[i];
	}
	args[argc] = (char *) NULL;

	strcpy(pg, args[0]);

	if(t_process){
		return 0;
	}else if(cmd[0] == '.' || cmd[0] == '/'){

		if(access(args[0], F_OK) == 0){
			return 0;
		}

	}else{
		strcpy(path_aux, getenv("MYPATH"));
		path = strtok_r(path_aux, ":", &sptr1);

		while(path != NULL){

			strcpy(aux, path);
			strcat(aux, "/");
			strcat(aux, pg);

			strcpy(args[0], aux);

			if(access(args[0], F_OK) == 0){
				return 0;
			}

			path = strtok_r(NULL, ":", &sptr1);
		}
	}

	strcpy(args[0], pg);
	return -1;
}
