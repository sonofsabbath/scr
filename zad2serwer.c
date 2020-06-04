#include <stdio.h>
#include <stdlib.h>
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

void fun(void *arg) {
	rt_task_set_mode(0, T_LOCK, NULL);

	RT_TASK_INFO info;
	rt_task_inquire(rt_task_self(), &info);

	printf("%s start: %d\n", info.name, (long) rt_timer_read());
	rt_timer_spin((*((int*)(arg))));
	printf("%s koniec: %d\n", info.name, (long) rt_timer_read());
}

void fromheap(void *arg) {
	printf("czekam na sterte\n");
	RT_HEAP heap;
	rt_heap_create(&heap, "myheap", TASK_MAX * sizeof(task_data), H_PRIO | H_SINGLE);

	printf("czekam na mutex\n");
	RT_MUTEX mutex;
	rt_mutex_create(&mutex, "mymutex");

	RT_SEM sem;
	rt_sem_create(&sem, "mysem", 0, 0);

	task_data* ptr;
	rt_heap_alloc(&heap, 0, TM_INFINITE, (void**)&ptr);

	RT_TASK zadania[TASK_MAX];
	int zadanie = 0;
	int ind = 0;

	while(1) {
		rt_sem_p(&sem, TM_INFINITE);
		rt_mutex_acquire(&mutex, TM_INFINITE);

		ind = (ind + 1) % TASK_MAX;
		int *czas = malloc(sizeof(int));
		*czas = ptr[ind].czas;

		char nazwa[12];
		sprintf(nazwa, "zad%d", zadanie);
		rt_task_create(&zadania[zadanie], nazwa, 0, ptr[ind].priorytet, 0);
		rt_task_start(&zadania[zadanie], &fun, (void*) czas);		

		rt_mutex_release(&mutex);
		zadanie = (zadanie + 1) % TASK_MAX;
	}
}

int main(int a, char** b) {
	mlockall(MCL_CURRENT | MCL_FUTURE);

	RT_TASK task;

	rt_task_create(&task, "fromtask", 0, 10, 0);
	rt_task_start(&task, &fromheap, NULL);

	pause();
}
