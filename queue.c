#include "queue.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list_sort.h"
/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Using __builtin_constant_p(x) to ignore cases where the return
 * value is always the same.  This idea is taken from a similar patch
 * written by Daniel Walker.
 */

int swap_me = 0, swap_l = 0;
/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head) {
        printf("Fail to allocate space to an empty queue!\n");
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        q_release_element(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *node = (element_t *) malloc(sizeof(element_t));
    if (!node) {
        return false;
    }
    INIT_LIST_HEAD(&node->list);
    node->value = strdup(s);
    if (!node->value) {
        // printf("Fail to allocate space to string!\n");
        free(node);
        return false;
    }
    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *node = (element_t *) malloc(sizeof(element_t));
    if (!node) {
        return false;
    }
    INIT_LIST_HEAD(&node->list);
    node->value = strdup(s);
    if (!node->value) {
        // printf("Fail to allocate space to string!\n");
        free(node);
        return false;
    }
    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *node;
    node = list_first_entry(head, element_t, list);
    if (node == NULL) {
        free(node);
        return NULL;
    }
    list_del(&node->list);
    int len = strlen(node->value);
    if (sp) {
        size_t cp_size = len > bufsize ? (bufsize - 1) : len;
        strncpy(sp, node->value, cp_size);
        *(sp + cp_size) = '\0';
    }
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *node;
    node = list_last_entry(head, element_t, list);
    if (node == NULL) {
        free(node);
        return NULL;
    }
    list_del(&node->list);
    int len = strlen(node->value);
    if (sp) {
        size_t cp_size = len > bufsize ? (bufsize - 1) : len;
        strncpy(sp, node->value, cp_size);
        *(sp + cp_size) = '\0';
    }
    return node;
}
/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    int len = 0;
    struct list_head *tmp;
    list_for_each (tmp, head) {
        len++;
    }
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }
    element_t *node;
    struct list_head *curr = head;
    int q_sz = floor(q_size(head) / 2);
    while (q_sz--) {
        curr = curr->next;
    }
    curr = curr->next;
    node = list_entry(curr, element_t, list);
    list_del(&node->list);
    q_release_element(node);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head)) {
        return false;
    }
    element_t *n_cmp, *safe;
    bool dup = false;
    list_for_each_entry_safe (n_cmp, safe, head, list) {
        if (strcmp(n_cmp->value, safe->value) == 0) {
            list_del(&n_cmp->list);
            q_release_element(n_cmp);
            dup = true;
        } else {
            if (dup) {
                list_del(&n_cmp->list);
                q_release_element(n_cmp);
                dup = false;
            }
        }
        if (safe->list.next == head) {
            if (dup) {
                list_del(&safe->list);
                q_release_element(safe);
            }
            break;
        }
    }
    return true;
}
void n_swap(struct list_head *node1, struct list_head *node2)
{
    struct list_head *tmp = node1->next;
    node1->next = node2->next;
    node2->next = tmp;
    if (node1->next != NULL) {
        node1->next->prev = node1;
    }
    if (node2->next != NULL) {
        node2->next->prev = node2;
    }
    tmp = node1->prev;
    node1->prev = node2->prev;
    node2->prev = tmp;
    if (node1->prev != NULL) {
        node1->prev->next = node1;
    }
    if (node2->prev != NULL) {
        node2->prev->next = node2;
    }
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *node1 = head->next, *node2 = node1->next;
    int q_sz = floor(q_size(head) / 2);
    while (q_sz) {
        n_swap(node1, node2);
        node1 = node1->next;
        node2 = node1->next;
        q_sz--;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *cur = head->prev, *tmp = NULL;
    int size = q_size(head);
    while (size > 0) {
        tmp = cur->prev;
        list_move_tail(cur, head);
        cur = tmp;
        size--;
    }

}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    int s = q_size(head);
    if (k > s) {
        return;
    }
    LIST_HEAD(tmp);
    LIST_HEAD(newHead);
    struct list_head *tail = NULL;
    int group = s / k;
    while (group) {
        int j = 0;
        list_for_each (tail, head) {
            if (j >= k) {
                break;
            }
            j++;
        }
        list_cut_position(&tmp, head, tail->prev);
        q_reverse(&tmp);
        list_splice_tail_init(&tmp, &newHead);
        group--;
    }
    list_splice_init(&newHead, head);
}
void mergesort(struct list_head *h1, struct list_head *h2, bool descend)
{
    if (!h1 || !h2) {
        return;
    }
    LIST_HEAD(newHead);
    while (!list_empty(h1) && !list_empty(h2)) {
        element_t *n_h1, *n_h2, *tmp;
        n_h1 = list_first_entry(h1, element_t, list);
        n_h2 = list_first_entry(h2, element_t, list);
        // swap_me++;
        if (!descend) {
            tmp = strcmp(n_h1->value, n_h2->value) < 0 ? n_h1 : n_h2;
        } else {
            tmp = strcmp(n_h1->value, n_h2->value) < 0 ? n_h1 : n_h2;
        }
        list_move_tail(&tmp->list, &newHead);
    }
    list_splice_tail_init(h1, &newHead);
    list_splice_tail_init(h2, &newHead);
    list_splice(&newHead, h1);
    return;
}
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    swap_me = 0;
    LIST_HEAD(tmp1);
    LIST_HEAD(tmp2);
    int s = q_size(head) / 2 - 1;
    struct list_head *tail = NULL;
    list_for_each (tail, head) {
        if (s < 0) {
            break;
        }
        s--;
    }
    list_cut_position(&tmp1, head, tail->prev);
    list_cut_position(&tmp2, tail->prev, head->prev);
    q_sort(&tmp1, descend);
    q_sort(&tmp2, descend);
    mergesort(&tmp1, &tmp2, descend);
    list_splice_init(&tmp1, head);
    // printf("My sort swap times: %d \n",swap_me);
    return;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return 0;
    }
    if (list_is_singular(head)) {
        return 1;
    }
    element_t *left = list_entry(head->next, element_t, list);
    element_t *right = list_entry(head->next->next, element_t, list);
    while (&right->list != head) {
        if (strcmp(left->value, right->value) < 0) {
            left = list_entry(left->list.next, element_t, list);
            right = list_entry(right->list.next, element_t, list);

        } else {
            list_del(&right->list);
            q_release_element(right);
            right = list_entry(left->list.next, element_t, list);
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return 0;
    }
    if (list_is_singular(head)) {
        return 1;
    }
    element_t *left = list_entry(head->prev->prev, element_t, list);
    element_t *right = list_entry(head->prev, element_t, list);
    while (&left->list != head) {
        if (strcmp(left->value, right->value) > 0) {
            left = list_entry(left->list.prev, element_t, list);
            right = list_entry(right->list.prev, element_t, list);

        } else {
            list_del(&left->list);
            q_release_element(left);
            left = list_entry(right->list.prev, element_t, list);
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // head: header of chain
    //  https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head)) {
        return 0;
    } else if (list_is_singular(head)) {
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);
    }
    queue_contex_t *first = list_first_entry(head, queue_contex_t, chain);
    queue_contex_t *cur = NULL, *safe = NULL;
    list_for_each_entry_safe (cur, safe, head, chain) {
        if (cur == first) {

            continue;
        }
        list_splice_init(cur->q, first->q);
        first->size += cur->size;
        cur->size = 0;
    }
    q_sort(first->q, descend);
    return first->size;
}
void q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    } else if (list_is_singular(head)) {
        return;
    }
    int n = q_size(head);
    for (int i = n - 1; i > 0; i--) {
        // Generate a random index between 0 and i (inclusive)
        int rand_index = rand() % (i + 1);
        struct list_head *old = head, *new = NULL, *cur = NULL;
        int s = 0;
        list_for_each (cur, head) {
            if (rand_index == i) {
                if (s == i) {
                    old = cur;
                    new = cur;
                    break;
                }
            } else {
                if (s == rand_index) {
                    new = cur;
                } else if (s == i) {
                    old = cur;
                    break;
                }
            }
            s++;
        }
        n_swap(old, new);
    }
}
static struct list_head *merge(void *priv,
                               struct list_head *a,
                               struct list_head *b,
                               bool descend)

