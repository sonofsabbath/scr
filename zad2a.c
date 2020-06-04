#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/heap.h>
#include <alchemy/mutex.h>

void toheap(void *arg) {
	RT_HEAP heap;

	rt_heap_create(&heap, "myheap", 100, H_SINGLE | H_PRIO);
	
	RT_MUTEX mutex;
	rt_mutex_create(&mutex, "mymutex");

	char* ptr;
	rt_heap_alloc(&heap, 0, TM_INFINITE, (void**)&ptr);
	
	int arr1;
	int arr2;

	while(1) {
		printf("Czas trwania: ");
		scanf("%d", &arr1);
		printf("Priorytet: ");
		scanf("%d", &arr2);

		rt_mutex_acquire(&mutex, TM_INFINITE);
		ptr[0] = arr1;
		ptr[1] = arr2;
		ptr[2] = 1;
		rt_mutex_release(&mutex);
	}
}

int main(int a, char** b) {
	mlockall(MCL_CURRENT | MCL_FUTURE);

	RT_TASK task;

	rt_task_create(&task, "totask", 0, 10, 0);
	rt_task_start(&task, &toheap, NULL);

	pause();
}
