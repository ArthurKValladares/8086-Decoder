#include "file.hpp"

void print_file(FILE* f) {
    fseek(f, 0, SEEK_SET);
    char c;
    c = fgetc(f);
    while (c != EOF)
    {
        printf ("%c", c);
        c = fgetc(f);
    }
    fclose(f);
}

void write_file_bytes_to_buffer(FILE* file, u64 &num_bytes, u8* buffer) {
    if (buffer == nullptr) {
        fseek(file, 0, SEEK_END);
        num_bytes = ftell(file);
        rewind(file);
    } else {
        fread(buffer, num_bytes, 1, file);
    }
}
