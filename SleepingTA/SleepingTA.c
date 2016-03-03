#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <semaphore.h>

#define NUM_ITERATIONS 10

pthread_mutex_t help_mutex;
pthread_mutex_t count_mutex;
sem_t TA_sem;
sem_t student_sem;
int number_waiting;
int go_home;

typedef struct {
	long nsecSleep;
	int numberOfIterations;
	int studentNumber;
} sleepAndCount;

void* student(void* param);
void* tA(void* param);

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Usage: ./organize <number of students>\n");
		return 1;
	}
	int numberOfStudents = atoi(argv[1]);
	if (numberOfStudents <= 0) {
		printf("Error: Number of students must be a positive integer.\n");
		return 2;
	}

	sem_init(&TA_sem, 0, 0);
	sem_init(&student_sem, 0, 0);
	number_waiting = 0;
	go_home = 0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_mutex_init(&help_mutex, NULL);
	pthread_mutex_init(&count_mutex, NULL);

	pthread_t tid_TA;
	pthread_create(&tid_TA, &attr, tA, NULL);

	pthread_t* tid_students[numberOfStudents];
	sleepAndCount* params[numberOfStudents];
	int i;
	for (i = 0; i < numberOfStudents; ++i) {
		srand(time(NULL) + i);
		tid_students[i] = malloc(sizeof(pthread_t));
		params[i] = malloc(sizeof(sleepAndCount));
		params[i]->nsecSleep = (long) rand() % 1000000000;
		params[i]->numberOfIterations = NUM_ITERATIONS;
		params[i]->studentNumber = i+1;
		pthread_create(tid_students[i], &attr, student, params[i]);
	}

	for (i = 0; i < numberOfStudents; ++i) {
		pthread_join(*tid_students[i], NULL);
		free(tid_students[i]);
		free(params[i]);
	}
	go_home = 1;
	sem_post(&TA_sem);
	pthread_join(tid_TA, NULL);
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&help_mutex);
	pthread_mutex_destroy(&count_mutex);
	sem_destroy(&TA_sem);
	sem_destroy(&student_sem);

	return 0;
}

void* tA(void* param) {
	srand(time(NULL) / 2);
	struct timespec time;
	time.tv_sec = 1;
	time.tv_nsec = (long) rand() % 1000000000;
	while (1) {
		printf("TA sleeping, number_waiting = %d\n",number_waiting);
		sem_wait(&TA_sem);
		if (go_home) break;
		printf("TA awake, number_waiting = %d\n",number_waiting);
		pthread_mutex_lock(&help_mutex);
		sem_post(&student_sem);
		pthread_mutex_lock(&count_mutex);
		number_waiting--;
//		if (number_waiting < 0){
//			number_waiting = 0;
//		}
		pthread_mutex_unlock(&count_mutex);
		printf("TA helping student, number_waiting = %d\n",number_waiting);
		nanosleep(&time, NULL);
		pthread_mutex_unlock(&help_mutex);
	}
	pthread_exit(0);
}

void* student(void* param) {
	struct timespec time;
	time.tv_sec = 0;
	time.tv_nsec = ((sleepAndCount*)param)->nsecSleep;
	int count = ((sleepAndCount*)param)->numberOfIterations;
	int stuNum = ((sleepAndCount*)param)->studentNumber;
	int i;
	for (i = 0; i < count; ++i) {
		printf("student %d programming, number_waiting = %d\n",stuNum,number_waiting);
		nanosleep(&time, NULL);
		pthread_mutex_lock(&count_mutex);
		if (number_waiting >= 3)
		{
			printf("student %d waiting for chair, number_waiting = %d\n",stuNum, number_waiting);
			pthread_mutex_unlock(&count_mutex);
		}
		else
		{
			number_waiting++;
			pthread_mutex_unlock(&count_mutex);
			sem_post(&TA_sem);
			printf("student %d waiting in chair %d, number_waiting = %d\n",stuNum, number_waiting, number_waiting);
			sem_wait(&TA_sem);
			pthread_mutex_lock(&help_mutex);
			pthread_mutex_unlock(&help_mutex);
		}
	}
	pthread_exit(0);
}
