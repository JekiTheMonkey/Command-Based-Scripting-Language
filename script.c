#include "def_const_linked_list.h"
#include "script.h"
#include "script_commands.h"
#include "script_constants.h"
#include "script_linked_list.h"
#include "string_helper.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


void load_script(script **pp_script, char *p_script_filepath)
{
    if (*pp_script == NULL)
    {
        fprintf(stderr, "load_script - Script pointer is NULL.");
        exit(1);
    }

    // Create a file pointer for the script
    FILE *p_script_file;

    // Open the script for reading in binary mode; Control fopen result
    if (!(p_script_file = fopen(p_script_filepath, "rb")))
    {
        fprintf(stderr, "load_script - File \"%s\" I/O error.\n", p_script_filepath);
        exit(1);
    }

    // Count number of commands
    while (!feof(p_script_file))
        if (fgetc(p_script_file) == '\n')
            ++((*pp_script)->script_size);
    ++((*pp_script)->script_size);

    // Close the script
    fclose(p_script_file);

    // Open the script for reading; Control fopen result
    if (!(p_script_file = fopen(p_script_filepath, "r")))
    {
        fprintf(stderr, "load_script - File \"%s\" I/O error.\n", p_script_filepath);
        exit(1);
    }

    // Read each script's line
    for (int i = 0; i < (*pp_script)->script_size; ++i)
    {
        // Create new node
        script_node *p_tmp = create_script_node();

        // Read the line in the allocated memory
        fgets(p_tmp->p_line, max_script_line_size, p_script_file);

        // Add new node to the list
        push_script_back(&(*pp_script)->p_script_head, p_tmp);
    }

    // Close the script
    fclose(p_script_file);

    (*pp_script)->p_curr_script_line = (*pp_script)->p_script_head;
}

void unload_script(script *p_script)
{
    // Return if no script is loaded
    if (!p_script)
        return;

    // Unload the whole script 
    clear_script(p_script->p_script_head);
}

void read_str(script *p_script, char *p_str_dest, const char *const delims)
{
    // Keep track of the parameter's length
    unsigned len = 0u;

    // Create a space for the current and previous characters
    char curr_char;
    char prev_char;

    // Is the current double quote '\"'
    int is_special_dquote = 0;

    while (p_script->curr_script_line_char < 
        (unsigned)strlen(p_script->p_curr_script_line->p_line))
    {
        // Read the next character from the line
        curr_char = p_script->p_curr_script_line->p_line[
            p_script->curr_script_line_char];

        // Read the previous character from the line if there is any
        if (p_script->curr_script_line_char > 0)
        {
            prev_char = p_script->p_curr_script_line->p_line[
                p_script->curr_script_line_char - 1];
                
            is_special_dquote = (prev_char == '\\' && curr_char == '"');
        }

        // Control if the reading is complete
        if (is_present(delims, curr_char) && !is_special_dquote)
            break;

        // Decrease len to make the special double quote overwrite the \ sign
        if (is_special_dquote)
            --len;

        // Append the character to the destination
        p_str_dest[len] = curr_char;

        // Increment the length of the destination
        ++len;

        // Move to the next character in the current line
        ++(p_script->curr_script_line_char);
    }

    // Append the trailing zero
    p_str_dest[len] = '\0';
}

int skip_trailing_space(script *p_script)
{
    // Create a space for the current character
    char curr_char;

    // Create a space to keep track of skipped spaced
    int skipped_spaces = 0;

    // Read the line until it's not complited
    while (p_script->curr_script_line_char < 
        (unsigned)strlen(p_script->p_curr_script_line->p_line))
    {
        // Read the next character from the line
        curr_char = p_script->p_curr_script_line->p_line[
            p_script->curr_script_line_char];

        // Control if the read character is the space to skip
        if (curr_char == ' ' || curr_char == '\n')
        {
            ++(p_script->curr_script_line_char);
            ++skipped_spaces;
        }

        // Stop increasing the character if there is no trailing space left
        else
            break;
    }

    return skipped_spaces;
}

void get_command(script *p_script, char *p_str_command)
{
    // Create delimiters
    char delims[] = { ' ', '\n' };

    // Read the string
    read_str(p_script, p_str_command, delims);

    // Skip trailing space
    skip_trailing_space(p_script);

    // Convert it all to uppercase
    strupr(p_str_command);
}

void get_string_param(script *p_script, char *p_param)
{    
    // Move past the opening double quote
    ++(p_script->curr_script_line_char);

    // Create delimiters
    char delims[] = { '"' };

    // Skip the trailing space 
    skip_trailing_space(p_script);

    // Read the string
    read_str(p_script, p_param, delims);

    // Skip double quote
    ++(p_script->curr_script_line_char);
}

int get_int_param(script *p_script)
{
    // Create some space for the integer's string representation
    char p_param[max_param_size + 1];
    
    // Create delimiters
    char delims[] = { ' ', '\n' };

    // Skip trailing space
    skip_trailing_space(p_script);
    
    // Read the string
    read_str(p_script, p_param, delims);

    // Create a space to store final integer value
    int int_value;

    // Check Boolean types
    if (stricmp(p_param, "true") == 0)
    {
        int_value = 1;
    }
    else if (stricmp(p_param, "false") == 0)
    {
        int_value = 0;
    }

    // Check DefConst
    else if (isalpha(p_param[0]) || p_param[0] == '_')
    {
        // Check read string in DefConst list
        def_const_node *p_found = find_def_const(
            p_script->p_def_const_head, p_param);

        // Control if read identifier exists
        if (p_found == NULL)
        {
            fprintf(stderr, "get_int_param - Identifier \"%s\" \
                is unknown.\n", p_param);
            exit(1);
        }
        
        int_value = p_found->value;
    }

    // Convert the string to an integer
    else
    {
        int_value = atoi(p_param);
    }

    // Return the integer value
    return int_value;
}

