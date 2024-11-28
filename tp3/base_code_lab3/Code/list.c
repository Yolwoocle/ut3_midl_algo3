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

/* SubList permettant de représenter de façon minimale
(pointeur de tête et pointeur de queue) une liste doublement chaînée sans sentinelle. */
struct s_SubList {
	LinkedElement* head;
	LinkedElement* tail;
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
	while (e != NULL && e != (*l)->sentinel) {
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

	LinkedElement* new_element = malloc(sizeof(LinkedElement));
	new_element->value = v;

	new_element->next = element->next;
	new_element->previous = element;

	element->next->previous = new_element;
	element->next = new_element;

	l->size++;

	return l;
}

/*-----------------------------------------------------------------*/

List* list_remove_at(List* l, int p) {
	LinkedElement* element = l->sentinel->next;
	int i = 0;
	while (i < p) {
		element = element->next;
		i++;
	}

	element->previous->next = element->next;
	element->next->previous = element->previous;
	l->size--;

	free(element);

	return l;
}

/*-----------------------------------------------------------------*/

int list_at(const List* l, int p) {
	LinkedElement* element = l->sentinel->next;
	int i = 0;
	while (i < p) {
		element = element->next;
		i++;
	}

	return element->value;
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

/*
	d´ecoupe une liste l en deux sous
	listes de tailles ´egales `a 1 ´el´ement pr`es. `A partir de la liste doublement chaˆın´ee l cette
	fonction renvoie une structure SubList dont le pointeur de tˆete d´esigne le dernier ´el´ement
	de la sous-liste gauche et le pointeur de queue le premier ´el´ement de la sous liste droite.
*/
SubList list_split(SubList l) {
    LinkedElement* slow = l.head;
    LinkedElement* fast = l.head;

    while (fast != l.tail && fast->next != l.tail) {
        fast = fast->next->next;
        slow = slow->next;
    }

    SubList output; 
	output.head = slow;
	output.tail = slow->next;

	output.head->next = NULL;
    output.tail->previous = NULL;

    return output;
}

/*-----------------------------------------------------------------*/

/*
	permettant de fusionner les deux listes tri´ees leftlist et rightlist en respectant
	l’ordre d´efini par la fonction pass´ee en param`etre f.
*/
SubList list_merge(SubList leftlist, SubList rightlist, OrderFunctor f) {
	LinkedElement* left = leftlist.head;
	LinkedElement* right = rightlist.head;

	SubList merged_list; 
	merged_list.head = NULL;
	merged_list.tail = NULL;
	
	LinkedElement** node;
	while (left != NULL && right != NULL) {
		node = f(left->value, right->value) > 0 ? &left : &right;
		
		LinkedElement* next = (*node)->next;
		if (merged_list.head == NULL) {
			merged_list.head = *node;
			(*node)->previous = NULL;
		}

		if ((*node)->next != NULL) {
			(*node)->next->previous = NULL;
		}

		(*node)->next = NULL;

		if (merged_list.tail != NULL) {
			merged_list.tail->next = *node;
			(*node)->previous = merged_list.tail;
		}
		merged_list.tail = *node;

		*node = next;

		// (*node)->next = NULL;
		// if (next != NULL) {
		// 	next->previous = NULL;
		// }

		// if (merged_list.tail != NULL) {
		// 	merged_list.tail->next = *node;
		// 	(*node)->previous = merged_list.tail;
		// }
		// merged_list.tail = *node;

		// *node = next;
	}

	if (left == NULL) {
		merged_list.tail->next = right;
		right->previous = merged_list.tail;
		merged_list.tail = rightlist.tail;
	}
	else if (right == NULL) {
		merged_list.tail->next = left;
		left->previous = merged_list.tail;
		merged_list.tail = leftlist.tail;
	}

	merged_list.tail->next = NULL;

	return merged_list;
}

SubList list_mergesort(SubList l, OrderFunctor f) {
	if (l.head == l.tail) {
		return l;
	}

	l.head->previous = NULL;
	l.tail->next = NULL;

	SubList split = list_split(l);
	(void) split;
	SubList leftlist = {0};
	SubList rightlist = {0};
	leftlist.head = l.head;
	leftlist.tail = split.head;
	rightlist.head = split.tail;
	rightlist.tail = l.tail;

	leftlist.head->previous = NULL;
	leftlist.tail->next = NULL;
	rightlist.head->previous = NULL;
	rightlist.tail->next = NULL;

	SubList merged = list_merge(list_mergesort(leftlist, f), list_mergesort(rightlist, f), f);

	return merged;
}

List* list_sort(List* l, OrderFunctor f) {
	SubList sublist;
	sublist.head = l->sentinel->next;
	sublist.tail = l->sentinel->previous;

	SubList sorted = list_mergesort(sublist, f);

	l->sentinel->next = sorted.head;
	l->sentinel->previous = sorted.tail;

	l->sentinel->next->previous = l->sentinel;
	l->sentinel->previous->next = l->sentinel;

	return l;
}