{
    if (list_empty(a)) {
        return NULL;
    }
    if (list_empty(b)) {
        return NULL;
    }
    struct list_head **tail = &head;
    element_t *n_h1, *n_h2;
    if (!descend) {
        for (;;) {
            /* if equal, take 'a' -- important for sort stability */
            n_h1 = list_entry(a, element_t, list);
            n_h2 = list_entry(b, element_t, list);
            // swap_l++;
            if (strcmp(n_h1->value, n_h2->value) <= 0) {
                // a<=b
                *tail = a;
                tail = &a->next;
                a = a->next;
                if (!a) {
                    *tail = b;
                    break;
                }
            } else {
                // a>b
                *tail = b;
                tail = &b->next;
                b = b->next;
                if (!b) {
                    // b list no element
                    *tail = a;
                    break;
                }
            }
        }
    } else {
        for (;;) {
            /* if equal, take 'a' -- important for sort stability */
            n_h1 = list_entry(a, element_t, list);
            n_h2 = list_entry(b, element_t, list);
            // swap_l++;
            if (strcmp(n_h1->value, n_h2->value) <= 0) {
                // a<=b
                *tail = b;
                tail = &b->next;
                b = b->next;
                if (!b) {
                    // b list no element
                    *tail = a;
                    break;
                }
            } else {
                // a>b
                *tail = a;
                tail = &a->next;
                a = a->next;
                if (!a) {
                    *tail = b;
                    break;
                }
            }
        }
    }
    return head;
}
static void merge_final(void *priv,
                        struct list_head *head,
                        struct list_head *a,
                        struct list_head *b,
                        bool descend)
{
    if (!a || list_empty(a)) {
        return;
    }
    if (!b || list_empty(b)) {
        return;
    }
    struct list_head *tail = head;
    element_t *n_h1, *n_h2;
    if (!descend) {
        for (;;) {
            /* if equal, take 'a' -- important for sort stability */
            n_h1 = list_entry(a, element_t, list);
            n_h2 = list_entry(b, element_t, list);
            // swap_l++;
            if (strcmp(n_h1->value, n_h2->value) <= 0) {
                tail->next = a;
                a->prev = tail;
                tail = a;
                a = a->next;
                if (!a)
                    break;
            } else {
                tail->next = b;
                b->prev = tail;
                tail = b;
                b = b->next;
                if (!b) {
                    b = a;
                    break;
                }
            }
        }
    } else {
        for (;;) {
            /* if equal, take 'a' -- important for sort stability */
            n_h1 = list_entry(a, element_t, list);
            n_h2 = list_entry(b, element_t, list);
            // swap_l++;
            if (strcmp(n_h1->value, n_h2->value) <= 0) {
                tail->next = b;
                b->prev = tail;
                tail = b;
                b = b->next;
                if (!b) {
                    b = a;
                    break;
                }
            } else {
                tail->next = a;
                a->prev = tail;
                tail = a;
                a = a->next;
                if (!a)
                    break;
            }
        }

    }
    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

void list_sort(void *priv, struct list_head *head, bool descend)
{

    swap_l = 0;
    int s = q_size(head) - 1;
    if (list_empty(head)) {
        return;
    } else if (list_is_singular(head)) {
        return;
    }
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */
    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;
    do {
        size_t bits;
        struct list_head **tail = &pending;
        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1) {
            tail = &(*tail)->prev;
        }
        /* Do the indicated merge */
        if (bits) {
            struct list_head *a = *tail, *b = a->prev;

            a = merge(priv, b, a, descend);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (s--);

    /* End of input; merge together all the pending lists. */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;
        if (!next)
            break;
        list = merge(priv, pending, list, descend);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    merge_final(priv, head, pending, list, descend);
    // printf("Linux sort swap times: %d\n",swap_l);
}
