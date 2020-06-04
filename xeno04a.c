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

	char lastmod = 0;
	short counter1 = 0;
	short counter2 = 0;

	printf("costam sobie dodaje\n");
	while(1) {
		rt_task_sleep(1000000000LL);
		lastmod++;
		counter1++;
		counter2 += counter1;

		rt_mutex_acquire(&mutex, TM_INFINITE);
		ptr[0] = lastmod;
		ptr[1] = counter1;
		ptr[3] = counter2;
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
