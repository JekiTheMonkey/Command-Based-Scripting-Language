#ifndef SCRIPT_LINKED_LIST_H
#define SCRIPT_LINKED_LIST_H


typedef struct _script_node
{
    char *p_line;

    struct _script_node *p_next;
} script_node;

script_node *create_script_node();
void delete_script_node(script_node *p_node);
void push_script_back(script_node **pp_cur, script_node *p_new_node);
script_node *find_script(script_node *p_head, const char *p_line);
void remove_script(script_node *p_head, script_node *p_node);
void remove_matching_script(script_node **p_head, const char *p_command);
void remove_matching_script_in_range(script_node **p_from, script_node *p_to, const char *p_command);
void clear_script(script_node *p_head);


#endif // !SCRIPT_LINKED_LIST_H