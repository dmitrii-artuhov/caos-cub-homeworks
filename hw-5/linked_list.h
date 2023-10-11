#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char type;
} node_data;

typedef struct node node;
struct node {
    node_data data;
    node* next;
};

typedef struct {
    node* head;
} linked_list;

// Initialize the linked list
void linked_list_init(linked_list* list) {
    list->head = NULL;
}

// Destroy the linked list and free allocated memory
void linked_list_destroy(linked_list* list) {
    node* current = list->head;
    while (current != NULL) {
        node* next = current->next;
        free(current);
        current = next;
    }
    list->head = NULL;
}

// Add a new node with data 'val' to the front of the list
void linked_list_add_front(linked_list* list, node_data val) {
    node* new_node = (node*)malloc(sizeof(node));
    if (new_node == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    new_node->data = val;
    new_node->next = list->head;
    list->head = new_node;
}

// Delete the first node with data 'val' from the list
void linked_list_delete(linked_list* list, node_data val) {
    node* current = list->head;
    node* prev = NULL;

    while (current != NULL) {
        if (current->data.id == val.id && current->data.type == val.type) {
            if (prev != NULL) {
                prev->next = current->next;
            } else {
                list->head = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void linked_list_get_nodes_as_string(linked_list* list, char* buffer, int size) {
    int i = 0;
    
    buffer[i++] = '[';
    
    node* current = list->head;
    while (current != NULL) {
        char label[20] = {0}; // enough because `int` is at max ~2*10^9
        sprintf(label, "%c%d", current->data.type, current->data.id);
        int j = 0;
        while (label[j]) {
            buffer[i++] = label[j++];
        }
        
        if (current->next != NULL) {
            buffer[i++] = ',';
        }
        
        current = current->next;
    }

    buffer[i++] = ']';

    while (i < size) {
        buffer[i++] = ' ';
    }

}

// Print the linked list
void linked_list_print(linked_list* list) {
    node* current = list->head;
    while (current != NULL) {
        printf("%c%d -> ", current->data.type, current->data.id);
        current = current->next;
    }
    printf("NULL\n");
}


#endif