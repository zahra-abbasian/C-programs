/* 
	Text Formatting and Aligning
	By: Zahra Abbasian
   

*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INPUT_LEN 1000    /*The maximum size of input which is 999 characters*/

/* ****************************************************************/
/* function prototypes */

int sep_text(char input[], char output[INPUT_LEN][INPUT_LEN]);
int remove_command(char str1[], char str2[]);
int remove_newline(char str2[], char str3[]);
int remove_space(char str3[], char str4[]);
void print_text(char sign, char str4[], int left_align, int line_len);
void print_centre(char str4[], int left_align, int line_len);
void print_heading(int count_h1, int count_h2, int count_h3, int left_align, 
	int line_len);




/* ***************************************************************/

/* main program */

int
main(int argc, char *argv[]) {
	
	int i, n;
	int left_align = 4;   /* The default size of left justifying of the text */
	int line_len = 50;    /* The default size of width of a line */
	int count_h1 = 0, count_h2, count_h3;   /* Counting the headings for .h */
	
	char input[INPUT_LEN];
	char new_str_1[INPUT_LEN];
	char new_str_2[INPUT_LEN];
	char new_str_3[INPUT_LEN];
	char output[INPUT_LEN][INPUT_LEN];
	char l_num[2];           /* Stores the new size for left alignment */
	char w_num[2];           /* Stores the new size for the width of a line */
	char h_num;          /* Stores the number indicating the type of headings */
	
	
	
	/* Ask for an input and read it as a string array */
	printf("Enter a text: ");
	scanf("%[^\t]s", input);
   
    /* separate the input over commands and return the number of 
    separated texts */
    n = sep_text(input,output);
   
    
    for (i = 0; i <= n ; i++) {
    	if (output[i][0] == '.' && output[i][1] == 'b') {
    		/* when starting with command .b */
    		remove_command(output[i], new_str_1);
    		remove_newline(new_str_1, new_str_2);
    		remove_space(new_str_2, new_str_3);
    		print_text('b', new_str_3, left_align, line_len);
    	
    	} else if (output[i][0] == '.' && output[i][1] == 'p') {
    		/* when starting with command .p */
    		remove_command(output[i], new_str_1);
    		remove_newline(new_str_1, new_str_2);
    		remove_space(new_str_2, new_str_3);
    		print_text('p', new_str_3, left_align, line_len);
    	
    	} else if (output[i][0] == '.' && output[i][1] == 'c') {
    		/* when starting with command .c */
    		remove_command(output[i], new_str_1);
    		remove_newline(new_str_1, new_str_2);
    		remove_space(new_str_2, new_str_3);
    		print_centre(new_str_3, left_align, line_len);
    		
    	} else if (output[i][0] == '.' && output[i][1] == 'l') {
    		/* when starting with command .l nn */
    		/* Extract the numbers (nn) in front of the command */ 
    		l_num[0] = output[i][3];
    		l_num[1] = output[i][4];
    		l_num[2] = '\0';
    		
    		left_align = atoi(l_num);   /* Change from char to int */
    	
    		remove_command(output[i], new_str_1);
    		remove_newline(new_str_1, new_str_2);
    		remove_space(new_str_2, new_str_3);
    		print_text('l', new_str_3, left_align, line_len);
    		
    	
    	} else if (output[i][0] == '.' && output[i][1] == 'w') {
    		/* when starting with command .w nn */
    		/* Extract the numbers (nn) in front of the command */
    		w_num[0] = output[i][3];
    		w_num[1] = output[i][4];
    		w_num[2] = '\0';
    		line_len = atoi(w_num);
    		
    		remove_command(output[i], new_str_1);
    		remove_newline(new_str_1, new_str_2);
    		remove_space(new_str_2, new_str_3);
    		print_text('w', new_str_3, left_align, line_len);
    		
    	} else if (output[i][0] == '.' && output[i][1] == 'h') {
    		/* when starting with command .h n */
    		h_num = output[i][3];  /*Extract the number in front of the command*/
                                /* There's no need to change it into int */
    		if (h_num == '1') {
    			count_h1++;
    			count_h2 = 0;
    			count_h3 = 0;
    		} else if (h_num == '2') {
    			count_h2++;
    			count_h3 = 0;
    		} else if (h_num == '3') {
    			count_h3++;
    		}
    		
    		remove_command(output[i], new_str_1);
    		remove_newline(new_str_1, new_str_2);
    		remove_space(new_str_2, new_str_3);
    		print_heading(count_h1, count_h2, count_h3, left_align, line_len);
    		print_text('h', new_str_3, left_align, line_len);
    		
    	
    	} else {
    		/* when the first line of a separated text does not start with 
    		a command */
    		remove_newline(output[i], new_str_2);
    		remove_space(new_str_2, new_str_3);
    		print_text('n', new_str_3, left_align, line_len);
    	}
    }
    return 0;
}
    	
    
/* ***************************************************************/ 
   
/* The function that splits input into separate strings based on 
newline and period characters. The structure of this function is inspired by:
https://www.includehelp.com/code-snippets/c-program-to-split-string-into-the-
words-separated-by-spaces.aspx */

int
sep_text(char input[], char output[INPUT_LEN][INPUT_LEN]) {
	int n=0, i, j=0, len;
	
	len = strlen(input); 
	
	for (i = 0; i < len; i++) {
		
		if (input[i] != '\n' && input[i+1] != '.') {
			output[n][j++] = input[i];
		} else if (input[i] != '\n' && input[i+1] == '.') {
			output[n][j++] = input[i];
		} else if (input[i] == '\n' && input[i+1] == '.' && input[i+2] == ' ') {
			output[n][j++] = input[i];		
		} else if (input[i] == '\n' && input[i+1] != '.') {
			output[n][j++] = '\n';
		} else if (input[i] == '\n' && input[i+1] == '.'){
			output[n][j++] = '\0';
			n++;
			j=0;
		}
	
	
		
	}
	return n;
}

