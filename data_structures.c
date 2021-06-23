#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "functions.h"

// Linked list to deal with jobs
void insert(node** list, int pid, int status){
	node* new_node = (node*) malloc(sizeof(node));
	new_node->pid = pid;
	new_node->status = status;
	new_node->prox = NULL; 

	if(*list == NULL){
		*list = new_node;
	}else{
		node* ptr = *list;
		while(ptr->prox != NULL){
			ptr = ptr->prox;
		}
		ptr->prox = new_node;
	}
}

void update_status(node** list, int pid, int status){
	node* ptr = *list;

	while(ptr != NULL){
		if(ptr->pid == pid){
			ptr->status = status;
			return;
		} 
		ptr = ptr->prox;
	}
}

void del(node** list, int pid){
	if(*list == NULL) return;

	node* ptr = *list;

	if(ptr->pid == pid){
		(*list) = (*list)->prox;
		free(ptr);
	}else{
		while(ptr->prox != NULL && (ptr->prox)->pid != pid){
			ptr = ptr->prox;
		}

		if(ptr->prox != NULL){
			node *aux = ptr->prox;
			ptr->prox = aux->prox;
			free(aux);
		}
	}
}

void kill_jobs_and_free_memory(node* lista){
	if(lista == NULL)
		return;
	kill_jobs_and_free_memory(lista->prox);
	kill(lista->pid, 9);
	free(lista);
}

// Linked list of commands
void add_cmd(cnode** list, char * val){
	cnode* new_node = (cnode*) malloc(sizeof(cnode));
	strcpy(new_node->cmd, val);
	new_node->prox = NULL;
	
	if(*list == NULL){
		*list = new_node;
	}else{
		cnode* ptr = *list;
		while(ptr->prox != NULL){
			ptr = ptr->prox;
		}
		ptr->prox = new_node;
	}
}

void clear_cmd(cnode* list){
	if(list == NULL)
		return;
	clear_cmd(list->prox);
	free(list);
}
