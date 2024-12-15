#include "bstree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"


/*------------------------  BSTreeType  -----------------------------*/

struct _bstree {
    int key;
    BinarySearchTree* parent;
    BinarySearchTree* left;
    BinarySearchTree* right;
};

/*------------------------  BaseBSTree  -----------------------------*/

BinarySearchTree* bstree_create(void) {
    return NULL;
}

/* This constructor is private so that we can maintain the oredring invariant on
 * nodes. The only way to add nodes to the tree is with the bstree_add function
 * that ensures the invariant.
 */
BinarySearchTree* bstree_cons(BinarySearchTree* left, BinarySearchTree* right, int key) {
    BinarySearchTree* t = malloc(sizeof(struct _bstree));
    t->parent = NULL;
    t->left = left;
    t->right = right;
    if (t->left != NULL)
        t->left->parent = t;
    if (t->right != NULL)
        t->right->parent = t;
    t->key = key;
    return t;
}

void freenode(const BinarySearchTree* t, void* n) {
    (void)n;
    free((BinarySearchTree*)t);
}

void bstree_delete(ptrBinarySearchTree* t) {
    bstree_depth_postfix(*t, freenode, NULL);
    *t = NULL;
}

bool bstree_empty(const BinarySearchTree* t) {
    return t == NULL;
}

int bstree_key(const BinarySearchTree* t) {
    assert(!bstree_empty(t));
    return t->key;
}

BinarySearchTree* bstree_left(const BinarySearchTree* t) {
    assert(!bstree_empty(t));
    return t->left;
}

BinarySearchTree* bstree_right(const BinarySearchTree* t) {
    assert(!bstree_empty(t));
    return t->right;
}

BinarySearchTree* bstree_parent(const BinarySearchTree* t) {
    assert(!bstree_empty(t));
    return t->parent;
}

bool bstree_is_leaf(const BinarySearchTree* t) {
    if (bstree_empty(t)) {
        return false;
    }
    return bstree_empty(t->left) && bstree_empty(t->right);
}

/*------------------------  BSTreeDictionary  -----------------------------*/

/* Obligation de passer l'arbre par référence pour pouvoir le modifier */
void bstree_add(ptrBinarySearchTree* t, int v) {
    BinarySearchTree** address_to_place = t;
    BinarySearchTree* parent = (*t);

    while (*address_to_place != NULL) {
        BinarySearchTree* current_tree = (*address_to_place);

        parent = current_tree;
        if (current_tree->key > v) {
            address_to_place = &(current_tree->left);
        }
        else if (current_tree->key < v) {
            address_to_place = &(current_tree->right);
        }
        else {
            return;
        }
    }

    BinarySearchTree* new_tree = bstree_cons(NULL, NULL, v);
    *address_to_place = new_tree;
    (*address_to_place)->parent = parent;
}

const BinarySearchTree* bstree_search(const BinarySearchTree* t, int v) {
    const BinarySearchTree* cursor = t;

    while (cursor != NULL) {
        if (cursor->key > v) {
            cursor = cursor->left;
        } else if (cursor->key < v) {
            cursor = cursor->right;
        } else {
            return cursor; 
        }
    }

    return NULL;
}

const BinarySearchTree* bstree_successor(const BinarySearchTree* t) {
    const BinarySearchTree* cursor;
    if (t->right != NULL) {
        cursor = t->right;
        while (cursor->left != NULL) {
            cursor = cursor->left;
        }
        return cursor;
        
    } else if (t->parent != NULL) {
        cursor = t;
        while (cursor->parent && cursor->parent->left != cursor) {
            cursor = cursor->parent;
        }
        return cursor->parent;

    }
    return NULL;
}

const BinarySearchTree* bstree_predecessor(const BinarySearchTree* t) {
    const BinarySearchTree* cursor;
    if (t->left != NULL) {
        cursor = t->left;
        while (cursor->right != NULL) {
            cursor = cursor->right;
        }
        return cursor;
        
    } else if (t->parent != NULL) {
        cursor = t;
        while (cursor->parent && cursor->parent->right != cursor) {
            cursor = cursor->parent;
        }
        return cursor->parent;

    }
    return NULL;
}

void swap(BinarySearchTree** a, BinarySearchTree** b) {
    BinarySearchTree* tmp = *a;
    *a = *b;
    *b = tmp;
}


void bstree_swap_nodes(ptrBinarySearchTree *tree, ptrBinarySearchTree from, ptrBinarySearchTree to) {
    assert(!bstree_empty(*tree) && !bstree_empty(from) && !bstree_empty(to));

    if (from == *tree)
        *tree = to;
    if (to == *tree)
        *tree = from;

    swap(&(from->parent), &(to->parent));

    if (from->parent) {
        if (from->parent->left == to)
            from->parent->left = from;
        else
            from->parent->right = from;
    } 

    if (to->parent) {
        if (to->parent->left == from)
            to->parent->left = to;
        else 
            to->parent->right = to;
    }

    swap(&(from->left), &(to->left));
    swap(&(from->right), &(to->right));

    if (from->left) from->left->parent = from;
    if (from->right) from->right->parent = from;
    if (to->left) to->left->parent = to;
    if (to->right) to->right->parent = to;
}


