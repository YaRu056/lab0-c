#include "queue.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


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
    node->value = strdup(s);
    if (!node->value) {
        printf("Fail to allocate space to string!\n");
        free(node);
        return false;
    }
    INIT_LIST_HEAD(&node->list);
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
    node->value = strdup(s);
    if (!node->value) {
        printf("Fail to allocate space to string!\n");
        free(node);
        return false;
    }
    INIT_LIST_HEAD(&node->list);
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
    list_del(&node->list);
    if (sp) {
        size_t cp_size =
            strlen(node->value) > bufsize ? (bufsize - 1) : strlen(node->value);
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
    list_del(&node->list);
    if (sp) {
        size_t cp_size =
            strlen(node->value) > bufsize ? (bufsize - 1) : strlen(node->value);
        strncpy(sp, node->value, cp_size);
        *(sp + cp_size) = '\0';
    }
    return node;
}
/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
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
    struct list_head *cur = head->next, *cmp = head->next->next;
    element_t *n_cur, *safe = NULL, *n_cmp;
    bool dup = false;
    while ((cmp != head && cur != head) || (!cur && !cmp)) {

        n_cur = list_entry(cur, element_t, list);
        n_cmp = list_entry(cmp, element_t, list);
        if (cmp->next == head) {
            break;
        }
        list_for_each_entry_safe (n_cmp, safe, cur, list) {
            if (strcmp(n_cur->value, n_cmp->value) == 0) {
                list_del(&n_cmp->list);
                q_release_element(n_cmp);
                dup = true;
            }
            if (safe->list.next == head) {
                if (strcmp(n_cur->value, safe->value) == 0) {
                    list_del(&safe->list);
                    q_release_element(safe);
                    dup = true;
                }
                break;
            }
        }
        if (dup) {
            cur = cur->next;
            cmp = cur->next;
            list_del(&n_cur->list);
            q_release_element(n_cur);
            dup = false;
        } else {
            cur = cur->next;
            cmp = cur->next;
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
    return;
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
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *p = head->next, *q = p->next;
    struct list_head *tmp;
    int q_sz = q_size(head);
    int i = 1;
    while (i < q_sz) {
        int j = i + 1;
        element_t *n_p = list_entry(p, element_t, list);
        element_t *n_q = list_entry(q, element_t, list);
        while (j <= q_sz) {
            n_q = list_entry(q, element_t, list);
            if (strcmp(n_p->value, n_q->value) > 0 && !descend) {
                // printf("change(<)\n");
                n_swap(p, q);
                tmp = q;
                q = p;
                p = tmp;
                n_p = list_entry(p, element_t, list);
            } else if (strcmp(n_p->value, n_q->value) < 0 && descend) {
                // printf("change(>)\n");
                n_swap(p, q);
                tmp = q;
                q = p;
                p = tmp;
                n_p = list_entry(p, element_t, list);
            }
            q = q->next;
            ++j;
        }
        p = p->next;
        q = p->next;
        ++i;
    }
    return;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
