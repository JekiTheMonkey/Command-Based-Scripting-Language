#include "script.h"
#include "script_linked_list.h"
#include "script_constants.h"
#include "string_helper.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


script_node *create_script_node()
{
    script_node *p_node = (script_node*)malloc(sizeof(script_node));
    p_node->p_line = (char*)malloc(max_script_line_size);
    p_node->p_next = NULL;

    return p_node;
}

void delete_script_node(script_node *p_node)
{
    free(p_node->p_line);
    free(p_node);
}

void push_script_back(script_node **pp_head, script_node *p_new_node)
{
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

script_node *find_script(script_node *p_head, const char *p_line)
{
    while (p_head != NULL)
    {
        if (stricmp(p_head->p_line, p_line) == 0)
            return p_head;

        p_head = p_head->p_next;
    }

    return NULL;
}

void remove_script(script_node *p_head, script_node *p_node)
{
    while (p_head != NULL)
    {
        if (p_head->p_next == p_node)
        {
            p_head->p_next = p_node->p_next;
            delete_script_node(p_node);
            return;
        }

        p_head = p_head->p_next;
    }
}

void remove_matching_script(script_node **pp_cur, const char *p_command)
{
    // Return if current element is null
    if (*pp_cur == NULL)
        return;

    // Call recursively
    remove_matching_script(&(*pp_cur)->p_next, p_command);

    // Create a local script to use get_command; Create a buffer to store the line's command
    script p_script;
    char p_buf[max_command_size];

    // Reset the local script
    p_script.p_script_head = *pp_cur;
    p_script.p_curr_script_line = *pp_cur;
    p_script.curr_script_line_char = 0u;

    // Get current line's command
    get_command(&p_script, p_buf);

    // Compare it with the requested one
    if (stricmp(p_buf, p_command) == 0)
    {
        // Save the node to delete, move on next node, delete the node
        script_node *p_tmp = *pp_cur;
        *pp_cur = (*pp_cur)->p_next;
        delete_script_node(p_tmp);
    }
}


void remove_matching_script_in_range(script_node **pp_from, script_node *p_to, const char *p_command)
{
    // Return if current element is null
    if (*pp_from == p_to)
        return;

    // Call recursively
    remove_matching_script_in_range(&(*pp_from)->p_next, p_to, p_command);

    // Create a local script to use get_command; Create a buffer to store the line's command
    script p_script;
    char p_buf[max_command_size];

    // Reset the local script
    p_script.p_script_head = *pp_from;
    p_script.p_curr_script_line = *pp_from;
    p_script.curr_script_line_char = 0u;

    // Get current line's command
    get_command(&p_script, p_buf);

    // Compare it with the requested one
    if (stricmp(p_buf, p_command) == 0)
    {
        // Save the node to delete, move on next node, delete the node
        script_node *p_tmp = *pp_from;
        *pp_from = (*pp_from)->p_next;
        delete_script_node(p_tmp);
    }
}

void clear_script(script_node *p_head)
{
    if (p_head == NULL)
        return;
    
    clear_script(p_head->p_next);
    delete_script_node(p_head);
}
