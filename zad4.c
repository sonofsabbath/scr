#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>

RT_TASK zadania[10];
RT_SEM sem[2];
long long int lista[10][2];

void fun1(void *arg) {
	long long int time = (*((long long int*)(arg)));

	RT_TASK_INFO info;
	rt_task_inquire(rt_task_self(), &info);

	printf("%s start na realizatorze 1: %lld\n", info.name, (long long int) rt_timer_ticks2ns(rt_timer_read()));
	rt_timer_spin(time);
	printf("%s koniec na realizatorze 1: %lld\n", info.name, (long long int) rt_timer_ticks2ns(rt_timer_read()));

	rt_sem_v(&sem[0]);
};

void fun2(void *arg) {
	long long int time = (*((long long int*)(arg)));

	RT_TASK_INFO info;
	rt_task_inquire(rt_task_self(), &info);

	printf("%s start na realizatorze 2: %lld\n", info.name, (long long int) rt_timer_ticks2ns(rt_timer_read()));
	rt_timer_spin(time);
	printf("%s koniec na realizatorze 2: %lld\n", info.name, (long long int) rt_timer_ticks2ns(rt_timer_read()));

	rt_sem_v(&sem[1]);
}

int main(int a, char** b) {
	FILE *plik;
	int i, j;
	char nazwa[10][10] = {"zad1", "zad2", "zad3", "zad4", "zad5", "zad6", "zad7", "zad8", "zad9", "zad10"};

	plik = fopen("zad4.txt", "r");
	for(i = 0; i < 10; i++) {
		for(j = 0; j < 2; j++) {
			fscanf(plik, "%lld", &lista[i][j]);
		}
	}
	fclose(plik);

	mlockall(MCL_CURRENT | MCL_FUTURE);

	long long int t1 = 0;
	long long int t2 = 0;
	int realtask[10];
	long long int ready[10];
	long long int task_time1;
	long long int task_time2;

	for(i = 0; i < 10; i++) {
		task_time1 = t1	+ lista[i][0];
		task_time2 = t2 + lista[i][1];

		if(task_time1 <= task_time2) {
			ready[i] = t1;
			realtask[i] = 1;
			t1 = task_time1;
		} else {
			ready[i] = t2;
			realtask[i] = 2;
			t2 = task_time2;
		}
	}

	rt_sem_create(&sem[0], "sem1", 0, 0);
	rt_sem_create(&sem[1], "sem2", 0, 0);

	for(i = 0; i < 10; i++) {
		printf("zad%d:\n", i+1);
		printf("Realizator %d, czas rozpoczecia %lld\n\n", realtask[i], ready[i]);

		/* rt_task_create(&zadania[i], *(nazwa + i), 0, 10, 0);
		if(realtask[i] == 1) {
			rt_task_start(&zadania[i], &fun1, &lista[i][0]);
			rt_sem_p(&sem[0], TM_INFINITE);
		} else {
			rt_task_start(&zadania[i], &fun2, &lista[i][1]);
			rt_sem_p(&sem[1], TM_INFINITE);
		} */
	}

	pause();
}
