/*
 * STN-LABZ
 * Module ABI 1.3
 *
 * main.c
 *
 * Minimal standalone ABI validation executable.
 */

#include "abi.h"


int
main(void)
{
    return
        stnlabz_module_abi_self_test();
}
