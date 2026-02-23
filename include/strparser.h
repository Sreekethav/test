#ifndef STRPARSER_H
#define STRPARSER_H

#include <stdint.h>
#include <stddef.h>

// Token structure for split operations
typedef struct {
    const char *start;
    size_t length;
} strparser_token_t;

// Key-value pair structure
typedef struct {
    const char *key_start;
    size_t key_length;
    const char *value_start;
    size_t value_length;
} strparser_kv_t;

// Integer list structure
typedef struct {
    int *values;
    size_t count;
    size_t capacity;
} strparser_int_list_t;

// Function declarations

// Check if character is a hex digit
int is_hex_digit(char c);

// Convert hex character to integer value
int hex_to_int(char c);

// Parse a comma-separated list of integers
// Returns 0 on success, -1 on error
int strparser_parse_int_list(const char *input, size_t input_len, strparser_int_list_t *result);

// Free memory allocated for integer list
void strparser_free_int_list(strparser_int_list_t *list);

// Split string by delimiter
// Returns number of tokens found, or -1 on error
int strparser_split(const char *input, size_t input_len, char delimiter,
                    strparser_token_t *tokens, size_t max_tokens, size_t *token_count);

// Parse a single key=value pair
// Returns 0 on success, -1 on error
int strparser_parse_kv(const char *input, size_t input_len, strparser_kv_t *result);

// Parse a list of key=value pairs separated by delimiter
// Returns 0 on success, -1 on error
int strparser_parse_kv_list(const char *input, size_t input_len, char delimiter,
                            strparser_kv_t *kvs, size_t max_kvs, size_t *kv_count);

// URL decode a string
// Returns 0 on success, -1 on error
int strparser_url_decode(const char *input, size_t input_len,
                         char *output, size_t output_size, size_t *output_len);

// Hex decode a string
// Returns 0 on success, -1 on error
int strparser_hex_decode(const char *input, size_t input_len,
                         uint8_t *output, size_t output_size, size_t *output_len);

#endif // STRPARSER_H
