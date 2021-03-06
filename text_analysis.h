/* * * * * * *
 * Text Analysis module for Assignment 2.
 *
 * created for COMP20007 Design of Algorithms 2020
 * template by Tobias Edwards <tobias.edwards@unimelb.edu.au>
 * implementation by Zahra Abbasian Korbekandi <ID: 1032221>
 */

#ifndef TEXT_ANALYSIS_H
#define TEXT_ANALYSIS_H




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>



#define COL 256 
#define ROW 1000 
#define CHAR_SIZE 87   //The range of characters in ASCII from '$' to 'z'
#define MAX_PRINT 5
#define TRUE 1

// A Struct to store a Trie node
struct Trie
{
	int freq_count;
	int is_leaf;	// 1 when node is a leaf node and 0 when not
	struct Trie* character[CHAR_SIZE];
};

//Functions:
struct Trie* new_trie_node();
void insert(struct Trie *root, char* str);
void preorder_traverse(struct Trie* root);
void free_trie(struct Trie* root);
void display(struct Trie* root, char str[], int level, int depth);
int display_prefix(struct Trie* root, char str[], int level, char* prefix, 
	char com_words[ROW][COL+1], int result_freq[]);
bool check_prefix(char* str, char* prefix);
void selection_sort(int array[], int n, int index_array[]);
float prefix_freq(struct Trie* root, char* prefix);








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
void problem_2_a();

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
void problem_2_b();

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
void problem_2_c();

#endif
