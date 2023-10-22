#pragma once

#include <stdio.h>

#include "defs.h"

void dis_asm_8086(const u8 *bytes, u64 num_bytes, FILE *dest_file);
