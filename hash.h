/* * * * * * *
 * Hashing module for Assignment 2.
 *
 * created for COMP20007 Design of Algorithms 2020
 * template by Tobias Edwards <tobias.edwards@unimelb.edu.au>
 * implementation by Zahra Abbasian Korbekandi <ID: 1032221>
 */

#ifndef HASH_H
#define HASH_H

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define COL 256 
#define ROW 1000
#define EXPAND_FACTOR 2
#define BASE_VAL 64



// A structure to store a node on hash table 
struct hash_table_node 
{ 
    //int dest; 
    char* word;
    struct hash_table_node* next; 
}; 
  
// A structure to store the array of hash table
struct hash_array 
{ 
    struct hash_table_node* head;  
}; 
  
// A structure to store the hash table
struct HashTable 
{ 
    int table_size; 
    struct hash_array* array; 
}; 
  
//Functions
struct hash_table_node* new_hash_table_node(char *word);
struct HashTable* create_hash_table(int table_size);
void add_word(struct HashTable* hash_table, int hash_value, char *word); 
void print_hash_table(struct HashTable* hash_table); 
void free_hash_table(struct HashTable* hash_table);
void remove_list(struct hash_table_node* head_pointer);
struct HashTable* hash_table_resize(struct HashTable* old_hash_table, int size, 
	int step_size);
int chr(int character);
int first_check(struct HashTable* hash_table, int index, int step_size);
int check_next(struct HashTable* hash_table, int index, int step_size);
struct HashTable* hashing(struct HashTable* hash_table, char* word, 
	int m_value, int step_size);
int hash(char* word, int m_value);





// Implements a solution to Problem 1 (a), which reads in from stdin:
//   N M
//   str_1
//   str_2
//   ...
//   str_N
// And outputs (to stdout) the hash values of the N strings 1 per line.
void problem_1_a();

// Implements a solution to Problem 1 (b), which reads in from stdin:
//   N M K
//   str_1
//   str_2
//   ...
//   str_N
// Each string is inputed (in the given order) into a hash table with size
// M. The collision resolution strategy must be linear probing with step
// size K. If an element cannot be inserted then the table size should be
// doubled and all elements should be re-hashed (in index order) before
// the element is re-inserted.
//
// This function must output the state of the hash table after all insertions
// are performed, in the following format
//   0: str_k
//   1:
//   2: str_l
//   3: str_p
//   4:
//   ...
//   (M-2): str_q
//   (M-1):
void problem_1_b();

#endif
