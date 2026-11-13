#include <chlsdl-modules/chlsdl-common/util/pdlist.h>

#include <assert.h>
#include <chlsdl-modules/chlsdl-common/print.h>
#include <stdlib.h>
#include <string.h>

static inline void
pdlist_node_zero(struct pdlist_node * node)
{
    memset(node, 0, sizeof(*node));
    node->priority = pdlist_node_priority_invalid;
}

static struct pdlist_node *
pdlist_node_create(size_t n)
{
    struct pdlist_node * r    = NULL;
    struct pdlist_node * prev = NULL; /* relative to current */
    /* struct pdlist_node * next = NULL; */

    for (size_t i = 0; i < n; ++i) {
        struct pdlist_node * node = malloc(sizeof(*node));
        assert(node);
        pdlist_node_zero(node);

        if (!r)
            r = node;

        if (prev) { /* useless if? */
            prev->next = node;
            node->prev = prev;
        }

        prev = node;
    }

    /* prev->next = NULL; */

    return r;
}

struct pdlist *
pdlist_create(size_t n)
{
    struct pdlist * r = malloc(sizeof(*r));
    if (!r)
        return NULL;

    r->first = pdlist_node_create(n);
    if (!r->first) {
        free(r);
        return NULL;
    }

    r->size = n;
    return r;
}

static void
pdlist_nodes_free(struct pdlist_node * node)
{
    for (struct pdlist_node * tmp; node;) {
        tmp = node->next;
        free(node);
        node = tmp;
    }
}

void
pdlist_destroy(struct pdlist * list)
{
    pdlist_nodes_free(list->first);
    free(list);
}
