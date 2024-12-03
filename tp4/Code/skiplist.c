#include <stdlib.h>
#include <assert.h>

#include "skiplist.h"
#include "rng.h"

struct s_Node;

typedef struct s_Link {
	struct s_Node* prev;
	struct s_Node* next;
} Link;


typedef struct s_Node {
	int value;
	int nb_links;
	struct s_Link* links;
} Node;


struct s_SkipList {
	Node* sentinel;
	RNG rng;
	int nb_levels;
	unsigned int size;
};


Node* node_create(int nb_links) {
	Node* node = malloc(sizeof(Node));

	node->value = 0;
	node->nb_links = nb_links;
	node->links = malloc(sizeof(Link) * nb_links);
	for (int i = 0; i < nb_links; i++) {
		node->links[i].prev = NULL;
		node->links[i].next = NULL;
	}

	return node;
}


void node_delete(Node** node) {
	free((*node)->links);
	free((*node));
	*node = NULL;
}


SkipList* skiplist_create(int nblevels) {
	(void)nblevels;

	SkipList* list = malloc(sizeof(SkipList));
	Node* sentinel = node_create(nblevels);

	list->sentinel = sentinel;
	for (int i = 0; i < nblevels; i++) {
		list->sentinel->links[i].next = sentinel;
		list->sentinel->links[i].prev = sentinel;
	}
	list->rng = rng_initialize(0, nblevels);
	list->nb_levels = nblevels;
	list->size = 0;

	return list;
}


void skiplist_delete(SkipList** d) {
	Node* node = (*d)->sentinel;
	Node* next = node->links[0].next;

	while (node != (*d)->sentinel) {
		next = node->links[0].next;
		node_delete(&node);
		node = next;
	}

	free(*d);
	*d = NULL;
}


unsigned int skiplist_size(const SkipList* d) {
	return d->size;
}


int skiplist_at(const SkipList* d, unsigned int n) {
	Node* node = d->sentinel->links[0].next;
	for (unsigned int i = 0; i < n; i++) {
		node = node->links[0].next;
	}
	return node->value;
}


void skiplist_map(const SkipList* d, ScanOperator f, void* user_data) {
	Node* node = d->sentinel->links[0].next;
	while (node != d->sentinel) {
		f(node->value, user_data);
		node = node->links[0].next;
	}
}


void print_node(Node* node, SkipList* list) {

	if (node == list->sentinel) {
		printf("[SENTINEL]:\n");
	} else {
		printf("[%d] ():\n", node->value);
	}
	for (int i = node->nb_links-1; i >= 0; i--) {
		printf("- %d: ", i);
		Node * prev = node->links[i].prev;
		Node * next = node->links[i].next;
		if (prev == list->sentinel) {
			printf("[SENTINEL]");
		} else if (prev != NULL) {
			printf("[%d]", prev->value);
		} else {
			printf("[NULL]");
		}
		if (next == list->sentinel) {
			printf(" <-> [SENTINEL]\n");
		} else if (next != NULL) {
			printf(" <-> [%d]\n", next->value);
		} else {
			printf(" <-> [NULL]\n");

		}
	}
}

void print_list_verbose(SkipList* list) {
	Node* node = list->sentinel;

	printf("SkipList: len = %d\n", list->size);
	do {
		print_node(node, list);
		node = node->links[0].next;
	} while (node != list->sentinel);
	printf("\n");
}


SkipList* skiplist_insert(SkipList* d, int value) {
	Node** nodes_to_connect = malloc(sizeof(Node*) * d->nb_levels);

	Node* node = d->sentinel;
	int i_link = node->nb_links - 1;

	while (i_link >= 0) {
		Node* next = node->links[i_link].next;
		if (next == d->sentinel || next->value > value) {
			nodes_to_connect[i_link] = node;
			i_link--;

		}
		else if (next->value < value) {
			node = next;

		}
		else {
			free(nodes_to_connect);
			return d;
		}
	}

	int nb_links = rng_get_value(&(d->rng)) + 1;

	Node* new_node = node_create(nb_links);
	new_node->value = value;

	for (int i_node = 0; i_node < nb_links; i_node++) {
		new_node->links[i_node].prev = nodes_to_connect[i_node];
		new_node->links[i_node].next = nodes_to_connect[i_node]->links[i_node].next;

		nodes_to_connect[i_node]->links[i_node].next = new_node;
		new_node->links[i_node].next->links[i_node].prev = new_node;
	}

	d->size++;
	free(nodes_to_connect);

	print_list_verbose(d);
	return d;
}

bool skiplist_search(const SkipList* d, int value, unsigned int* nb_operations) {
	Node* node = d->sentinel;
	int i_link = d->sentinel->nb_links - 1;
	Node* next_node;

	*nb_operations = 1;

	while (i_link >= 0) {
		next_node = node->links[i_link].next;

		while (next_node != d->sentinel && next_node->value < value) {
			node = next_node;
			next_node = node->links[i_link].next;
			(*nb_operations)++;
		}

		if (next_node != d->sentinel && next_node->value == value) {
			return true;
		}

		i_link--;
	}

	return false;
}


/*
bool skiplist_search(const SkipList* d, int value, unsigned int* nb_operations) {
	int i_link = d->nb_levels - 1;
	Node* node = d->sentinel;
	*nb_operations = 0;

	while (i_link >= 0) {
		Link link = node->links[i_link];
		// printf("- %d\n", node->value);
		*nb_operations += 1;

		if (link.next == d->sentinel || link.next->value > value) {
			i_link--;

		} else if (link.next->value < value) {
			node = link.next;

		} else if (link.next->value == value) {
			return true;

		}
	}
	return false;
}
*/


SkipList* skiplist_remove(SkipList* d, int value) {
	int i_link = d->nb_levels - 1;
	Node* node = d->sentinel;

	while (i_link >= 0) {
		Link link = node->links[i_link];

		if (link.next == d->sentinel || link.next->value > value) {
			i_link--;

		}
		else if (link.next->value < value) {
			node = link.next;

		}
		else if (link.next->value == value) {
			i_link = -1;

		}
	}

	for (int i = 0; i < node->nb_links; i++) {
		Link* links = node->links;

		links[i].prev->links[i].next = links[i].next;
	}
	node_delete(&node);

	return d;
}



/*-----------------------*/
/* Iterator              */
/*-----------------------*/

struct s_SkipListIterator {
	SkipList* list;
	IteratorDirection direction;

	Node* position;
};


SkipListIterator* skiplist_iterator_create(SkipList* d, IteratorDirection w) {
	SkipListIterator* iterator = malloc(sizeof(SkipListIterator));

	iterator->list = d;
	iterator->direction = w;

	return iterator;
}


void skiplist_iterator_delete(SkipListIterator** it) {
	free(*it);
	*it = NULL;
}


SkipListIterator* skiplist_iterator_begin(SkipListIterator* it) {
	it->position = it->list->sentinel->links[0].next;
	return it;
}


bool skiplist_iterator_end(SkipListIterator* it) {
	return it->position == it->list->sentinel;
}


SkipListIterator* skiplist_iterator_next(SkipListIterator* it) {
	it->position = it->position->links[0].next;
	return it;
}


int skiplist_iterator_value(SkipListIterator* it) {
	return it->position->value;
}