
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_RESOURCES 5

int available_resources = MAX_RESOURCES;
pthread_mutex_t leMutex = PTHREAD_MUTEX_INITIALIZER;

void *partC(void*);
void *partA(void*);
int increase_count(int);
int decrease_count(int);

int main(int argc, char *argv[]){
	if (argc != 2){
		printf("incorrect number of arguments.");
		return -1;
	}
	int num_threads = atoi(argv[1]);
	char letter;
	printf("a or c?");
	scanf("%c", &letter);
	if (letter == 'a'){
		printf("A. The data involved in the race condition is the available_resources variable.\n"
						"It is global data shared between the threads.\n"
						"Its access should be controlled with a semaphore.\n");
		printf("B. The race condition occurs whenever processes try to modify the available_resources\n"
						"variable without ensuring that it is locked away from other processes.\n");


		srand (time(NULL));

		int i = 0;
		int resources_requested = 0;
		pthread_t threads[num_threads];
		pthread_attr_t threadAttributes;

		pthread_attr_init(&threadAttributes);

		for(i=0;i<num_threads;i++){
			resources_requested = (rand()%MAX_RESOURCES)+1;
			pthread_create(&threads[i],&threadAttributes,partA,(void *)resources_requested);
		}

		for(i=0;i<num_threads;i++){
			pthread_join(threads[i],NULL);
		}


		pthread_mutex_destroy(&leMutex);
		pthread_exit(NULL);
		return 0;

	}
	if (letter == 'c'){
		srand (time(NULL));

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


		pthread_mutex_destroy(&leMutex);
		pthread_exit(NULL);
		return 0;
	}

}

void* partA(void *resources){

	while(1){
			decrease_count(resources);
			printf("Taking %d resources current count = %d\n",resources,available_resources);
			break;
	  }

	sleep(rand()%5);
	increase_count(resources);
	printf("Returning %d resources current count = %d\n",resources,available_resources);
	pthread_exit(NULL);
}

void* partC(void *resources){

	while(1){
		pthread_mutex_lock(&leMutex);
		if((int)available_resources<(int)resources){
			pthread_mutex_unlock(&leMutex);
		}
		else{
			decrease_count(resources);
			printf("Taking %d resources current count = %d\n",resources,available_resources);
			pthread_mutex_unlock(&leMutex);
			break;
		}
	  }

	sleep(rand()%5);
	pthread_mutex_lock(&leMutex);
	increase_count(resources);
	printf("Returning %d resources current count = %d\n",resources,available_resources);
	pthread_mutex_unlock(&leMutex);

	pthread_exit(NULL);
}

int increase_count(int count){
	available_resources = available_resources + count;
	return 0;
}
int decrease_count(int count){
	if (available_resources < count){
		return -1;
	}
	available_resources = available_resources - count;
	return 0;
}
