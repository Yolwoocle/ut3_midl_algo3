/*-----------------------------------------------------------------*/
/*
 Licence Informatique - Structures de données
 Mathias Paulin (Mathias.Paulin@irit.fr)

 Implantation du TAD List vu en cours.
 */
 /*-----------------------------------------------------------------*/
#include <stdio.h>
#
#include <stdlib.h>
#include <assert.h>

#include "list.h"

typedef struct s_LinkedElement {
	int value;
	struct s_LinkedElement* previous;
	struct s_LinkedElement* next;
} LinkedElement;

/* Use of a sentinel for implementing the list :
 The sentinel is a LinkedElement* whose next pointer refer always to the head of the list and previous pointer to the tail of the list
 */
struct s_List {
	LinkedElement* sentinel;
	int size;
};


/*-----------------------------------------------------------------*/

List* list_create(void) {
	List* l = malloc(sizeof(List));

	l->size = 0;
	l->sentinel = malloc(sizeof(LinkedElement));
	l->sentinel->value = 0;
	l->sentinel->previous = l->sentinel;
	l->sentinel->next = l->sentinel;

	return l;
}

/*-----------------------------------------------------------------*/

List* list_push_back(List* l, int v) {
	LinkedElement* element = malloc(sizeof(LinkedElement));
	LinkedElement* old_last = l->sentinel->previous;

	element->value = v;

	old_last->next = element;
	element->previous = old_last;

	l->sentinel->previous = element;
	element->next = l->sentinel;

	l->size++;

	return l;
}

/*-----------------------------------------------------------------*/

void list_delete(ptrList* l) {
	LinkedElement* e = (*l)->sentinel->next;
	LinkedElement* next;
	while (e != (*l)->sentinel) {
		next = e->next;
		free(e);
		e = next;
	}

	free((*l)->sentinel);
	free(*l);

	*l = NULL;
}

/*-----------------------------------------------------------------*/

List* list_push_front(List* l, int v) {
	LinkedElement* element = malloc(sizeof(LinkedElement));
	LinkedElement* old_first = l->sentinel->next;

	element->value = v;

	old_first->previous = element;
	element->next = old_first;

	l->sentinel->next = element;
	element->previous = l->sentinel;

	l->size++;

	return l;
}

/*-----------------------------------------------------------------*/

int list_front(const List* l) {
	return l->sentinel->next->value;
}

/*-----------------------------------------------------------------*/

int list_back(const List* l) {
	return l->sentinel->previous->value;
}

/*-----------------------------------------------------------------*/

List* list_pop_front(List* l) {
	LinkedElement* old_head = l->sentinel->next;
	LinkedElement* new_head = old_head->next;

	new_head->previous = l->sentinel;
	l->sentinel->next = new_head;
	
	l->size--;

	free(old_head);
	return l;
}

/*-----------------------------------------------------------------*/

List* list_pop_back(List* l){
	// TOOD check if list is empty
	LinkedElement* old_last = l->sentinel->previous;
	LinkedElement* new_last = old_last->previous;

	new_last->next = l->sentinel;
	l->sentinel->previous = new_last;
	
	l->size--;

	free(old_last);

	return l;
}

/*-----------------------------------------------------------------*/

List* list_insert_at(List* l, int p, int v) {
	LinkedElement* element = l->sentinel;
	int i = 0;
	while (i < p) {
		element = element->next;
		i++;
	}

	// TODO insert element 
	azrijazrjiorzejiorzejijo

	return l;
}

/*-----------------------------------------------------------------*/

List* list_remove_at(List* l, int p) {
	(void)p;
	return l;
}

/*-----------------------------------------------------------------*/

int list_at(const List* l, int p) {
	(void)l;
	return p;
}

/*-----------------------------------------------------------------*/

bool list_is_empty(const List* l) {
	return (l->size == 0);
}

/*-----------------------------------------------------------------*/

int list_size(const List* l) {
	return l->size;
}

/*-----------------------------------------------------------------*/

List* list_map(List* l, ListFunctor f, void* environment) {
	LinkedElement* cursor = l->sentinel->next;
	while (cursor != l->sentinel) {
		cursor->value = f(cursor->value, environment);
		cursor = cursor->next;
	}
	return l;
}

/*-----------------------------------------------------------------*/

List* list_sort(List* l, OrderFunctor f) {
	(void)f;
	return l;
}

