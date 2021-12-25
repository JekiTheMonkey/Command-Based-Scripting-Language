#include "ScriptConstants.h"
#include "StringHelper.h"
#include "DefConstLinkedList.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>


// Script file
unsigned script_size;
char **pp_str_script;

// Script line
unsigned curr_script_line;
unsigned curr_script_line_char;

// DefConsts
def_const_node *p_def_const_head;

// Commands
char command_printstring[] = "PrintString";
char command_printstringloop[] = "PrintStringLoop";
char command_printinteger[] = "PrintInteger";
char command_printfloat[] = "PrintFloat";
char command_printbool[] = "PrintBool";
char command_newline[] = "Newline";
char command_defconst[] = "DefConst";
char command_printdefconsts[] = "PrintDefConsts";

int is_present(const char *p_str, char ch)
{
    char tmp;

    while ((tmp = *p_str) != '\0')
    {
        if (tmp == ch)
            return 1;
        ++p_str;
    }

    return 0;
}

void load_script(char *p_script_filepath)
{
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
            ++script_size;
    ++script_size;

    // Close the script
    fclose(p_script_file);

    // Open the script for reading; Control fopen result
    if (!(p_script_file = fopen(p_script_filepath, "r")))
    {
        fprintf(stderr, "load_script - File \"%s\" I/O error.\n", p_script_filepath);
        exit(1);
    }

    // Allocate memory for script_size lines
    pp_str_script = (char**)malloc(script_size * sizeof(char*));

    // Read each script's line
    for (int i = 0; i < script_size; ++i)
    {
        // Allocate memory for the line and a null terminator
        pp_str_script[i] = (char*)malloc(max_script_line_size + 1 * sizeof(char));

        // Read the line in the allocated array
        fgets(pp_str_script[i], max_script_line_size, p_script_file);
    }

    // Close the script
    fclose(p_script_file);
}

void unload_script()
{
    // Return if no script is loaded
    if (!pp_str_script)
        return;

    // Free each line of code individually
    for (int i = 0; i < script_size; ++i)
        free(pp_str_script[i]);

    // Free the script structure itself
    free(pp_str_script);
}

