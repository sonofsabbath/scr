#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>

// struktura do przechowywania zadan
RT_TASK zadania[2];
int globalna = 0;

// procedura do modyfikacji przekazanego argumentu
void fun1(void *arg) {
	// pobierz nazwe zadania
	RT_TASK_INFO info;
	rt_task_inquire(rt_task_self(), &info);

	while(1) {
		// zwieksz wartosc i wyswietl
		(*((int*)(arg)))++;
		printf("%s = %i\n", info.name, *((int*)(arg)) );

		// poczekaj do kolejnego wywolania
		rt_task_wait_period(NULL);
	}
}

// procedura do modyfikacji zmiennej globalnej
void fun2(void *arg) {
	// pobierz nazwe zadania
	RT_TASK_INFO info;
	rt_task_inquire(rt_task_self(), &info);

	while(1) {
		// zmniejsz wartosc i wyswietl
		globalna--;
		printf("%s = %i\n", info.name, globalna );

		// poczekaj do kolejnego wywolania
		rt_task_wait_period(NULL);
	}
}

int main(int a, char** b) {
	// zmienna lokalna procedury main
	int lokalna = 0;

	// zablokuj plik wymiany
	mlockall(MCL_CURRENT | MCL_FUTURE);

	// utworz zadania
	char name[10];
	sprintf(name, "lokalna");
	rt_task_create(&zadania[0], name, 0, 10, 0);
	sprintf(name, "globalna");
	rt_task_create(&zadania[1], name, 0, 10, 0);

	// ustal okresowosc
	rt_task_set_periodic(&zadania[0], TM_NOW, 5000000000LL);
	rt_task_set_periodic(&zadania[1], TM_NOW, 5000000000LL);

	// uruchom
	rt_task_start(&zadania[0], &fun1, &lokalna);
	rt_task_start(&zadania[1], &fun2, NULL);

	pause();
}
