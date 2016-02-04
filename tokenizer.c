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

char* undefined = "undefined", *decimal = "decimal integer", *octal = "octal integer", *hex = "hexadecimal integer", *ffloat = "floating point integer", *word = "word";

				coperator singles[] = {{"*", "multiplier"}, {"/", "divider"}, {"%", "modulus"}, {"+", "add"}, {"-", "subtract"}, {"<", "less than"}, {">", "greater than"}, {"&", "bitwise and"}, {"^", "bitwise exclusive or"}, {"|", "bitwise or"}, {"?", "ternary true"}, {":", "ternary false"}, {",", "comma operator"}, {"!", "negate"}, {"~", "1's comp"}, {"(", "function start"}, {")", "function end"}, {"[", "array begin"}, {"]", "array end"}, {".", "structure member"}, {NULL, NULL}};

				coperator doubles[] = {{">>", "shift right"}, {"<<", "shift left"}, {"<=", "less or equal"}, {">=", "greater or equal"}, {"==", "equals"}, {"!=", "not equals"}, {"&&", "logical and"}, {"||", "logical or"}, {"++", "inc"}, {"--", "dec"}, {"->", "structure pointer"}, {"+=", "add-and-assign"}, {"-=", "subtract-and-assign"}, {"*=", "multiply-and-assign"},{"/=", "divide-and-assign"}, {"%=", "modulus-and-assign"}, {"&=", "bitwise-and-and-assign"}, {"^=", "xor-and-assign"}, {"|=", "or-and-assign"}, {NULL, NULL}};

				coperator triples[] = {{">>=", "shift-right-and-assign"}, {"<<=", "shift-left-and-assign"}, {NULL, NULL}};

BOOL isdelimiter(char c)
{
	switch(c)
	{
		case ' ':
		case '\t':
		case '\v':
		case '\f':
		case '\n':
		case '\r':
		case '\0':
			return TRUE;
		default:
			return FALSE;
	}
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


BOOL operatortest(int current_starting_index_of_orig, TokenizerT* tk, int* sdtresult, int* indexresult)
{
	int singleindex = -1;
	char otc_orig = tk->orig[current_starting_index_of_orig];

	while (TRUE) { 
		singleindex++;

		if (singles[singleindex].op == NULL) {
			break;
		} else if (otc_orig == singles[singleindex].op[0]) {
			/* found a single and possibly a double */
			int doubleindex = -1;
			char next = tk->orig[current_starting_index_of_orig+1];
			while (TRUE) {
				doubleindex++;
				if (doubles[doubleindex].op == NULL) {
					break;
				} else if ((otc_orig == doubles[doubleindex].op[0]) && (next == doubles[doubleindex].op[1])) {
					/* found a double and possibly a triple */
					
					int tripleindex = -1;
					char nnext = tk->orig[current_starting_index_of_orig+2];
				
					while(TRUE) {  
						tripleindex++;

						if (triples[tripleindex].op == NULL) {
							break;
						}  else if ((otc_orig == triples[tripleindex].op[0]) && (next == triples[tripleindex].op[1]) && (nnext == triples[tripleindex].op[2])) {
							/* return triple */

							*sdtresult = 3;
							*indexresult = tripleindex;
							return TRUE;
						}
					}

					/* return double */	
					*sdtresult = 2;
					*indexresult = doubleindex;
					return TRUE;								
				}
			}	
				
			/* return single */
			*sdtresult = 1;
			*indexresult = singleindex;
			return TRUE;
		}
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
			tk->currtokentype = word;
			tk->malloced_token = TRUE; return TheToken;						
		} else if (isdigit(c_orig)) {
			if (c_orig == '0') {

				char next = tk->orig[current_index_in_token+1];
				/* strange corner case of ending with 0x or 0 */
				if ((next == 'x') || (next == 'X')) {
					/*Hex time */
					
					int tokenlen = 2;
					current_index_in_token += 2;
					/* see if it exceeds length of orig, eventually */
					c_orig = tk->orig[current_index_in_token];
				
					/* gives "hexadecimal integer: 0x" it bothers me */
					while(ishexdigit(c_orig)) {
						tokenlen++;
						c_orig = tk->orig[++current_index_in_token];
					}

					char* TheToken = malloc(tokenlen+1);
									
					strncpy(TheToken, &(tk->orig[token_start_index]), tokenlen);
					TheToken[tokenlen+1] ='\0';

					tk->current_index_in_orig = current_index_in_token;
					tk->currtokentype = hex;
					tk->malloced_token = TRUE; return TheToken;
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


					tk->malloced_token = TRUE; return TheToken;
				} 

			}

			/* regular decimal */

			/* right now the way it works is that .35 is not treated as a float */
			/* problem with floating point that starts with 0 */

			BOOL is_float = FALSE;

			int tokenlen = 1;
			current_index_in_token++;
			/* do a bounds check? worked fine in hex */
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
						if (nnext == '-') {
							tokenlen+= 2;
							current_index_in_token += 2;
							c_orig = tk->orig[current_index_in_token];
						} else {
							tokenlen++;
							c_orig = tk->orig[++current_index_in_token];
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

			tk->malloced_token = TRUE; return TheToken;
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
						tk->currtokentype = "bad token";
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
				printf("%s \"%s\"\n", t->currtokentype, tok); 
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