void read_str(char *p_str_dest, const char *const delims)
{
    // Keep track of the parameter's length
    unsigned len = 0u;

    // Create a space for the current and previous characters
    char curr_char;
    char prev_char;

    // Is the current double quote '\"'
    int is_special_dquote = 0;

    while (curr_script_line_char < 
        (unsigned)strlen(pp_str_script[curr_script_line]))
    {
        // Read the next character from the line
        curr_char = pp_str_script[curr_script_line][curr_script_line_char];

        // Read the previous character from the line if there is any
        if (curr_script_line_char > 0)
        {
            prev_char = pp_str_script[curr_script_line][curr_script_line_char - 1];
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
        ++curr_script_line_char;
    }

    // Append the trailing zero
    p_str_dest[len] = '\0';
}

int skip_trailing_space()
{
    // Create a space for the current character
    char curr_char;

    // Create a space to keep track of skipped spaced
    int skipped_spaces = 0;

    // Read the line until it's not complited
    while (curr_script_line_char < 
        (unsigned)strlen(pp_str_script[curr_script_line]))
    {
        // Read the next character from the line
        curr_char = pp_str_script[curr_script_line][curr_script_line_char];

        // Control if the read character is the space to skip
        if (curr_char == ' ' || curr_char == '\n')
        {
            ++curr_script_line_char;
            ++skipped_spaces;
        }

        // Stop increasing the character if there is no trailing space left
        else
            break;
    }

    return skipped_spaces;
}

void get_command(char *p_str_command)
{
    // Create delimiters
    char delims[] = { ' ', '\n' };

    // Read the string
    read_str(p_str_command, delims);

    // Skip trailing space
    skip_trailing_space();

    // Convert it all to uppercase
    strupr(p_str_command);
}

void get_string_param(char *p_param)
{    
    // Move past the opening double quote
    ++curr_script_line_char;

    // Create delimiters
    char delims[] = { '"' };

    // Skip the trailing space 
    skip_trailing_space();

    // Read the string
    read_str(p_param, delims);

    // Skip double quote
    ++curr_script_line_char;
}

int get_int_param()
{
    // Create some space for the integer's string representation
    char p_param[max_param_size + 1];
    
    // Create delimiters
    char delims[] = { ' ', '\n' };

    // Skip trailing space
    skip_trailing_space();
    
    // Read the string
    read_str(p_param, delims);

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
        def_const_node *p_found = find(p_def_const_head, p_param);

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

float get_float_param()
{
    // Create some space for the float's string representation
    char p_param[max_param_size + 1];
    
    // Create delimiters
    char delims[] = { ' ', '\n' };

    // Skip trailing space
    skip_trailing_space();
    
    // Read the string
    read_str(p_param, delims);

    // Create a space to store final float value
    float float_value;

    // Check Boolean types
    if (stricmp(p_param, "true") == 0)
        float_value = 1;
    else if (stricmp(p_param, "false") == 0)
        float_value = 0;
    // Convert the string to an float
    else
        float_value = atof(p_param);

    // Return the float value
    return float_value;
}

void get_def_const_identifier(char *p_param)
{
    // Create delimeters
    char delims[] = { ' ', '\n' };

    // Skip trailing space
    skip_trailing_space();

    // Get the DefConst's identifier
    read_str(p_param, delims);

    // Control if the identifier is valid
    if (!isalpha(p_param[0]) && p_param[0] != '_')
    {
        fprintf(stderr, "get_def_const_param - Illegal operation. \
            Identifier \"%s\" is invalid.\n", p_param);
        exit(1);
    }
}

void run_script()
{
    // Allocare strings for holding source substrings
    char p_str_command[max_command_size + 1];
    char p_str_string_param[max_param_size + 1];

    // Loop though each line of code and execute it
    for (curr_script_line = 0u; 
        curr_script_line < script_size; 
        ++curr_script_line)
    {
        // Reset the current character
        curr_script_line_char = 0u;

        // Skip empty lines and lines which are comments
        if (strlen(pp_str_script[curr_script_line]) == skip_trailing_space()
        ||  (pp_str_script[curr_script_line][0] == '/'
         &&  pp_str_script[curr_script_line][1] == '/'))
            continue;

        // Read the command
        get_command(p_str_command);

        // PrintString
        if (stricmp(p_str_command, command_printstring) == 0)
        {
            // Get the string
            get_string_param(p_str_string_param);

            // Print the string
            printf("\t%s\n", p_str_string_param);
        }

        // PrintStringLoop
        else if (stricmp(p_str_command, command_printstringloop) == 0)
        {
            // Get the string 
            get_string_param(p_str_string_param);

            // Get the loop count
            int loop_count = get_int_param();

            for (int i = 0; i < loop_count; ++i)
                printf("\t%d: %s\n", i, p_str_string_param);
        }

        // PrintInteger
        else if (stricmp(p_str_command, command_printinteger) == 0)
        {
            // Get the integer parameter
            int int_value = get_int_param();

            // Print the integer
            printf("\t%d\n", int_value);
        }

        // PrintFloat
        else if (stricmp(p_str_command, command_printfloat) == 0)
        {
            // Get the float parameter
            float float_value = get_float_param();

            // Print the float
            printf("\t%f\n", float_value);
        }

        // PrintBool
        else if (stricmp(p_str_command, command_printbool) == 0)
        {
            // Get the bool parameter
            int bool_value = get_int_param();

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
            def_const_node *p_node = createNode();

            // Get the DefConst's identifier
            get_def_const_identifier(p_node->p_identifier);

            // Get the DefConst's value
            p_node->value = get_int_param();

            // Push the DefConst to the list
            push_back(&p_def_const_head, p_node);
        }

        // PrintDefConsts
        else if (stricmp(p_str_command, command_printdefconsts) == 0)
        {
            // Define the output stream
            FILE *stream = stdout;

            // Print the list
            fprintf(stream, "\tExisting DefConsts:\n");
            print(stream, p_def_const_head);
        }

        // Anything else is invalid
        else
        {
            printf("\tError: \"%s\" is an invalid command.\n", p_str_command);
            break;
        }
    }
}

int main(int argc, char **argv)
{
    load_script("Scripts/Script.jcbs");
    run_script();
    unload_script();
    return 0;
}