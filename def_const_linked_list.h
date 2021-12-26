#ifndef DEF_CONST_LINKED_LIST_H
#define DEF_CONST_LINKED_LIST_H

#include <stdio.h>


typedef struct _def_const_node
{
    char *p_identifier;
    int value;

    struct _def_const_node *p_next;
} def_const_node;

def_const_node *create_def_const_node();
void push_def_const_back(def_const_node **pp_head, def_const_node *p_new_node);
def_const_node *find_def_const(def_const_node *p_head, const char *p_identifier);
void print_def_const(FILE *stream, const def_const_node *p_head);


#endif // !DEF_CONST_LINKED_LIST_H