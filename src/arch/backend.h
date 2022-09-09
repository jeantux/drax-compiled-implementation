/* Drax Lang - 2022 */

#ifndef __DBACKEND
#define __DBACKEND

#include "../dlowcode.h"

int dx_init_data_section();

int dx_init_text_section();

int dx_init_start();

int dx_init_exit();

int write_lines_to_buffer(dlines_cmd* v);

#endif