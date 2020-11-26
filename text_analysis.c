/* * * * * * *
 * Text Analysis module for Assignment 2.
 *
 * created for COMP20007 Design of Algorithms 2020
 * template by Tobias Edwards <tobias.edwards@unimelb.edu.au>
 * implementation by Zahra Abbasian
 */

#include "text_analysis.h"

// Build a character level trie for a given set of words.
//
// The input to your program is an integer N followed by N lines containing
// words of length < 100 characters, containing only lowercase letters.
//
// Your program should built a character level trie where each node indicates
// a single character. Branches should be ordered in alphabetic order.
//
// Your program must output the pre-order traversal of the characters in
// the trie, on a single line.
void problem_2_a() {
	
	char word[ROW][COL+1];
    int i = 0;
    int num_words;
	
    scanf("%d\n", &num_words);

    struct Trie* root = new_trie_node();
    
    while(fgets(word[i], COL, stdin)) {
        word[i][strlen(word[i]) - 1] = '$';
        word[i][strlen(word[i])] = '\0';
        insert(root, word[i]);
        i++;
    }
    
    printf("^\n");
    preorder_traverse(root);
 
    free_trie(root);
}

// Using the trie constructed in Part (a) this program should output all
// prefixes of length K, in alphabetic order along with their frequencies
// with their frequencies. The input will be:
//   n k
//   str_0
//   ...
//   str_(n-1)
// The output format should be as follows:
//   an 3
//   az 1
//   ba 12
//   ...
//   ye 1
void problem_2_b() {
	
	char word[ROW][COL+1];
    int i = 0;
    int num_words;
    int depth;

    scanf("%d %d\n", &num_words, &depth);

    struct Trie* root = new_trie_node();

    while(fgets(word[i], COL, stdin)) {
        word[i][strlen(word[i]) - 1] = '$';
        word[i][strlen(word[i])] = '\0';
        insert(root, word[i]);
        i++;
    }
    
    int level = 0; 
    char str[COL+1]; 
  
    //Print contents of Trie until a given depth 
    display(root, str, level, depth); 
    
    free_trie(root); 
}

// Again using the trie data structure you implemented for Part (a) you will
// provide a list (up to 5) of the most probable word completions for a given
// word stub.
//
// For example if the word stub is "al" your program may output:
//   0.50 algorithm
//   0.25 algebra
//   0.13 alright
//   0.06 albert
//   0.03 albania
//
// The probabilities should be formatted to exactly 2 decimal places and
// should be computed according to the following formula, for a word W with the
// prefix S:
//   Pr(word = W | stub = S) = Freq(word = W) / Freq(stub = S)
//
// The input to your program will be the following:
//   n
//   stub
//   str_0
//   ...
//   str_(n-1)
// That is, there are n + 1 strings in total, with the first being the word
// stub.
//
// If there are two strings with the same probability ties should be broken
// alphabetically (with "a" coming before "aa").
void problem_2_c() {
	
	char word[ROW][COL+1];
	char prefix[COL];
    int i = 0;
    int num_words;
    int depth;

    scanf("%d %d\n", &num_words, &depth);

    struct Trie* root = new_trie_node();
    
    //Read the given prefix from second line
    fgets(prefix, COL, stdin);
    prefix[strlen(prefix) - 1] = '\0';
    
    while(fgets(word[i], COL, stdin)) {
        word[i][strlen(word[i]) - 1] = '$';
        word[i][strlen(word[i])] = '\0';
        insert(root, word[i]);
        i++;
    }
   
    int level = 0; 
    char str[COL+1]; 
    char com_words[ROW][COL+1];
    int result_freq[ROW];
    int index_array[ROW];
    float prefix_count;
    float stub;
    
    //Number of found words with the given prefix
    int n = display_prefix(root, str, level, prefix, com_words, result_freq);
    
    //Create an index array representing the found words array, then sort this
    //index array exactly as the frequency array and use the indices to 
    //refer to the array of found words
    for (i = 0; i < n; i++) {
    	index_array[i] = i;
    }
   
    //Count the frequency of the given prefix
    prefix_count = prefix_freq(root, prefix); 
    
    //Sort the result array and index_array together
    selection_sort(result_freq, n, index_array);
    
    //For the purpose of printing the results up to a given limit
    if (n > MAX_PRINT) {
    	n = MAX_PRINT;
    }
    
    //Print the result
    for (i = 0; i < n; i++) {
    	stub = result_freq[i]/prefix_count;
    	printf("%.2f", stub);  
    	printf(" %s\n", com_words[index_array[i]]);
    }
    
    free_trie(root);
}




/* ************************************************************************* */
//Functions:


// Function that creates a new Trie node
struct Trie* new_trie_node() {
	int i;
	
	struct Trie* node = (struct Trie*)malloc(sizeof(struct Trie));
	assert(node);
	
