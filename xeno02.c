#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/sem.h>

RT_TASK zadania[3];

// semafor
RT_SEM sem;

int licznik;

void serve(void *arg) {
	RT_TASK_INFO info;
	rt_task_inquire(rt_task_self(), &info);

	printf("%s utworzone. \n", info.name);

	while(1) {
		// czekaj na zwolnienie semafora
		printf("%s czeka na semafor.\n", info.name);
		rt_sem_p(&sem, TM_INFINITE);
		printf("%s otrzymalo semafor.\n", info.name);

		// zwieksz licznik i wyswietl zmieniona wartosc
		printf("%s ustawia licznik na %i.\n", info.name, ++licznik);

		// czekaj do ponownego wywolania zadania, ale bez zwalniania semafora
		rt_task_wait_period(NULL);

		// zwolnij semafor
		rt_sem_v(&sem);
		printf("%s zwolnilo semafor.\n", info.name);
	}
}

int main(int a, char** b) {
	mlockall(MCL_CURRENT | MCL_FUTURE);

	// inicjalizacja semafora
	rt_sem_create(&sem, "somename", 2, S_PRIO);

	// zadania
	char name[3][10];
	sprintf(name[0], "zad 1");
	sprintf(name[1], "zad 2");
	sprintf(name[2], "zad 3");

	rt_task_create(&zadania[0], name[0], 0, 10, 0);
	rt_task_set_periodic(&zadania[0], TM_NOW, 5000000000LL);
	rt_task_start(&zadania[0], &serve, 0);

	rt_task_create(&zadania[1], name[1], 0, 20, 0);
	rt_task_set_periodic(&zadania[1], TM_NOW, 5000000000LL);
	rt_task_start(&zadania[1], &serve, 0);

	rt_task_create(&zadania[2], name[2], 0, 30, 0);
	rt_task_set_periodic(&zadania[2], TM_NOW, 5000000000LL);
	rt_task_start(&zadania[2], &serve, 0);

	pause();
}