/* ***************************************************************/

/* The function that removes lines starting with a command */
int
remove_command(char str1[], char str2[]) {
	
	int i = 0;
	int n;
	int len = 0;
	
	n = strlen(str1);

		if (str1[i] == '.' && (str1[i+1] == 'b' || str1[i+1] == 'p' 
			|| str1[i+1] == 'c')) {
			i = 2;
			while (i<n) {
				str2[len++] = str1[i];
				i++;
			}
			str2[len] = '\0';
			} else if (str1[i] == '.' && str1[i+1] == 'h') {
				i = 4;
				while (i<n) {
				str2[len++] = str1[i];
				i++;
			}
			str2[len] = '\0';
			
		
		} else if (str1[i] == '.' && (str1[i+1] == 'l' || str1[i+1] == 'w')) {
			i = 5;
			while (i<n) {
				str2[len++] = str1[i];
				i++;
			}
			str2[len] = '\0';
		
		} else {
			while (i<n) {
				str2[len++] = str1[i];
				i++;
			}
			str2[len] = '\0';
		}
	
	return 0;
}


/* ***************************************************************/

/* The function that removes newline characters all over the string*/
int
remove_newline(char str2[], char str3[]) {
	int i=0;
	int n;
	int len = 0;
	
	n = strlen(str2);
	while (i<n) {
		if (str2[i] == '\n') {
			str3[len++] = ' ';
			
		} else {
			str3[len++] = str2[i];
		}
		i++;
		
	} 
	str3[len] = '\0';
	return 0;
}
	

/* ***************************************************************/

/* The function that removes multiple spaces all over the string*/
int
remove_space(char str3[], char str4[]) {
	
	int i=0;
	int n;
	int len = 0;
	
	n = strlen(str3);

	while (i<n) {
		if (str3[i] == ' ' && str3[i+1] == ' ' ) {
			i++;
			
		} else {
			str4[len++] = str3[i];
			i++;
		}	
	} 
	str4[len] = '\0';
	return 0;
}

/* ***************************************************************/

/* The function that prints the string. This is partly inspired by:
https://stackoverflow.com/questions/15472299/split-string-into-tokens-and-save-
them-in-an-array */

void
print_text(char sign, char str4[], int left_align, int line_len) {
	int i;
	int j;
	int tok_len;
	
	char *tok = strtok(str4, " ");
	
	if (sign == 'b' || sign == 'l' || sign == 'w' || sign == 'n') {
		printf("\n");
		for (j=0; j < left_align; j++) {
			putchar(' ');
		}
	} else if ( sign == 'p') {
		printf("\n\n");
		for (j=0; j < left_align; j++) {
			putchar(' ');
		}
	} else if (sign == 'h') {
		/* do nothing (no newlines) */
	}
	
	i = 0;
	while(tok != NULL) {
		
	    tok_len = strlen(tok);		
		
	    if (tok_len <= line_len - i) {
			printf("%s ", tok);
			i = i + tok_len + 1;
			
		} else {
			printf("\n");
			for (j=0; j < left_align; j++) {
				putchar(' ');
			}
			printf("%s ", tok);
			i = 0;
			i = i + tok_len + 1;
				
		}
		
		tok = strtok(NULL, " ");
	}
}

/* ***************************************************************/

/* The function that prints the line starting with command .c */
void
print_centre(char str4[], int left_align, int line_len) {
	int n=0,i,j=0, len;
	int tok_len = 0;
	int num_spaces;
	len = strlen(str4);
	char str5[len][len];
	
	printf("\n");
	
	for(i=0;i<len;i++)
	{
		if(str4[i]!=' '){
			str5[n][j++]=str4[i];
		}
		else{
			str5[n][j++]='\0';
			n++;
			j=0;
		}
	}
	
	/* Measuring the size of tokens and spaces needed around them */
	for (i= 0; i<=n; i++) {
		tok_len += strlen(str5[i]);
	}
	
	/* number of spaces needed before the line of type int divided by 2 will
	be already a rounded down number */
	num_spaces = (line_len - (tok_len + n + 1))/2;
	
	for (i=0; i<= left_align + num_spaces; i++) {
		putchar(' ');
	}
		
	char *tok = strtok(str4, " ");
	i = 0;
	while(tok != NULL) {
		printf("%s ", tok);
		
		tok = strtok(NULL, " ");
	}
}

/* ***************************************************************/

/* The function that prints the headings numbers for command .h */
void
print_heading(int count_h1, int count_h2, int count_h3, int left_align,
	int line_len) {
	int i;
	printf("\n");
	for (i=0; i<left_align; i++) {
		putchar(' ');
	}
	if (count_h2 == 0 && count_h3 == 0) {
		for (i=0; i< line_len; i++) {
			printf("-");
		}
		printf("\n");
		for (i=0; i<left_align; i++) {
			putchar(' ');
		}
		printf("%d ", count_h1);
	} else if (count_h2 != 0 && count_h3 == 0) {
		printf("%d.%d ", count_h1, count_h2);
	} else if (count_h3 != 0) {
		printf("%d.%d.%d ", count_h1, count_h2, count_h3);
	}
}

/* ***************************************************************/
/* "ALGORITHMS ARE FUN!!! */
