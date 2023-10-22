#pragma once

#include <stdio.h>

#include "defs.h"

void print_file(FILE* f);
void write_file_bytes_to_buffer(FILE* file, u64 &num_bytes, u8* buffer);
