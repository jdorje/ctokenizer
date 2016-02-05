/*
 * tokenizer.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {FALSE, TRUE} BOOL;

/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */

struct TokenizerT_ {
	char* orig, *currtokentype;
	BOOL malloced_token;
	int current_index_in_orig, orig_len;
};

typedef struct TokenizerT_ TokenizerT;

struct coperator_ {
	char* op, *opname;
};

typedef struct coperator_ coperator;

char* undefined = "undefined", *decimal = "decimal integer", *octal = "octal integer", *hex = "hexadecimal integer", *ffloat = "floating point integer", *word = "word", *keyword = "keyword", *badtoken = "bad token", *cstring = "string";

				coperator singles[] = {{"*", "multiplier"}, {"/", "divider"}, {"%", "modulus"}, {"+", "add"}, {"-", "subtract"}, {"<", "less than"}, {">", "greater than"}, {"&", "bitwise and"}, {"^", "bitwise exclusive or"}, {"|", "bitwise or"}, {"?", "ternary true"}, {":", "ternary false"}, {",", "comma operator"}, {"!", "negate"}, {"~", "1's comp"}, {"(", "function start"}, {")", "function end"}, {"[", "array begin"}, {"]", "array end"}, {".", "structure member"}, {NULL, NULL}};

				coperator doubles[] = {{">>", "shift right"}, {"<<", "shift left"}, {"<=", "less or equal"}, {">=", "greater or equal"}, {"==", "equals"}, {"!=", "not equals"}, {"&&", "logical and"}, {"||", "logical or"}, {"++", "inc"}, {"--", "dec"}, {"->", "structure pointer"}, {"+=", "add-and-assign"}, {"-=", "subtract-and-assign"}, {"*=", "multiply-and-assign"},{"/=", "divide-and-assign"}, {"%=", "modulus-and-assign"}, {"&=", "bitwise-and-and-assign"}, {"^=", "xor-and-assign"}, {"|=", "or-and-assign"}, {"/*", "begin comment block"}, {"*/", "end comment block"}, {"//", "line comment"}, {NULL, NULL}};

				coperator triples[] = {{">>=", "shift-right-and-assign"}, {"<<=", "shift-left-and-assign"}, {NULL, NULL}};

char* ckeywords[] = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"};

BOOL isdelimiter(char c)
{
	return (isspace(c) || c == '\0');
}
/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 * 
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

