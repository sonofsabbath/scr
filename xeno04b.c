#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/heap.h>
#include <alchemy/mutex.h>

void fromhealp(void *arg) {
	printf("czekam na sterte\n");
	RT_HEAP heap;
	rt_heap_bind(&heap, "myheap", TM_INFINITE);

	printf("czekam na mutex\n");
	RT_MUTEX mutex;
	rt_mutex_bind(&mutex, "mymutex", TM_INFINITE);

	char *ptr;
	char lastmod = 255;

	rt_heap_alloc(&heap, 0, TM_INFINITE, (void**)&ptr);
	while(1) {
		rt_mutex_acquire(&mutex, TM_INFINITE);
		if(ptr[0] != lastmod) {
			printf("%hi, %hi\n", ptr[1], ptr[3]);
			lastmod = ptr[0];
			}
		rt_mutex_release(&mutex);
		rt_task_sleep(100000000LL);
	}
}

int main(int a, char** b) {
	mlockall(MCL_CURRENT | MCL_FUTURE);

	RT_TASK task;

	rt_task_create(&task, "fromtask", 0, 10, 0);
	rt_task_start(&task, &fromhealp, NULL);

	pause();
}
