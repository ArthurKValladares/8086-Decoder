#include <cassert>
#include <fstream>
#include <stdlib.h>

#include "defs.h"
#include "8086.hpp"
#include "8086_decode.h"
#include "8086_text.h"
#include "file.hpp"

int main()
{
    FILE* in_file;
    FILE* dest_file;
    // NOTE: Clearly not the most memory-safe way to append the full path,
    // but it's fine for this constrained example
    char command_char_buffer[128];
    const char* dest_compiled_file = "dest_scratch";
#ifdef __APPLE__
    const char* file_path = "./data/";
    const char* remove_command = "rm ";
#else
    const char* file_path = "../data/";
    const char* remove_command = "del ";
#endif
    const char *files[] = {
        "listing_0037_single_register_mov",
        "listing_0038_many_register_mov",
        "listing_0039_more_movs",
        "listing_0040_challenge_movs",
        "listing_0041_add_sub_cmp_jnz",
        "listing_0042_completionist_decode"
    };

    for (const auto &file_name : files)
    {

        printf("========\nDecoding: %s\n========\n", file_name);

        // Compile file
        strcpy(command_char_buffer, "nasm ");
        strcat(command_char_buffer, file_path);
        strcat(command_char_buffer, file_name);
        strcat(command_char_buffer, ".asm -o ");
        strcat(command_char_buffer, dest_compiled_file);
        system(command_char_buffer);

        // Read compiled file bytes to buffer
        in_file = fopen(dest_compiled_file, "r");
        u64 num_bytes;
        write_file_bytes_to_buffer(in_file, num_bytes, nullptr);
        u8 *bytes = new u8[num_bytes];
        write_file_bytes_to_buffer(in_file, num_bytes, bytes);
        fclose(in_file);

        // Decode bytes to scratch destination file
        const char* scratch_file_path = "scratch.asm";
        dest_file = fopen(scratch_file_path, "w");
        if (!dest_file) {
            printf("Failed to open scratch file at: %s", scratch_file_path);
            return 1;
        }
        fclose(dest_file);
        dest_file = fopen(scratch_file_path, "a");
        if (!dest_file) {
            printf("Failed to re-open scratch file at: %s", scratch_file_path);
            return 1;
        }
        fprintf(dest_file, "bits 16\n");
        dis_asm_8086(bytes, num_bytes, dest_file);
        fclose(dest_file);
        
        in_file = fopen("scratch.asm", "r");
        print_file(in_file);
        fclose(in_file);
        
        // Compile file and save bytes to buffer
        system("nasm scratch.asm");
        in_file = fopen("scratch", "rb");
        u64 filelen;
        write_file_bytes_to_buffer(in_file, filelen, nullptr);
        u8 *buffer = new u8[filelen];
        write_file_bytes_to_buffer(in_file, filelen, buffer);
        fclose(in_file);
        
        // Check that both files are the same
        assert(num_bytes  == filelen);
        assert(memcmp(bytes, buffer, num_bytes) == 0);
        
        // Cleanup
        strcpy(command_char_buffer, remove_command);
        strcat(command_char_buffer, dest_compiled_file);
        system(command_char_buffer);

        strcpy(command_char_buffer, remove_command);
        strcat(command_char_buffer, "scratch");
        system(command_char_buffer);
        
        strcpy(command_char_buffer, remove_command);
        strcat(command_char_buffer, "scratch.asm");
        system(command_char_buffer);

        delete[] bytes;
        delete[] buffer;
    }

    return 0;
}
