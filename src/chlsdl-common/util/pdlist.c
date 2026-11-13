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

static inline bool
pdlist_node_is_empty(const struct pdlist_node * node)
{
    return node->data == NULL;
}

static struct pdlist_node *
pdlist_node_first_lower_priority(
    const struct pdlist_node * node, enum pdlist_node_priority priority)
{
    for (; node->next; node = node->next) {
        if (node->priority < priority)
            return (struct pdlist_node *)node;
    }
    return NULL;
}

static struct pdlist_node *
pdlist_node_first_empty(const struct pdlist_node * node)
{
    for (; node; node = node->next) {
        if (pdlist_node_is_empty(node))
            return (struct pdlist_node *)node;
    }

    /* should never happen. `node->next` being `NULL` can though */
    return NULL;
}

static void
pdlist_node_remove(const struct pdlist_node * node)
{
    struct pdlist_node * prev = node->prev;
    struct pdlist_node * next = node->next;
    prev->next                = next;
    next->prev                = prev;
}

static void
pdlist_node_insert(struct pdlist_node * node, struct pdlist_node * prev,
    struct pdlist_node * next)
{
    if (prev)
        prev->next = node;
    node->prev = prev;

    if (next)
        next->prev = node;
    node->next = next;
}

bool
pdlist_push(
    struct pdlist * list, const void * data, enum pdlist_node_priority priority)
{
    struct pdlist_node * node = list->first;
    if (pdlist_node_is_empty(node)) {
        node->data     = (void *)data;
        node->priority = priority;
        return true;
    }

    struct pdlist_node * next_node
        = pdlist_node_first_lower_priority(node, priority);
    assert(next_node);

    if (pdlist_node_is_empty(next_node)) {
        next_node->data     = (void *)data;
        next_node->priority = priority;
        return true;
    }

    struct pdlist_node * empty = pdlist_node_first_empty(next_node);
    assert(empty);

    if (!empty->next)
        return false; /* TODO: resize */

    pdlist_node_remove(empty);

    struct pdlist_node * prev_node = next_node->prev;

    empty->data     = (void *)data;
    empty->priority = priority;

    pdlist_node_insert(empty, prev_node, next_node);

    if (next_node == node)
        list->first = empty;

    return true;
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
