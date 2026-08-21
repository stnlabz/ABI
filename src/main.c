/*
 * STN-LABZ
 * Module ABI 1.4
 *
 * main.c
 *
 * Minimal standalone lifecycle validation executable.
 */

#include "abi.h"


int
main(void)
{
    return
        stnlabz_module_abi_self_test();
}
