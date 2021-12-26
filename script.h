#ifndef SCRIPT_H
#define SCRIPT_H

#include "script_linked_list.h"
#include "def_const_linked_list.h"

// Forward declarations
struct script_node;
struct def_const_node;


typedef struct _script
{
    // Script file
    unsigned script_size;
    script_node *p_script_head;

    // Script line
    script_node *p_curr_script_line;
    unsigned curr_script_line_char;

    // DefConsts
    def_const_node *p_def_const_head;
} script;

void load_script(script **pp_script, char *p_script_filepath);
void unload_script(script *p_script);

void read_str(script *p_script, char *p_str_dest, const char *const delims);
int skip_trailing_space(script *p_script);

void get_command(script *p_script, char *p_str_command);
void get_string_param(script *p_script, char *p_param);
int get_int_param(script *p_script);
float get_float_param(script *p_script);
void get_def_const_identifier(script *p_script, char *p_param);

int run_script(script *p_script);

#endif // !SCRIPT_H