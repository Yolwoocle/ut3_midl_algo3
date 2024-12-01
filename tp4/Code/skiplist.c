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


void print_node(Node* node, void* env) {
	(void)env;
	printf("[%d]:\n", node->value);
	for (int i = 0; i < node->nb_links; i++) {
		printf("- (%d): ", i);
		if (node->links[i].prev != NULL) {
			printf("[%d]", node->links[i].prev->value);
		}
		else {
			printf("[NULL]");
		}
		if (node->links[i].next != NULL) {
			printf(" <-> [%d]\n", node->links[i].next->value);
		}
		else {
			printf(" <-> [NULL]\n");

		}
	}
}


SkipList* skiplist_insert(SkipList* d, int value) {
	int nb_links = rng_get_value(&(d->rng)) + 1;
	Node** nodes_to_connect = malloc(sizeof(Node*) * d->nb_levels);

	Node* new_node = node_create(nb_links);
	new_node->value = value;

	Node* node = d->sentinel;
	int i_link = node->nb_links - 1;

	while (i_link >= 0) {
		if (node->links[i_link].next == NULL || node->links[i_link].next->value > value) {
			nodes_to_connect[i_link] = node;
			i_link--;
		}
		else if (node->links[i_link].next->value < value) {
			node = node->links[i_link].next;
		}
		else {
			free(nodes_to_connect);
			return d;
		}
	}

	for (int i_node = 0; i_node < nb_links; i_node++) {
		new_node->links[i_node].prev = new_node;
		new_node->links[i_node].next = nodes_to_connect[i_node]->links[i_node].next;

		nodes_to_connect[i_node]->links[i_node].next = new_node;
	}

	d->size++;
	free(nodes_to_connect);

	return d;
}

bool skiplist_search(const SkipList* d, int value, unsigned int* nb_operations) {
	int i_link = d->nb_levels - 1;
	Node* node = d->sentinel;
	*nb_operations = 0;

	while (i_link >= 0) {
		Link link = node->links[i_link];
		*nb_operations += 1;
		printf("- %d\n", node->value);

		if (link.next == NULL || link.next->value > value) {
			i_link--;

		} else if (link.next->value < value) {
			node = link.next;

		} else if (link.next->value == value) {
			return true;

		}
	}
	return false;
}
