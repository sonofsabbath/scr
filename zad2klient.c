#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/heap.h>
#include <alchemy/mutex.h>
#include <alchemy/sem.h>

const int TASK_MAX = 50;

typedef union{
	struct {
		int priorytet, czas;
	};
	int ind;
}task_data;

int main() {
	printf("czekam na sterte\n");
	RT_HEAP heap;
	rt_heap_bind(&heap, "myheap", TM_INFINITE);
	
	printf("czekam na mutex\n");
	RT_MUTEX mutex;
	rt_mutex_bind(&mutex, "mymutex", TM_INFINITE);

	printf("czekam na semafor\n");
	RT_SEM sem;
	rt_sem_bind(&sem, "mysem", TM_INFINITE);

	task_data* ptr;
	rt_heap_alloc(&heap, 0, TM_INFINITE, (void**)&ptr);

	while(1) {
		task_data input;
		printf("Czas trwania: ");
		scanf("%d", &input.czas);
		printf("Priorytet: ");
		scanf("%d", &input.priorytet);

		rt_mutex_acquire(&mutex, TM_INFINITE);

		int ind = ++ptr[0].ind;
		if(ind == TASK_MAX) {
			ind = 1;
			ptr[0].ind = ind;
		}
		ptr[ind] = input;

		rt_mutex_release(&mutex);
		rt_sem_v(&sem);
	}
}
