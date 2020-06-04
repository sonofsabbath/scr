#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>

// struktura do przechowywania zadan
RT_TASK zadania[10];
long long int lista[10][4];

void fun2(void *arg) {
	rt_task_set_mode(0, T_LOCK, NULL);
	int k;
	int rep = (*((int*)(arg)));

	// pobierz nazwe zadania
	RT_TASK_INFO info;
	rt_task_inquire(rt_task_self(), &info);

	for(k = 0; k < lista[rep][3]; k++) {
		printf("%s start %d/%d: %lld\n", info.name, k+1, lista[rep][3], (long long int) rt_timer_ticks2ns(rt_timer_read()));
		rt_timer_spin(lista[rep][1]);
		printf("%s koniec %d/%d: %lld\n", info.name, k+1, lista[rep][3], (long long) rt_timer_ticks2ns(rt_timer_read()));

		// poczekaj do kolejnego wywolania
		rt_task_wait_period(NULL);
	}
}

int main(int a, char** b) {
	FILE *plik;
	int i, j;
	char nazwa[10][10] = {"zad1", "zad2", "zad3", "zad4", "zad5", "zad6", "zad7", "zad8", "zad9", "zad10"};

	plik = fopen("zad1.txt", "r");
	for(i = 0; i < 10; i++) {
		for(j = 0; j < 4; j++) {
			fscanf(plik, "%lld", &lista[i][j]);
		}
	}
	fclose(plik);

	// zablokuj plik wymiany
	mlockall(MCL_CURRENT | MCL_FUTURE);
	
	long long start = rt_timer_read();

	// utworz zadania
	for(i = 0; i < 10; i++) {
		rt_task_create(&zadania[i], *(nazwa + i), 0, lista[i][2], 0);

		long long ready = (long long) rt_timer_ns2ticks(lista[i][0]);
		rt_task_set_periodic(&zadania[i], start + ready, lista[i][1]);
		rt_task_start(&zadania[i], &fun2, &i);
	}

	pause();
}
