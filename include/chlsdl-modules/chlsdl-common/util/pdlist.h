#ifndef PDLIST_H_
#define PDLIST_H_

#include <string.h>

enum pdlist_node_priority {
    /* pdlist_node_priority_invalii = -2147483649, */
    pdlist_node_priority_invalid = -1000000000, /* FIXME: underflow */
    pdlist_node_priority_bulk    = -10,
    pdlist_node_priority_normal  = 0,
    pdlist_node_priority_urgent  = 10,
};

struct pdlist_node {
    void *               data;
    int                  priority;
    struct pdlist_node * prev;
    struct pdlist_node * next;
};

struct pdlist {
    struct pdlist_node * first;
    size_t               size; /* for resizing only */
};

extern struct pdlist *
pdlist_create(size_t n);

extern bool
pdlist_push(struct pdlist * list, const void * data,
    enum pdlist_node_priority priority);

extern bool
pdlist_pop(struct pdlist * list, struct pdlist_node * r);

extern void
pdlist_destroy(struct pdlist * pdlist);

#endif // PDLIST_H_
