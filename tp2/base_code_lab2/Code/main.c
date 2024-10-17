// Need this to use the getline C function on Linux. Works without this on MacOs. Not tested on Windows.
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "token.h"
#include "queue.h"
#include "stack.h"

/**
 * Utilities function to print the token queues
 */
void print_token(const void *e, void *user_param);
void print_queue(FILE *f, Queue *q);

/**
 * Une fonction bool isSymbol(char c) renvoyant vrai si le caractère c est un symbole
 * de l’expression (’+’, ’−’, ’∗’, ’/’, ’∧’, ’(’ ou ’)’).
 */
bool isSymbol(char c)
{
	char *testChrs = "+-*/^()";
	int len = strlen(testChrs);
	for (int i = 0; i < len; i++)
	{
		if (c == testChrs[i])
			return true;
	}
	return false;
}

/**
 * Une fonction bool isSymbol(char c) renvoyant vrai si le caractère c est un chiffre
 * ('0' à '9').
 */
bool isDigit(char c)
{
	return '0' <= c && c <= '9';
}

/**
 * Prend en paramètre une chaîne de caractères et transforme cette chaîne de caractères en file de
 * Token.
 */
Queue *stringToTokenQueue(const char *expression)
{
	Queue *queue = create_queue();

	char *curpos = (char *)expression;
	int tokenLen;
	while (*curpos != '\0')
	{
		tokenLen = 0;
		if (*curpos == '\0' || *curpos == '\n' || *curpos == ' ') {
			//...

		} else if (isSymbol(*curpos)) {
			tokenLen = 1;

		} else if (isDigit(*curpos)) {
			tokenLen = 0;
			while (isDigit(*(curpos + tokenLen))) {
				tokenLen++;
			}

		} else {

		}

		if (tokenLen > 0) {
			Token *token = create_token_from_string(curpos, tokenLen);
			queue_push(queue, (void *)token);

			curpos += tokenLen - 1;
		}

		curpos++;
	}

	return queue;
}

/**
 * Function to be written by students
 */
void computeExpressions(FILE *input)
{
	(void)input;

	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	while ((nread = getline(&line, &len, input)) != -1) {
		printf("Input (len %3zd): ", nread);
		fwrite(line, nread, 1, stdout);

		Queue *queue = stringToTokenQueue(line);
		print_queue(stdout, queue);
		printf("\n");

		delete_queue(&queue);
	}

	free(line);
}

Queue* shuntingYard(Queue* infix) {
	Queue * output = create_queue();
	Stack * operators = create_stack();

	while (!queue_empty(infix)) {
		Token* token = (Token *) queue_top(infix);
		queue_pop(infix);

		if (token_is_number(token)) {
			queue_push(output, (void *) token);
		} else if (token_is_operator(token)) {
			/*
				there is an operator o2 at the top of the operator stack which is not a left parenthesis, 
            	and (o2 has greater precedence than o1 or (o1 and o2 have the same precedence and o1 
				is left-associative))
			*/
			while () {

			}
		}
	}

	return output;
}

/** Main function for testing.
 * The main function expects one parameter that is the file where expressions to translate are
 * to be read.
 *
 * This file must contain a valid expression on each line
 *
 */
int main(int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "usage : %s filename\n", argv[0]);
		return 1;
	}

	FILE *input = fopen(argv[1], "r");

	if (!input)
	{
		perror(argv[1]);
		return 1;
	}

	computeExpressions(input);

	fclose(input);
	return 0;
}

void print_token(const void *e, void *user_param)
{
	FILE *f = (FILE *)user_param;
	Token *t = (Token *)e;
	token_dump(f, t);
}

void print_queue(FILE *f, Queue *q)
{
	fprintf(f, "(%d) --  ", queue_size(q));
	queue_map(q, print_token, f);
}