TokenizerT *TKCreate( char * ts ) {
	TokenizerT* me = (TokenizerT*)malloc(sizeof(TokenizerT));
	
	me->orig = malloc(strlen(ts)+1);
	if (me->orig != NULL) {
		strcpy(me->orig, ts);
		me->current_index_in_orig = 0;
		me->orig_len = strlen(me->orig);
		me->currtokentype = undefined;
		me->malloced_token = TRUE;
		
		return me;
	}

  return NULL;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy( TokenizerT * tk ) {
	
	free (tk->orig);
	free(tk);
}

BOOL ishexdigit(char c)
{
	if ( ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')) )
		return TRUE;
	else
		return FALSE;
}

/*
 * operatortest takes a TokenizerT object in the process of being scanned, and two result pointers
 * if it detects any of the hardcoded operators (up to 3 in length, e.g. ">>=", higher len prioritized)
 * then it returns an index to a global array of strings with the particular type of C operator detected.
 *
 * operatortest is its own function because TKGetNextToken uses it twice: once, to output the operator detected
 * and another time when an operator could possibly delimit the end of a bad token
*/

BOOL operatortest(int current_starting_index_of_orig, TokenizerT* tk, int* sdtresult, int* indexresult)
{
	int singleindex = 0;
	char otc_orig = tk->orig[current_starting_index_of_orig];

	while (singles[singleindex].op != NULL) { 
		if (otc_orig == singles[singleindex].op[0]) {
			/* found a single and possibly a double */
			int doubleindex = 0;
			char next = tk->orig[current_starting_index_of_orig+1];
			while (doubles[doubleindex].op != NULL) {
				if ((otc_orig == doubles[doubleindex].op[0]) && (next == doubles[doubleindex].op[1])) {
					/* found a double and possibly a triple */
					
					int tripleindex = 0;
					char nnext = tk->orig[current_starting_index_of_orig+2];
				
					while(triples[tripleindex].op != NULL) {  
						if ((otc_orig == triples[tripleindex].op[0]) && (next == triples[tripleindex].op[1]) && (nnext == triples[tripleindex].op[2])) {
							/* return triple */

							*sdtresult = 3;
							*indexresult = tripleindex;
							return TRUE;
						}
						tripleindex++;
					}

					/* return double */	
					*sdtresult = 2;
					*indexresult = doubleindex;
					return TRUE;								
				}
				doubleindex++;
			}	
				
			/* return single */
			*sdtresult = 1;
			*indexresult = singleindex;
			return TRUE;
		}
		singleindex++;
	}

	return FALSE;
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken( TokenizerT * tk ) {

	int token_start_index = tk->current_index_in_orig;
	int current_index_in_token = token_start_index;

	while(current_index_in_token < tk->orig_len+1)
	{

		char c_orig = tk->orig[current_index_in_token];
		
		if (isalpha(c_orig)) {
			int tokenlen = 0;
			while(isalnum(c_orig)) {
				tokenlen++;
				c_orig = tk->orig[++current_index_in_token];
			}
			char *TheToken = malloc(tokenlen+1);
			
			strncpy(TheToken, &(tk->orig[token_start_index]), tokenlen);
			TheToken[tokenlen+1] = '\0';
				
			tk->current_index_in_orig = current_index_in_token;

			int i = 0;
			for(;i < sizeof(ckeywords)/sizeof(ckeywords[0]); i++)
			{
				if (strcmp(ckeywords[i], TheToken) == 0)
				{
					tk->currtokentype = keyword;
					free(TheToken);
					tk->malloced_token = FALSE;
					return ckeywords[i];
				}
			}


			tk->currtokentype = word;
			tk->malloced_token = TRUE; 
			return TheToken;						
		} else if (isdigit(c_orig)) {
			if (c_orig == '0') {

				char next = tk->orig[current_index_in_token+1];
				if ((next == 'x') || (next == 'X')) {
					/*Hex time */
					char nnext = tk->orig[current_index_in_token+2];
					if (ishexdigit(nnext)) {
						int tokenlen = 2;
						current_index_in_token += 2;
						/* see if it exceeds length of orig, eventually */
						c_orig = tk->orig[current_index_in_token];
					
						while(ishexdigit(c_orig)) {
							tokenlen++;
							c_orig = tk->orig[++current_index_in_token];
						}

						char* TheToken = malloc(tokenlen+1);
										
						strncpy(TheToken, &(tk->orig[token_start_index]), tokenlen);
						TheToken[tokenlen+1] ='\0';

						tk->current_index_in_orig = current_index_in_token;
						tk->currtokentype = hex;
						tk->malloced_token = TRUE; 
						return TheToken;
					}
				} else if ((next >= '0') && (next <= '7')) {

					int tokenlen = 0;
					while ((c_orig >= '0') && (c_orig <= '7')) {
						tokenlen++;
						c_orig = tk->orig[++current_index_in_token];
					}

					char* TheToken = malloc(tokenlen+1);
										
					strncpy(TheToken, &(tk->orig[token_start_index]), tokenlen);
					TheToken[tokenlen+1] ='\0';

					tk->current_index_in_orig = current_index_in_token;
					tk->currtokentype = octal;

					tk->malloced_token = TRUE; 
					return TheToken;
				} 

			}

			/* regular decimal */
			BOOL is_float = FALSE;

			int tokenlen = 1;
			current_index_in_token++;
			c_orig = tk->orig[current_index_in_token];

			while(TRUE) {

				if (isdigit(c_orig)){
					tokenlen++;
					c_orig = tk->orig[++current_index_in_token];
				} else if (c_orig == '.') {
					if (!is_float) {
						char next = tk->orig[current_index_in_token+1];
						if (isdigit(next)) {
							is_float = TRUE;
							tokenlen++;
							c_orig = tk->orig[++current_index_in_token];
						} else {
							/* the dot could be the C object operator*/
							break;
						}
					} else {
						break;
					}

				} else if (c_orig == 'e') {
					if (is_float) {
						char nnext = tk->orig[current_index_in_token+1];
						char nnnext = tk->orig[current_index_in_token+2];
						if (nnext == '-') {
							if (isdigit(nnnext)) {
								tokenlen+= 2;
								current_index_in_token += 2;
								c_orig = tk->orig[current_index_in_token];
							} else {
								break;
							}
						} else {
							if (isdigit(nnext)) {
								tokenlen++;
								c_orig = tk->orig[++current_index_in_token];
							} else {
								break;
							}
						}
						continue;
					}
					break;
				} else {
					break;
				}
			}


			char* TheToken = malloc(tokenlen+1);
			strncpy(TheToken, &(tk->orig[token_start_index]), tokenlen);
			TheToken[tokenlen+1] ='0';

			tk->current_index_in_orig = current_index_in_token;
			tk->currtokentype = is_float ? ffloat : decimal;

			tk->malloced_token = TRUE; 
			return TheToken;
		} else {
			/* operator test */

			int single_double_or_triple = -1;
			int index_of_sdt = -1;

			if (operatortest(current_index_in_token, tk, &single_double_or_triple, &index_of_sdt) == TRUE)
			{
				tk->malloced_token = FALSE;
				current_index_in_token += single_double_or_triple;
				tk->current_index_in_orig = current_index_in_token;

				switch(single_double_or_triple)
				{
					case 1:
						tk->currtokentype = singles[index_of_sdt].opname;
						return singles[index_of_sdt].op;
					case 2:

						if (strcmp(doubles[index_of_sdt].op, "/*") == 0) {
							/* block comment test */
							char* rest_of_string = &tk->orig[current_index_in_token];
							int i = 0, n = strlen(rest_of_string);
							for(; i < n; i++)
							{
								if (strncmp(&rest_of_string[i], "*/", 2) == 0)
								{
									tk->current_index_in_orig += i+2;
									return TKGetNextToken(tk);
								}	
							}	
						} else if (strcmp(doubles[index_of_sdt].op, "//") == 0) {
							/* line comment test */
							char *rest_of_string = &tk->orig[current_index_in_token];
							int i = 0, n = strlen(rest_of_string);
							for(; i < n; i++)
							{
								if (rest_of_string[i] == '\n')
								{
									tk->current_index_in_orig += i+1;
									return TKGetNextToken(tk);
								}
							}

							return NULL;
						}

						tk->currtokentype = doubles[index_of_sdt].opname;
						return doubles[index_of_sdt].op;
					case 3:
						tk->currtokentype = triples[index_of_sdt].opname;
						return triples[index_of_sdt].op;
					default:
						printf("AAAAAAAAAAHHHHHHHHHHH\n");
						exit(-1);
				}	
			}

			/* not an operator, continue on*/
		}

		if ((c_orig == '"') || (c_orig == '\''))
		{
			BOOL is_double_quote = (c_orig == '"');
			char* rest_of_string = &tk->orig[current_index_in_token];
			int i = 1, n = strlen(rest_of_string);
			for(; i < n; i++)
			{
				if ( ((rest_of_string[i] == '\'') && !is_double_quote) ||
				     ((rest_of_string[i] == '"') && is_double_quote))
				{

					int tokenlen = (current_index_in_token+i) - token_start_index;
					char* TheToken = malloc(tokenlen);
					if (TheToken != NULL) {
						strncpy(TheToken, &(tk->orig[token_start_index+1]), tokenlen-1);
						TheToken[tokenlen] = '\0';

						tk->current_index_in_orig = current_index_in_token+i+1;
						tk->currtokentype = cstring;
						tk->malloced_token = TRUE;
						return TheToken;
					}
				}
			}
		}

		/* looks like a bad token, which can  be stopped by a delimeter OR start of good token type */

		while (TRUE) {
			int dummy_s_d_t = -1;
			int dummy_sdtindex = -1;
		
			if ( (isalnum(c_orig)) || (isdelimiter(c_orig)) || (operatortest(current_index_in_token, tk, &dummy_s_d_t, &dummy_sdtindex)) ) 
			{
				int tokenlen = current_index_in_token - token_start_index;
				if (tokenlen > 0) {
					char* TheToken = malloc(tokenlen);
					if (TheToken != NULL) {
						strncpy(TheToken, &(tk->orig[token_start_index]), tokenlen);
						TheToken[tokenlen] = '\0';
					
						tk->current_index_in_orig = current_index_in_token;				
						tk->currtokentype = badtoken;
						tk->malloced_token = TRUE; return TheToken;
					}	
				} else if (tokenlen == 0) {
					token_start_index++;
					break;
				}
			}

			current_index_in_token++;
			c_orig = tk->orig[current_index_in_token];
		}

		current_index_in_token++;

	}

	tk->current_index_in_orig = current_index_in_token;

	

  	return NULL;
}

void printbadtoken(char* bt)
{
	printf("%s \"%s\" [", badtoken, bt); 
	int i = 0, n = strlen(bt);

	for(;i < n; i++)
	{
		printf("%X", bt[i]);
	}
	printf("]\n");
}

/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) {

	if (argv[1] != NULL) {
		TokenizerT* t = TKCreate(argv[1]);
		if (t != NULL) {
			char *tok = TKGetNextToken(t);
			while (tok != NULL) {
				if (strcmp(t->currtokentype, badtoken) == 0) {
					printbadtoken(tok);
				} else {
					printf("%s \"%s\"\n", t->currtokentype, tok); 
				}
				if (t->malloced_token == TRUE) {
					free(tok);
					t->malloced_token = FALSE;
				}
				tok = TKGetNextToken(t);
			}
			TKDestroy(t);
		} else {
			printf("couldn't create TokenizerT... ?_?\n");
			return -1;
		}
	} else {
		printf("invalid usage: use %s \"string-to-tokenize\"\n", argv[0]);
		return -1;
	}	

  return 0;
}
