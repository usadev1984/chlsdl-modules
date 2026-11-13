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

static inline void
pdlist_node_insert_after(struct pdlist_node * node, struct pdlist_node * prev)
{
    prev->next = node;
    node->prev = prev;
}

static inline struct pdlist_node *
pdlist_find_last(const struct pdlist * list)
{
    const struct pdlist_node * r = list->first;
    while (r->next)
        r = r->next;
    return (struct pdlist_node *)r;
}

static bool
pdlist_resize(struct pdlist * list, size_t old_size, struct pdlist_node * last)
{
    struct pdlist_node * new = pdlist_node_create(old_size * 0.5);
    if (!new)
        return false;

    if (!last)
        if (!(last = pdlist_find_last(list)))
            return false;

    assert(!last->next);
    pdlist_node_insert_after(new, last);
    list->size += old_size * 0.5;
    return true;
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

    if (!empty->next) {
        print_debug_warn("resizing\n");
        if (!pdlist_resize(list, list->size, empty))
            return false;
    }

    pdlist_node_remove(empty);

    struct pdlist_node * prev_node = next_node->prev;

    empty->data     = (void *)data;
    empty->priority = priority;

    pdlist_node_insert(empty, prev_node, next_node);

    if (next_node == node)
        list->first = empty;

    return true;
}

bool
pdlist_pop(struct pdlist * list, struct pdlist_node * r)
{
    struct pdlist_node * node = list->first;
    if (pdlist_node_is_empty(node))
        return false;

    list->first = node->next;

    *r = *node;
    pdlist_node_zero(node);

    struct pdlist_node * last = pdlist_find_last(list);

    last->next = node;
    node->next = NULL;
    node->prev = last;

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