	node->freq_count = 0;
	node->is_leaf = 0;

	for (i = 0; i < CHAR_SIZE; i++) {
		node->character[i] = NULL;
	}

	return node;
}


// Insert a string to the Trie
void insert(struct Trie *root, char* str) {
	
	struct Trie* current = root;
	
	while (*str) {
		// create a new node if the given character doesn't exist
		if (current->character[*str - '$'] == NULL)
			current->character[*str - '$'] = new_trie_node();

		// go to next node in Trie
		current->freq_count++;
		current = current->character[*str - '$'];
        
		// go to next character of the string
		str++;
	}

	// mark current node as leaf
	current->is_leaf = TRUE;
    current->freq_count++;
}



//Preorder traversal of Trie
void preorder_traverse(struct Trie* root) {

	struct Trie* current = root;
	
	if (current->is_leaf != TRUE) {
		for (int i = 0; i < CHAR_SIZE; i++) {
			if (current->character[i]) {
				printf("%c\n", i+'$');
				preorder_traverse(current->character[i]);
			}
		}
	}
}


//Free the trie
void free_trie(struct Trie* root) {
	struct Trie* current = root;
	
	for (int i = 0; i < CHAR_SIZE; i++) {
		if (current->character[i]!=NULL) {	
			free_trie(current->character[i]);
		}
	}
	
	free(root);
	root = NULL;
}



// Display the contents of Trie until some depth 
void display(struct Trie* root, char str[], int level, int depth) { 
    
	int i; 

	//When end of a word is reached
    if (root->is_leaf == TRUE) { 
        str[level-1] = '\0'; 
    } 
    
    //When it is the required depth
    if (level == depth) {
    	str[level] = '\0'; 
    	
    	//Only print the string with the lengths equal to depth
    	if (strlen(str) == depth) {
    		printf("%s %d\n", str, root->freq_count); 
        }
    }
    	
    for (i = 0; i < CHAR_SIZE; i++) { 
        if (root->character[i]) { 
            str[level] = i + '$'; 
            display(root->character[i], str, level + 1, depth); 
        } 
    } 
} 



// Function to store the words on a Trie using a given prefix 
int display_prefix(struct Trie* root, char str[], int level, char* prefix, 
	char com_words[ROW][COL+1], int result_freq[]) {

	static int len=0;
	struct Trie* current = root;
	int i;
	
    if (root->is_leaf == TRUE) { 
        str[level-1] = '\0'; 
        
        if(check_prefix(str, prefix)) {
           strcpy(com_words[len], str );
           result_freq[len] = root->freq_count;
           len++;
        }
    }
 
    for (i = 0; i < CHAR_SIZE; i++) { 
        if (current->character[i]) { 
            str[level] = i + '$'; 
            len = display_prefix(root->character[i], str, level + 1, prefix, 
            	com_words, result_freq); 
        } 
    } 
    return len;
} 


//Check if the found words have the same prefix
bool check_prefix(char* str, char* prefix) {
	int level;
	int length = strlen(prefix);
	
	for (level = 0; level<length; level++) {
		if (str[level] != prefix[level]) {
			return false;
		}
	}
	return true;
}



//Find the frequency of the given prefix
float prefix_freq(struct Trie* root, char* prefix) {
	
	struct Trie* current = root;
	int index = 0;
	int depth = strlen(prefix);
	float prefix_freq_count;
	int level;
	
    for (level = 0; level < depth; level++) { 
    	index = prefix[level] - '$'; 

        if (!current->character[index]) {
            return -1; 
        }
        //Stop going to the next level if Trie
        if (level == depth - 1) {
        	break;
        }
        current = current->character[index]; 
    } 
       
    prefix_freq_count = current->character[index]->freq_count;
               
    return prefix_freq_count;
} 



//A function to sort the result array by Selection Sort
void selection_sort(int array[], int n, int index_array[]) { 
	int i;
	
    for (i = 0; i < n - 1; i++) { 
        int min = i; 
        for (int j = i + 1; j < n; j++) {
            if (array[min] < array[j]) 
                min = j; 
        }
        
        //Also swap the index array exactly as the results and use that
        //to sort the array of the result words
        int key = array[min]; 
        int temp = index_array[min];
        
        while (min > i) { 
            array[min] = array[min - 1]; 
            
            index_array[min] = index_array[min - 1];
            min--; 
        } 
        array[i] = key; 
        index_array[i] = temp;
    } 
} 



/* *********************************************************************** */
/* References:

The code above may be inspired by following websites:

https://www.w3resource.com/c-programming-exercises/file-handling/c-file-
handling-exercise-4.php

https://www.techiedelight.com/trie-implementation-insert-search-delete/

https://www.geeksforgeeks.org/auto-complete-feature-using-trie/

https://www.geeksforgeeks.org/stable-selection-sort/
*/

