#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ts_hashmap.h"

/**
 * Creates a new thread-safe hashmap.
 *
 * @param capacity initial capacity of the hashmap.
 * @return a pointer to a new thread-safe hashmap.
 */
ts_hashmap_t *initmap(int capacity)
{
  ts_hashmap_t *map = (ts_hashmap_t *)malloc(sizeof(ts_hashmap_t));
  map->size = 0;
  map->capacity = capacity;
  map->table = (ts_entry_t **)malloc(sizeof(ts_entry_t *) * capacity);
  for (int i = 0; i < capacity; i++)
  {
    map->table[i] = (ts_entry_t *)malloc(sizeof(ts_entry_t));
  }
  return map;
}

/**
 * Obtains the value associated with the given key.
 * @param map a pointer to the map
 * @param key a key to search
 * @return the value associated with the given key, or INT_MAX if key not found
 */
int get(ts_hashmap_t *map, int key)
{
  // find hash value
  int hash = abs(key) % map->capacity;
  // lock
  pthread_mutex_lock(&lock);
  // get first entry
  ts_entry_t *entry = map->table[hash];
  // while it's not empty
  while (entry != NULL)
  {
    // if we found it
    if (entry->key == key)
    {
      int value = entry->value;
      // unlock and return value
      pthread_mutex_unlock(&lock);
      return value;
    }
    entry = entry->next;
  }
  // we found nothing, unlock and return int max
  pthread_mutex_unlock(&lock);
  return INT_MAX;
}

/**
 * Associates a value associated with a given key.
 * @param map a pointer to the map
 * @param key a key
 * @param value a value
 * @return old associated value, or INT_MAX if the key was new
 */
int put(ts_hashmap_t *map, int key, int value)
{
  // find hash
  int hash = abs(key) % map->capacity;
  // lock
  pthread_mutex_lock(&lock);
  // get first entry of that hash
  ts_entry_t *entry = map->table[hash];
  // if it's not null, we want to return it, or max if there is nothing there
  while (entry != NULL)
  {
    if (entry->key == key)
    {
      // the key already exists, update it and return old val
      int old_value = entry->value;
      entry->value = value;
      // unlock and return old val
      pthread_mutex_unlock(&lock);
      return old_value;
    }
    entry = entry->next;
  }
  // key not found, just add it
  ts_entry_t *new_entry = malloc(sizeof(ts_entry_t));
  new_entry->key = key;
  new_entry->value = value;
  new_entry->next = map->table[hash];
  map->table[hash] = new_entry;
  map->size++;
  // unlock and return max
  pthread_mutex_unlock(&lock);
  return INT_MAX;
}

/**
 * Removes an entry in the map
 * @param map a pointer to the map
 * @param key a key to search
 * @return the value associated with the given key, or INT_MAX if key not found
 */
int del(ts_hashmap_t *map, int key)
{
  // find hash
  int hash = abs(key) % map->capacity;
  // lock
  pthread_mutex_lock(&lock);
  // find entry
  ts_entry_t *prev_entry = NULL;
  ts_entry_t *entry = map->table[hash];
  // while the entry exists, we remove it, otherwise we return max
  while (entry != NULL)
  {
    if (entry->key == key)
    {
      // store its value
      int value = entry->value;
      // if there are no previous values this is the new prev
      if (prev_entry == NULL)
      {
        map->table[hash] = entry->next;
      }
      else
      {
        prev_entry->next = entry->next;
      }
      free(entry);
      map->size--;
      pthread_mutex_unlock(&lock);
      return value;
    }
    prev_entry = entry;
    entry = entry->next;
  }
  pthread_mutex_unlock(&lock);
  return INT_MAX;
}

/**
 * @return the load factor of the given map
 */
double lf(ts_hashmap_t *map)
{
  return (double)map->size / map->capacity;
}

/**
 * Prints the contents of the map
 */
void printmap(ts_hashmap_t *map)
{
  for (int i = 0; i < map->capacity; i++)
  {
    printf("[%d] -> ", i);
    ts_entry_t *entry = map->table[i];
    while (entry != NULL)
    {
      printf("(%d,%d)", entry->key, entry->value);
      if (entry->next != NULL)
        printf(" -> ");
      entry = entry->next;
    }
    printf("\n");
  }
}