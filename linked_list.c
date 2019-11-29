#include <stdlib.h>
#include "linked_list.h"
#include <stdio.h>

linked_list_t* ll_create(void *data) 
{
    linked_list_t *node = (linked_list_t*)malloc(sizeof(linked_list_t));
    node->user_data = data;
    node->next = NULL;
    return node;
}

linked_list_t* ll_append(linked_list_t *head, void *user_data) 
{
    if (!head) {
        head = ll_create(user_data);
        return head;
    }
    linked_list_t *p = head;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = ll_create(user_data);
    return head;
}

linked_list_t *ll_poph(linked_list_t **head)
{
    if (!*head) {
        return NULL;
    }
    linked_list_t *out = *head;
    if ((*head)->next) {
        *head = (*head)->next;
    } else {
        *head = NULL;
    }
    return out;
}

unsigned int ll_len(linked_list_t *head)
{
    if (!head) {
        return 0;
    }
    int i = 0;
    linked_list_t *p = head;
    while (p) {
        i++;
        p = p->next;
    }
    return i;
}

void *ll_get_at_index(linked_list_t *head, uint32_t idx)
{
    linked_list_t *p = head;
    uint32_t i = 0;

    while (i++ != idx) {
       if (p->next) {
           p = p->next;
       } else {
           return NULL;
       }
    }

    return p->user_data;

}

void ll_free(linked_list_t *ll, free_user_data_func_t freefn)
{
    linked_list_t *head;
    while (head = ll_poph(&ll)) {
        void *res = head->user_data;
        if (freefn) {
            freefn(res);
        } else {
            free(res);
        }
        free(head);
    }
}