// t -> the tree to remove from, current -> the node to remove
void bstree_remove_node(ptrBinarySearchTree* t, ptrBinarySearchTree current) {
    assert(!bstree_empty(*t) && !bstree_empty(current));
    
    if (current->left == NULL && current->right == NULL) {
        // Leaf node

        if (current->parent->left == current) {
            current->parent->left = NULL;
        } else {
            current->parent->right = NULL;
        }
        free(current);

    } else if ((current->left == NULL && current->right != NULL) || (current->left != NULL && current->right == NULL)) {
        // Only one child
        if (current->left != NULL) {
            bstree_swap_nodes(t, current, current->left);
        } else {
            bstree_swap_nodes(t, current, current->right);
        }
        bstree_remove_node(t, current);

    } else {
        // Two childs
        BinarySearchTree* successor = (BinarySearchTree*) bstree_successor(current);
        bstree_swap_nodes(t, current, successor);

        bstree_remove_node(t, current);
    }
}

void bstree_remove(ptrBinarySearchTree* t, int v) {
    BinarySearchTree* node = (BinarySearchTree*) bstree_search(*t, v);

    if (node == NULL) {
        return;
    }

    bstree_remove_node(t, node);
}

/*------------------------  BSTreeVisitors  -----------------------------*/

void bstree_depth_prefix(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    if (t == NULL) {
        return;
    }

    f(t, environment);
    bstree_depth_prefix(t->left, f, environment);
    bstree_depth_prefix(t->right, f, environment);
}

void bstree_depth_infix(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    if (t == NULL) {
        return;
    }

    bstree_depth_infix(t->left, f, environment);
    f(t, environment);
    bstree_depth_infix(t->right, f, environment);
}

void bstree_depth_postfix(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    if (t == NULL) {
        return;
    }

    bstree_depth_postfix(t->left, f, environment);
    bstree_depth_postfix(t->right, f, environment);
    f(t, environment);
}

void bstree_iterative_breadth(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    Queue* queue = create_queue();
    queue_push(queue, (void *) t);

    while (!queue_empty(queue)) {
        BinarySearchTree* cursor = (BinarySearchTree *) queue_top(queue);
        queue_pop(queue);

        if (cursor->left != NULL) {
            queue_push(queue, cursor->left);
        }
        if (cursor->right != NULL) {
            queue_push(queue, cursor->right);
        }

        f(cursor, environment);
    }
}

void bstree_iterative_depth_infix(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    if (t == NULL) {
        return;
    }

    // Get the minimum node
    BinarySearchTree* cursor = (BinarySearchTree*) t;
    while (cursor->left != NULL) {
        cursor = cursor->left;
    }

    while (cursor != NULL) {
        f(cursor, environment);
        cursor = (BinarySearchTree*) bstree_successor(cursor);
    }
}

/*------------------------  BSTreeIterator  -----------------------------*/

struct _BSTreeIterator {
    /* the collection the iterator is attached to */
    const BinarySearchTree* collection;
    /* the first element according to the iterator direction */
    const BinarySearchTree* (*begin)(const BinarySearchTree*);
    /* the current element pointed by the iterator */
    const BinarySearchTree* current;
    /* function that goes to the next element according to the iterator direction */
    const BinarySearchTree* (*next)(const BinarySearchTree*);
};

/* minimum element of the collection */
const BinarySearchTree* goto_min(const BinarySearchTree* e) {
    if (e == NULL) {
        return NULL;
    }

    BinarySearchTree* cursor = (BinarySearchTree*) e;
    while (cursor->left != NULL) {
        cursor = cursor->left;
    }

    return cursor;
}

/* maximum element of the collection */
const BinarySearchTree* goto_max(const BinarySearchTree* e) {
    if (e == NULL) {
        return NULL;
    }

    BinarySearchTree* cursor = (BinarySearchTree*) e;
    while (cursor->right != NULL) {
        cursor = cursor->right;
    }

    return cursor;
}

/* constructor */
BSTreeIterator* bstree_iterator_create(const BinarySearchTree* collection, IteratorDirection direction) {
    BSTreeIterator* it = malloc(sizeof(BSTreeIterator));

    it->collection = collection;
    it->current = NULL;
    if (direction == forward) {
        it->begin = goto_min;
        it->next = bstree_successor;

    } else {
        it->begin = goto_max;
        it->next = bstree_predecessor;
    }   

    return it;
}

/* destructor */
void bstree_iterator_delete(ptrBSTreeIterator* i) {
    free(*i);
    *i = NULL;
}

BSTreeIterator* bstree_iterator_begin(BSTreeIterator* i) {
    i->current = i->begin(i->collection);
    return i;
}

bool bstree_iterator_end(const BSTreeIterator* i) {
    return i->current == NULL;
}

BSTreeIterator* bstree_iterator_next(BSTreeIterator* i) {
    i->current = i->next(i->current);
    return i;
}

const BinarySearchTree* bstree_iterator_value(const BSTreeIterator* i) {
    return i->current;
}

