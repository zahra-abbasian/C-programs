/* * * * * * *
 * Hashing module for Assignment 2.
 *
 * created for COMP20007 Design of Algorithms 2020
 * template by Tobias Edwards <tobias.edwards@unimelb.edu.au>
 * implementation by Zahra Abbasian
 */

#include "hash.h"

// Implements a solution to Problem 1 (a), which reads in from stdin:
//   N M
//   str_1
//   str_2
//   ...
//   str_N
// And outputs (to stdout) the hash values of the N strings 1 per line.
void problem_1_a() {
	
	char words[ROW][COL+1];
    int i = 0;
    int num_words;
    int m_value;
    
    scanf("%d %d\n", &num_words, &m_value);
    
    while(fgets(words[i], COL, stdin)) {
        words[i][strlen(words[i]) - 1] = '\0';
        i++;
    }
   
    for (i=0; i<num_words; i++) {
    	printf("%d\n", hash(words[i], m_value));
    }
    
}

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
void problem_1_b() {
	
  	int num_words;
    int m_value;
    int step_size;
    char word[ROW][COL+1];
    int i = 0;
	
    scanf("%d %d %d\n", &num_words, &m_value, &step_size);
	
	struct HashTable* hash_table = create_hash_table(m_value);

    //printf("Num m value is: %d\n", m_value);
    while(fgets(word[i], COL, stdin)) 
	{
		word[i][strlen(word[i]) - 1] = '\0';        
        hash_table = hashing(hash_table, word[i], m_value, step_size);
        i++;
    }
    
    print_hash_table(hash_table); 
    
    free_hash_table(hash_table);
    
}


/* ************************************************************************* */
//Functions

//Create node in hash table
struct hash_table_node* new_hash_table_node(char *word) { 
	
	struct hash_table_node* new_node = 
    (struct hash_table_node*) malloc(sizeof(struct hash_table_node)); 
    assert(new_node);
     
    new_node->word = word; 
    new_node->next = NULL; 
    return new_node; 
} 
  

//Create hash table
struct HashTable* create_hash_table(int table_size) { 
	
	struct HashTable* hash_table =  
    (struct HashTable*) malloc(sizeof(struct HashTable)); 
    assert(hash_table);
        
    hash_table->table_size = table_size; 
  
    hash_table->array =  
    (struct hash_array*) malloc(table_size * sizeof(struct hash_array)); 
    assert(hash_table->array);
    
    int i; 
    for (i = 0; i < table_size; i++) {
        hash_table->array[i].head = NULL; 
    }
  
    return hash_table; 
} 
  

//Add words to the hash table
void add_word(struct HashTable* hash_table, int hash_value, char *word) { 
    
	struct hash_table_node* new_node = new_hash_table_node(word); 
    
    hash_table->array[hash_value].head = new_node;
    new_node->next = NULL;   	
}


//Print the hash table
void print_hash_table(struct HashTable* hash_table) { 
	
    int i; 
    for (i = 0; i < hash_table->table_size; i++) { 
        struct hash_table_node* current = hash_table->array[i].head; 
        printf("%d: ", i); 
        while (current) { 
            printf("%s", current->word); 
            current = current->next; 
        } 
        printf("\n"); 
    } 
} 
 

//Resize the hash table
struct HashTable* hash_table_resize(struct HashTable* old_hash_table, int size, 
	int step_size) {

    //Create a new hash table with the new size
    struct HashTable *new_hash_table = create_hash_table(size);
    int i; 
    for (i = 0; i < old_hash_table->table_size; i++) { 
        struct hash_table_node* current = old_hash_table->array[i].head; 

        while (current) { 
        	//Add the previous words on the old table to the new hash table
        	new_hash_table = hashing(new_hash_table, current->word, 
        		new_hash_table->table_size, step_size);
        	current = current->next; 
        }
    }
    free_hash_table(old_hash_table);
    
    return new_hash_table;
}


//Free the hash_table
void free_hash_table(struct HashTable* hash_table) {
	int i;
	int size = hash_table->table_size;
	
	for (i = 0; i < size; i++) {
		remove_list(hash_table->array[i].head); 		
	}
	free(hash_table);
}


//The function to free the array of the hash_table
void remove_list(struct hash_table_node* head_pointer) { 
   struct hash_table_node* current = head_pointer; 
   struct hash_table_node* next; 
  
   while (current != NULL) { 
       next = current->next; 
       free(current); 
       current = next; 
   } 
   head_pointer = NULL; 
} 


//Calculate each character's value
int chr(int character) {
	if (character >=48 && character <=57) {
		return character + 4;
	} else if (character >=65 && character <=90) {
		return character - 39;
	} else if (character >= 97 && character <= 122) {
		return character - 97;
	}
	return -1;
}


//Calculate hash value of words by Horner's rule
int hash(char* word, int m_value) {
	int i;
	int result;
	
	if (strlen(word) == 1) {
		result = chr(word[0]);
	} else {
		result = (chr(word[0]) % m_value)*(BASE_VAL % m_value);
	
		for (i=1; i<strlen(word)-1; i++) {
			result = (result % m_value) + (chr(word[i]) % m_value);
			result = (result % m_value) * (BASE_VAL % m_value);
		}
		result = (result % m_value) + ( chr(word[strlen(word)-1]) % m_value);
	}
	return result % m_value;
}



//Check if an index of the hash table is empty
int first_check(struct HashTable* hash_table, int index, int step_size) {
	if (hash_table->array[index].head == NULL) {
		return index;
	} else if (hash_table->array[index].head != NULL) {
		//Check for the next free index by the given step size
		index = (index+step_size)%hash_table->table_size;
		if (hash_table->array[index].head == NULL) {
		return index;
		} else {
			index = check_next(hash_table, index, step_size);
			return index;
		}
	}
	return -1;
}


//Check the second next and more indices of the hash table
int check_next(struct HashTable* hash_table, int index, int step_size) {
	int i;
	for (i=index; i<hash_table->table_size; i+=step_size) {
		if (hash_table->array[i].head == NULL) {
			return i;
			//Stop when the first empty index is found
			break;
		}
	}
			
	return -1;
}


//Hash the words and find their index on the hash table, if empty insert the
//word, if not empty search for next empty cells by the given step size
//if non found, then resize the hash table
struct HashTable* hashing(struct HashTable* hash_table, char* word, 
	int m_value, int step_size) {

	int index = hash(word, m_value);
    
    if ((index = first_check(hash_table, index, step_size)) != -1) {
    	add_word(hash_table, index, word);
    	
    } else if (first_check(hash_table, index, step_size) == -1) {
    	//No free index is found, so resize the hash table		
    	m_value *= EXPAND_FACTOR;
    	
    	hash_table = hash_table_resize(hash_table, m_value, step_size);
    			
    	hash_table = hashing(hash_table, word, m_value, step_size);							
    }				
    
    return hash_table;
}



/* *********************************************************************** */
/* References:

The code above may be inspired by following websites:
https://www.w3resource.com/c-programming-exercises/file-handling/c-file-
handling-exercise-4.php

https://www.geeksforgeeks.org/graph-and-its-representations/
*/



