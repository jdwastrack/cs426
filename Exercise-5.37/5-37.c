#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_RESOURCES 5

int available_resources = MAX_RESOURCES;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
						"variable without ensuring that it is locked away from other processes."
						"\n ie. in the increase_count and decrease_count functions. \n");



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


		pthread_mutex_destroy(&mutex);
		pthread_exit(NULL);
		return 0;

	}
	if (letter == 'c'){

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

}

/**
 * The normal code with the race
 * condition still present.
 */
void* partA(void *resources){

	decrease_count((int)resources);
	printf("Taking %d resources current count = %d\n",(int)resources,(int)available_resources);
	sleep(rand()%5);
	increase_count((int)resources);
	printf("Returning %d resources current count = %d\n",(int)resources,(int)available_resources);
	pthread_exit(NULL);
}
/**
 * Uses a mutex to stabilize the race condition
 * from part A.
 */
void* partC(void *resources){

	while(1){
		pthread_mutex_lock(&mutex);
		if((int)available_resources<(int)resources){ // that number of resources is not available, concede control of resource.
			pthread_mutex_unlock(&mutex);
		}
		else{
			decrease_count((int)resources); // that number of resources are available, so we take them.
			printf("Taking %d resources current count = %d\n",(int)resources,(int)available_resources);
			pthread_mutex_unlock(&mutex); // concede control
			break;
		}
	  }

	sleep(rand()%5);
	pthread_mutex_lock(&mutex); // lock control of available_resources
	increase_count((int)resources); // give the resources back
	printf("Returning %d resources current count = %d\n",(int)resources,(int)available_resources);
	pthread_mutex_unlock(&mutex);

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
