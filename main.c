
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "ts_hashmap.h"

ts_hashmap_t *map;
pthread_mutex_t lock;

typedef struct
{
	int capacity;
	ts_hashmap_t *map;
} Par_args;

void *rand_assign(void *args)
{
	Par_args *input_args = (Par_args *)args;
	int capacity = input_args->capacity;
	ts_hashmap_t *map = input_args->map;
	for (int i = 0; i < 1000; i++)
	{
		int key = rand() % (capacity * 4);
		int value = rand() % capacity;
		int choice = rand() % 3;
		if (choice == 0)
		{
			put(map, key, value);
		}
		else if (choice == 1)
		{
			get(map, key);
		}
		else if (choice == 2)
		{
			del(map, key);
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	// check args
	if (argc < 3)
	{
		printf("Usage: %s <num threads> <hashmap capacity>\n", argv[0]);
		return 1;
	}
	// set up lock
	pthread_mutex_init(&lock, NULL);
	// srand(time(NULL));
	srand(1);
	int num_threads = atoi(argv[1]);
	int capacity = atoi(argv[2]);

	// TODO: Write your testa
	map = initmap(capacity);
	pthread_t threads[num_threads];
	Par_args args[num_threads];
	for (int i = 0; i < num_threads; i++)
	{
		args[i].capacity = capacity;
		args[i].map = map;
		pthread_create(&threads[i], NULL, rand_assign, (void *)&args[i]);
	}
	// wait for them to finish
	for (int i = 0; i < num_threads; i++)
	{
		pthread_join(threads[i], NULL);
	}
	printmap(map);
	for (int i = 0; i < capacity; i++)
	{
		free(map->table[i]);
	}
	free(map->table);
	free(map);

	return 0;
}