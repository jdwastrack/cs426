#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_SEAT 3

#define SLEEP_MAX 5

sem_t sem_stu;
sem_t sem_ta;

pthread_mutex_t mutex;

int chair[3];
int count = 0; // number of waiting students
int next_seat = 0;
int next_teach = 0;

void rand_sleep(void);
void* stu_programming(void* stu_id);
void* ta_teaching();

int main(int argc, char **argv){
	pthread_t *students;
	pthread_t ta;
	int* student_ids;
	int student_num;

	int i;

	if (argc != 2){
		printf("Incorrect number of arguments");
		return -1;
	}
	if (!isdigit(argv[1])){
		printf("Argument must be an integer");
		return -1;
	}
	student_num = atoi(argv[1]);
	students = (pthread_t*)malloc(sizeof(pthread_t) * student_num);
	student_ids = (int*)malloc(sizeof(int) * student_num);
	memset(student_ids, 0, student_num);
	sem_init(&sem_stu,0,0);
	sem_init(&sem_ta,0,1);
	srand(time(NULL));
	pthread_mutex_init(&mutex,NULL);
	pthread_create(&ta,NULL,ta_teaching,NULL);
	for(i=0; i<student_num; i++)
	{
		student_ids[i] = i+1;
		pthread_create(&students[i], NULL, stu_programming, (void*) &student_ids[i]);
	}

	pthread_join(ta, NULL);
	for(i=0; i<student_num;i++)
	{
		pthread_join(students[i],NULL);
	}
	return 0;
}

void* stu_programming(void* stu_id)
{
	int id = *(int*)stu_id;
	printf("Student %d is programming\n",id);

	while(1)
	{
		rand_sleep();
		pthread_mutex_lock(&mutex);
		if(count < NUM_SEAT) // student can sit in a chair
		{
			chair[next_seat] = id;
			count++;
			printf("Student %d is waiting in line.\n",id);
			printf("Student numbers in chairs: %d %d %d\n",chair[0],chair[1],chair[2]);
			next_seat = (next_seat+1) % NUM_SEAT;
			pthread_mutex_unlock(&mutex);
			sem_post(&sem_stu);
			sem_wait(&sem_ta);
		}
		else // chairs are full
		{
			pthread_mutex_unlock(&mutex);
			printf("Chairs are all full, so student %d will program for a while.\n",id);
		}
	}
}

void* ta_teaching()
{
	while(1)
	{
		sem_wait(&sem_stu);
		pthread_mutex_lock(&mutex);
		printf("TA is teaching student %d\n",chair[next_teach]);
		chair[next_teach]=0;
		count--;
		printf("student numbers in chairs: %d %d %d\n",chair[0],chair[1],chair[2]);
		next_teach = (next_teach + 1) % NUM_SEAT;
		rand_sleep();
		printf("TA is finished with student.\n");
		pthread_mutex_unlock(&mutex);
		sem_post(&sem_ta);
	}
}

void rand_sleep(void){
	int time = rand() % SLEEP_MAX + 1;
	sleep(time);
}
