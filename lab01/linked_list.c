#include "linked_list.h"
#include <limits.h>

node *linked_list_init() {
    node *head = (node *) malloc(sizeof(node));
    head->count = 0;
    head->next = NULL;
    return head;
}

void linked_list_free(node *head) {
    node *cur = head;
    node *last;
    while (cur != NULL) {
        last = cur;
        cur = cur->next;
        free(last);
    }
}

char linked_list_string[0x10000];

char *linked_list_tostring(node *head) {
    node *cur = head->next;
    char *position;
    int length = 0;
    while (cur != NULL) {
        position = linked_list_string + length;
        length += sprintf(position, "%d", cur->value);
        cur = cur->next;
        if (cur != NULL) {
            position = linked_list_string + length;
            length += sprintf(position, "->");
        }
    }
    position = linked_list_string + length;
    length += sprintf(position, "%c", '\0');
    return linked_list_string;
}

int linked_list_size(node *head) {
    return head->count;
}

void linked_list_append(node *head, int val) {
    node *cur = head;
    node *new_node;
    while (cur->next != NULL) {
        cur = cur->next;
    }
    new_node = (node *) malloc(sizeof(node));
    new_node->value = val;
    new_node->next = NULL;
    cur->next = new_node;
    head->count++;
}

/* your implementation goes here */

void linked_list_insert(node *head, int val, int index) {
    node* origin_head = head;

    while (index != 0 && head != NULL) {
        index--;
        head = head->next;
    }

    if (head == NULL) {
        return;
    }

    node *new_node = (node *) malloc(sizeof(node));
    new_node->value = val;

    node *next = head->next;
    head->next = new_node;
    new_node->next = next;

    origin_head->count++;
}

void linked_list_delete(node *head, int index) {
    node* origin_head = head;

    while (index != 0 && head != NULL) {
        index--;
        head = head->next;
    }

    if (head == NULL) {
        return;
    }

    node *to_delete = head->next;
    if (to_delete == NULL) {
        return;
    }

    node *next = to_delete->next;
    head->next = next;

    free(to_delete);
    origin_head->count--;
}

void linked_list_remove(node *head, int val) {
    node* origin_head = head;
    while (head->next != NULL) {
        if (head->next->value == val) {
            node *to_delete = head->next;
            head->next = to_delete->next;
            free(to_delete);
            origin_head->count--;
            return;
        } else {
            head = head->next;
        }
    }
}

void linked_list_remove_all(node *head, int val) {
    node* origin_head = head;
    while (head->next != NULL) {
        if (head->next->value == val) {
            node *to_delete = head->next;
            head->next = to_delete->next;
            free(to_delete);
            origin_head->count--;
        } else {
            head = head->next;
        }
    }
}

int linked_list_get(node *head, int index) {
    head = head->next;
    while (index != 0 && head != NULL) {
        index--;
        head = head->next;
    }

    if (head == NULL) {
        return INT_MIN;
    }

    return head->value;
}

int linked_list_search(node *head, int val) {
    head = head->next;
    int pos = 0;
    while (head != NULL) {
        if (head->value == val) {
            return pos;
        } else {
            head = head->next;
            pos++;
        }
    }
    return -1;
}

node *linked_list_search_all(node *head, int val) {
    node *result = linked_list_init();

    head = head->next;
    int pos = 0;
    while (head != NULL) {
        if (head->value == val) {
            linked_list_append(result, pos);
        }
        head = head->next;
        pos++;

    }
    return result;
}
