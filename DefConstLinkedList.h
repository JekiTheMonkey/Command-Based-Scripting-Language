#ifndef DEF_CONST_LINKED_LIST_H
#define DEF_CONST_LINKED_LIST_H

#include <stdio.h>


typedef struct _DefConstNode
{
    char *p_identifier;
    int value;

    struct _DefConstNode *p_next;
} def_const_node;

def_const_node *createNode();
void push_back(def_const_node **pp_head, def_const_node *p_new_node);
def_const_node *find(def_const_node *p_head, const char *p_identifier);
void print(FILE *stream, const def_const_node *p_head);


#endif // !DEF_CONST_LINKED_LIST_H