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
	list->rng = rng_initialize(0, nblevels);
	list->nb_levels = nblevels;
	list->size = 0;

	return list;
}


void skiplist_delete(SkipList** d) {
	Node* node = (*d)->sentinel;
	Node* next = node->links[0].next;

	while (node != NULL) {
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
	while (node != NULL) {
		f(node->value, user_data);
		node = node->links[0].next;
	}
}


SkipList* skiplist_insert(SkipList* d, int value) {
	int nb_links = rng_get_value(&(d->rng));

	Node* new_node = node_create(nb_links);
	new_node->value = value;

	Node* node = d->sentinel;
	int i_link = node->nb_links - 1; // = ??? 
	Link link = node->links[i_link];
	
	while (node != NULL && i_link > 0) {
		i_link = node->nb_links - 1;
		link = node->links[i_link];
		while (i_link > 0 && link.next == NULL && link.next->value > value) {
			link = node->links[i_link];
			i_link++;
		}

		if (i_link == 0) {
			
		} else {
			node = link.next;
		}
	}

	new_node->links[0].next = node->links[0].next;
	new_node->links[0].prev = new_node;
	node->links[0].next = new_node;

	return d;
}
