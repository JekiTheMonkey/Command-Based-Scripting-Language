#include "script.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    const unsigned scripts = 1u;
    script **pp_scripts = (script**)malloc(sizeof(script*) * scripts);

    for (unsigned i = 0; i < scripts; ++i)
    {
        pp_scripts[i] = (script*)malloc(sizeof(script));
        load_script(&pp_scripts[i], "Scripts/Script.jcbs");
    }

    while (1)
    {
        for (unsigned i = 0; i < scripts; ++i)
        {
            run_script(pp_scripts[i]);
        }
    }

    for (unsigned i = 0; i < scripts; ++i)
    {
        unload_script(pp_scripts[i]);
    }

    return 0;
}