typedef struct _node{
	int pid;
	int status;
	struct _node *prox; 
}node;

void insert(node**, int, int);
void update_status(node**, int, int);
void del(node**, int);
void kill_jobs_and_free_memory(node*);


typedef struct _cnode{
	char cmd[100];
	struct _cnode *prox; 
}cnode;
void add_cmd(cnode**, char*);
void clear_cmd(cnode*);
