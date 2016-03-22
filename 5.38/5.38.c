#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_RESOURCES 5

int available_resources = MAX_RESOURCES;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *partC(void*);
//void *partA(void*);
void increase_count(int);
void decrease_count(int);
//int increase_count_monitor(int);
//int decrease_count_monitor(int);

//struct monitor
//{
//	int available_resources = MAX_RESOURCES;
//	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//	int increase_count_monitor(int);
//	int decrease_count_monitor(int);
//};
//monitor resources {
//	int available_resources;
//	condition resources_avail;
//	int decrease_count(int count)
//	{
//		while (available_resources < count)
//			resources_avail.wait();
//		available_resources -= count;
//	}
//	int increase_count(int count) {
//		available_resources += count;
//		resources_avail.signal();
//	}
//}

int main(int argc, char *argv[]){
	if (argc != 2){
		printf("incorrect number of arguments.");
		return -1;
	}
	int num_threads = atoi(argv[1]);
	char letter;

	int i = 0;
	int resources_requested = 0;
	pthread_t threads[num_threads];
	pthread_attr_t threadAttributes;

	pthread_attr_init(&threadAttributes);

	for(i=0;i<num_threads;i++){
		resources_requested = (rand()%MAX_RESOURCES)+1;
		pthread_create(&threads[i],&threadAttributes,partC,(void *)resources_requested);
	}

	for(i=0;i<num_threads;i++){
		pthread_join(threads[i],NULL);
	}


	pthread_mutex_destroy(&mutex);
	pthread_exit(NULL);
	return 0;


}

/**
 * Uses a mutex to stabilize the race condition
 * from part A.
 */
void* partC(void *resources){

	//while(1)
	//{
		pthread_mutex_lock(&mutex);
//		if((int)available_resources<(int)resources)
//		{ // that number of resources is not available, concede control of resource.
//			pthread_mutex_unlock(&mutex);
//		}
		//else
		//{
			decrease_count((int)resources); // that number of resources are available, so we take them.
			printf("Taking %d resources current count = %d\n",(int)resources,(int)available_resources);
			//pthread_mutex_unlock(&mutex); // concede control
			//break;
	//}
	//}

	sleep(rand()%5);
	pthread_mutex_lock(&mutex); // lock control of available_resources
	increase_count((int)resources); // give the resources back
	printf("Returning %d resources current count = %d\n",(int)resources,(int)available_resources);
	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

void increase_count(int count)
{
	available_resources = available_resources + count;
	pthread_cond_broadcast(&cond);
}

void decrease_count(int count)
{
	if (available_resources < count)
	{
		//return -1;
		pthread_cond_wait(&cond, &mutex);
	}
	else
	{
		available_resources = available_resources - count;
	}
//	while (available_resources < count)
//		wait(mutex);
//	available_resources -= count;
}
