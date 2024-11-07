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

#define OPERATOR_STACK_MAX_SIZE 512

/**
 * Utilities function to print the token queues
 */
void print_token(const void *e, void *user_param);
void print_queue(FILE *f, Queue *q);
bool operation_while_loop_condition(const Stack* operators, Token* o1);
Queue* shuntingYard(Queue* infix);
bool isSymbol(char c);
bool isDigit(char c);
Queue *stringToTokenQueue(const char *expression);
void computeExpressions(FILE *input);
Token* evaluateOperator(Token* arg1, Token* op, Token* arg2);
float evaluateExpression(Queue * postfix);


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
 * Compute the expressions in the given file.
 */
void computeExpressions(FILE *input)
{
	(void)input;

	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	while ((nread = getline(&line, &len, input)) != -1) {
		printf("Input:\t ");
		fwrite(line, nread, 1, stdout);

		Queue *tokenQueue = stringToTokenQueue(line);
		printf("Infix:\t ");
		print_queue(stdout, tokenQueue);
		printf("\n");

		Queue *queue = shuntingYard(tokenQueue);
		printf("Postfix:\t ");
		print_queue(stdout, queue);
		printf("\n");

		float value = evaluateExpression(queue);
		printf("Evaluate:\t %f\n", value);
		printf("\n");

		delete_queue(&queue);
	}

	free(line);
}


/**
 * Intended to be used in the shunting yard algorithm, when the currently parsed token is an operator. 
 * Given the operator stack and the currently parsed token o1, returns whether: 
 * 
 * There is an operator o2 at the top of the operator stack which is not a left parenthesis, 
 * and (o2 has greater precedence than o1 or (o1 and o2 have the same precedence and o1 is left-associative)).
 */
bool operation_while_loop_condition(const Stack* operators, Token* token) {
	Token * operator;
	return (
		(
			!stack_empty(operators) && 
			token_is_operator(operator = (Token *) stack_top(operators))
		) && (
			(token_operator_priority(operator) > token_operator_priority(token)) || 
			(
				token_operator_priority(operator) == token_operator_priority(token) && 
				token_operator_leftAssociative(operator)
			)
		) && !(token_is_parenthesis(operator) && token_parenthesis(operator) == '(')
	);
}

/**
 * Implements the shunting yard algorithm. Takes as input an expression in infix form and returns it in postfix form.
 */
Queue* shuntingYard(Queue* infix) {
	Queue * output = create_queue();
	Stack * operators = create_stack(queue_size(infix));

	while (!queue_empty(infix)) {
		Token* token = (Token *) queue_top(infix);
		queue_pop(infix);

		if (token_is_number(token)) {
			queue_push(output, token);

		} else if (token_is_operator(token)) {
			while (operation_while_loop_condition(operators, token)) {
				queue_push(output, stack_top(operators));
				stack_pop(operators);
			}
			stack_push(operators, token);

		} else if (token_is_parenthesis(token) && token_parenthesis(token) == '(') {
			stack_push(operators, token);

		} else if (token_is_parenthesis(token) && token_parenthesis(token) == ')') {
			while (!stack_empty(operators) && !(token_is_parenthesis(stack_top(operators)) && token_parenthesis(stack_top(operators)) == '(')) {
				assert(!stack_empty(operators));

				queue_push(output, stack_top(operators));
				stack_pop(operators);
			}
			assert(!stack_empty(operators) && token_is_parenthesis(stack_top(operators)) && token_parenthesis(stack_top(operators)) == '(');
			stack_pop(operators);

		}
	}

	while (!stack_empty(operators)) {
		assert(!stack_empty(operators) && !(token_is_parenthesis(stack_top(operators)) && token_parenthesis(stack_top(operators)) == '('));
		queue_push(output, stack_top(operators));
		stack_pop(operators);
	}

	return output;
}


/**
 * Evaluate an expression composed of a number, an operator, and another number.
 */
Token* evaluateOperator(Token* arg1, Token* op, Token* arg2) {
	assert(token_is_number(arg1));
	assert(token_is_operator(op));
	assert(token_is_number(arg2));

	float value1 = token_value(arg1);
	float value2 = token_value(arg2);
	if (token_operator(op) == '+') {
		return create_token_from_value(value1 + value2);

	} else if (token_operator(op) == '-') {
		return create_token_from_value(value1 - value2);
		
	} else if (token_operator(op) == '*') {
		return create_token_from_value(value1 * value2);

	} else if (token_operator(op) == '/') {
		return create_token_from_value(value1 / value2);

	} else if (token_operator(op) == '^') {
		return create_token_from_value(powf(value1, value2));
	}

	printf("evaluateOperator: Error while parsing: invalid operator %c\n", token_operator(op));
	return NULL;
}

/**
 * Evaluates an expression in postfix form, and returns its value.
 */
float evaluateExpression(Queue * postfix) {
	Stack * stack = create_stack(queue_size(postfix));

	while (!queue_empty(postfix)) {
		Token * token = (Token *) queue_top(postfix);
		queue_pop(postfix);

		if (token_is_operator(token)) {
			Token * value2 = (Token *) stack_top(stack);
			stack_pop(stack);
			Token * value1 = (Token *) stack_top(stack);
			stack_pop(stack);
			assert(token_is_number(value1));
			assert(token_is_number(value2));

			Token * result = evaluateOperator(value1, token, value2);
			stack_push(stack, result);

		} else if (token_is_number(token)) {
			stack_push(stack, token);

		}
	}

	const Token * result = stack_top(stack);
	assert(token_is_number(result));
	delete_stack(&stack);
	return token_value(result);
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