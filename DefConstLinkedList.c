#include "DefConstLinkedList.h"
#include "ScriptConstants.h"
#include "StringHelper.h"

#include <stdlib.h>
#include <string.h>


def_const_node *createNode()
{
    def_const_node *p_node = (def_const_node*)malloc(sizeof(def_const_node));
    p_node->p_identifier = (char*)malloc(max_param_size);
    p_node->p_next = NULL;

    return p_node;
}

void push_back(def_const_node **pp_head, def_const_node *p_new_node)
{
    if (find(*pp_head, p_new_node->p_identifier) != NULL)
    {
        fprintf(stderr, "insertNode - Illegal operation. \
            Identifier \"%s\" is already used.\n", p_new_node->p_identifier);
        exit(1);
    }

    if (*pp_head == NULL)
    {
        *pp_head = p_new_node;
    }
    else 
    {
        while ((*pp_head)->p_next != NULL)
            pp_head = &(*pp_head)->p_next;
            
        (*pp_head)->p_next = p_new_node;
    }
}

def_const_node *find(def_const_node *p_head, const char *p_identifier)
{
    while (p_head != NULL)
    {
        if (stricmp(p_head->p_identifier, p_identifier) == 0)
            return p_head;

        p_head = p_head->p_next;
    }

    return NULL;
}

void print(FILE *stream, const def_const_node *p_head)
{
    int i = 0;

    while (p_head != NULL)
    {
        fprintf(stream, "\t%5d. Identifier: \"%s\", Value: \"%d\"\n", 
            i, p_head->p_identifier, p_head->value);

        p_head = p_head->p_next;
        ++i;
    }
}
