#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/mutex.h>

void serve(void *arg) {
	RT_MUTEX mx;

	printf("%s szuka mymux.\n", (char*)(arg));
	rt_mutex_bind(&mx, "mymux", TM_INFINITE);

	while(1) {
		printf("%s czeka na mutex.\n", (char*)(arg));
		rt_mutex_acquire(&mx, TM_INFINITE);
		printf("%s pobral mutex.\n", (char*)(arg));

		rt_task_sleep(5000000000LL);
		rt_mutex_release(&mx);
		printf("%s zwolnil mutex.\n", (char*)(arg));
	}
}

int main(int a, char** b) {
	mlockall(MCL_CURRENT | MCL_FUTURE);

	RT_MUTEX mx;
	RT_TASK task;

	char nazwa[10];
	sprintf(nazwa, "Xeno B");

	rt_task_create(&task, nazwa, 0, 20, 0);
	rt_task_start(&task, &serve, nazwa);

	pause();
}
