pthread_mutex_t mutex;

typedef struct {
	int arg;//argument for function
	int pr; //priority
	int (* act) (int);//function
	int res;//result
	int num;//number of the task
} Data;
struct Node {
	Data val;
	struct Node * next;
};
struct List {
	struct Node * head;
};

struct List * job_list_create ();

int thread_pool_add_job (struct List * list, Data x);

int findMax (struct List * list);

Data extractMax (struct List * list);

void list_print (struct List * list);

void list_clear(struct List * list);

void job_list_destroy (struct List * list);

void * mythread (void * list);
pthread_t * thread_pool_start(int n, struct List *list);

void thread_pool_destroy (pthread_t * t, int n);