float get_float_param(script *p_script)
{
    // Create some space for the float's string representation
    char p_param[max_param_size + 1];
    
    // Create delimiters
    char delims[] = { ' ', '\n' };

    // Skip trailing space
    skip_trailing_space(p_script);
    
    // Read the string
    read_str(p_script, p_param, delims);

    // Create a space to store final float value
    float float_value;

    // Check Boolean types
    if (stricmp(p_param, "true") == 0)
    {
        float_value = 1;
    }
    else if (stricmp(p_param, "false") == 0)
    {
        float_value = 0;
    }

    // Convert the string to an float
    else
    {
        float_value = atof(p_param);
    }

    // Return the float value
    return float_value;
}

void get_def_const_identifier(script *p_script, char *p_param)
{
    // Create delimeters
    char delims[] = { ' ', '\n' };

    // Skip trailing space
    skip_trailing_space(p_script);

    // Get the DefConst's identifier
    read_str(p_script, p_param, delims);

    // Control if the identifier is valid
    if (!isalpha(p_param[0]) && p_param[0] != '_')
    {
        fprintf(stderr, "get_def_const_param - Illegal operation. \
            Identifier \"%s\" is invalid.\n", p_param);
        exit(1);
    }
}

int run_script(script *p_script)
{
    // Allocate strings for holding source substrings
    char p_str_command[max_command_size + 1];
    char p_str_string_param[max_param_size + 1];

    // Execute one line of code and then return control
    do
    {
        // Send a signal if the script is finished
        if (p_script->p_curr_script_line == NULL)
            return 0;

        // Reset the current character
        p_script->curr_script_line_char = 0u;

        // Skip empty lines and lines which are comments
        if (strlen(p_script->p_curr_script_line->p_line) == 
            skip_trailing_space(p_script)
        ||  (p_script->p_curr_script_line->p_line[0] == '/'
            &&  p_script->p_curr_script_line->p_line[1] == '/'))
        {
            // Move the index pointer to the next line
            p_script->p_curr_script_line = 
                p_script->p_curr_script_line->p_next;
            continue;
        }

        // Read the command
        get_command(p_script, p_str_command);

        // PrintString
        if (stricmp(p_str_command, command_printstring) == 0)
        {
            // Get the string
            get_string_param(p_script, p_str_string_param);

            // Print the string
            printf("\t%s\n", p_str_string_param);
        }

        // PrintStringLoop
        else if (stricmp(p_str_command, command_printstringloop) == 0)
        {
            // Get the string 
            get_string_param(p_script, p_str_string_param);

            // Get the loop count
            int loop_count = get_int_param(p_script);

            for (int i = 0; i < loop_count; ++i)
                printf("\t%d: %s\n", i, p_str_string_param);
        }

        // PrintInteger
        else if (stricmp(p_str_command, command_printinteger) == 0)
        {
            // Get the integer parameter
            int int_value = get_int_param(p_script);

            // Print the integer
            printf("\t%d\n", int_value);
        }

        // PrintFloat
        else if (stricmp(p_str_command, command_printfloat) == 0)
        {
            // Get the float parameter
            float float_value = get_float_param(p_script);

            // Print the float
            printf("\t%f\n", float_value);
        }

        // PrintBool
        else if (stricmp(p_str_command, command_printbool) == 0)
        {
            // Get the bool parameter
            int bool_value = get_int_param(p_script);

            // Print the bool
            printf("\t%s\n", bool_value ? "TRUE" : "FALSE");
        }

        // NewLine
        else if (stricmp(p_str_command, command_newline) == 0)
        {
            // Print a newline
            printf("\n");
        }

        // DefConst
        else if (stricmp(p_str_command, command_defconst) == 0)
        {
            // Allocare memory for new DefConst node
            def_const_node *p_node = create_def_const_node();

            // Get the DefConst's identifier
            get_def_const_identifier(p_script, p_node->p_identifier);

            // Get the DefConst's value
            p_node->value = get_int_param(p_script);

            // Push the DefConst to the list
            push_def_const_back(&p_script->p_def_const_head, p_node);

            printf("New DefConst defined: %s %d\n", p_node->p_identifier, p_node->value);
        }

        // PrintDefConsts
        else if (stricmp(p_str_command, command_printdefconsts) == 0)
        {
            // Define the output stream
            FILE *stream = stdout;

            // Print the list
            fprintf(stream, "\tExisting DefConsts:\n");
            print_def_const(stream, p_script->p_def_const_head);
        }

        // Repeat
        else if (stricmp(p_str_command, command_repeat) == 0)
        {
            // Remove all used DefConsts; Change head if it changed
            remove_matching_script_in_range(&p_script->p_script_head, 
                p_script->p_curr_script_line, "DefConst");

            // Place index pointer to the start of the script
            p_script->p_curr_script_line = p_script->p_script_head;
        }

        // Anything else is invalid
        else
        {
            printf("\tError: \"%s\" is an invalid command.\n", p_str_command);
            break;
        }

        // Move the index pointer to the next line
        p_script->p_curr_script_line = 
            p_script->p_curr_script_line->p_next;
    } while (0);
}