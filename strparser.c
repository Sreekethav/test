#include "strparser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Check if character is a hex digit
int is_hex_digit(char c) {
    return (c >= '0' && c <= '9') ||
           (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

// Convert hex character to integer value
int hex_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// Parse a comma-separated list of integers
int strparser_parse_int_list(const char *input, size_t input_len, strparser_int_list_t *result) {
    if (!input || !result) return -1;

    result->values = NULL;
    result->count = 0;
    result->capacity = 0;

    if (input_len == 0) return 0;

    size_t i = 0;
    while (i < input_len) {
        // Skip whitespace
        while (i < input_len && isspace((unsigned char)input[i])) i++;
        if (i >= input_len) break;

        // Check for negative sign
        int negative = 0;
        if (input[i] == '-') {
            negative = 1;
            i++;
        }

        // Parse number
        if (i >= input_len || !isdigit((unsigned char)input[i])) {
            // Invalid format
            free(result->values);
            result->values = NULL;
            result->count = 0;
            return -1;
        }

        int value = 0;
        while (i < input_len && isdigit((unsigned char)input[i])) {
            value = value * 10 + (input[i] - '0');
            i++;
        }
        if (negative) value = -value;

        // Add to result
        if (result->count >= result->capacity) {
            size_t new_capacity = result->capacity == 0 ? 8 : result->capacity * 2;
            int *new_values = realloc(result->values, new_capacity * sizeof(int));
            if (!new_values) {
                free(result->values);
                result->values = NULL;
                result->count = 0;
                return -1;
            }
            result->values = new_values;
            result->capacity = new_capacity;
        }
        result->values[result->count++] = value;

        // Skip whitespace
        while (i < input_len && isspace((unsigned char)input[i])) i++;

        // Expect comma or end
        if (i < input_len) {
            if (input[i] == ',') {
                i++;
            } else {
                // Invalid character
                free(result->values);
                result->values = NULL;
                result->count = 0;
                return -1;
            }
        }
    }

    return 0;
}

// Free memory allocated for integer list
void strparser_free_int_list(strparser_int_list_t *list) {
    if (list && list->values) {
        free(list->values);
        list->values = NULL;
        list->count = 0;
        list->capacity = 0;
    }
}

// Split string by delimiter
int strparser_split(const char *input, size_t input_len, char delimiter,
                    strparser_token_t *tokens, size_t max_tokens, size_t *token_count) {
    if (!input || !tokens || !token_count || max_tokens == 0) return -1;

    *token_count = 0;

    if (input_len == 0) {
        return 0;
    }

    size_t start = 0;
    for (size_t i = 0; i <= input_len; i++) {
        if (i == input_len || input[i] == delimiter) {
            if (*token_count >= max_tokens) {
                return -1; // Buffer too small
            }
            tokens[*token_count].start = input + start;
            tokens[*token_count].length = i - start;
            (*token_count)++;
            start = i + 1;
        }
    }

    return 0;
}

// Parse a single key=value pair
int strparser_parse_kv(const char *input, size_t input_len, strparser_kv_t *result) {
    if (!input || !result) return -1;

    result->key_start = NULL;
    result->key_length = 0;
    result->value_start = NULL;
    result->value_length = 0;

    if (input_len == 0) return -1;

    // Find the '=' character
    size_t eq_pos = 0;
    int found = 0;
    for (size_t i = 0; i < input_len; i++) {
        if (input[i] == '=') {
            eq_pos = i;
            found = 1;
            break;
        }
    }

    if (!found) return -1;

    // Extract key (trim whitespace)
    size_t key_start = 0;
    size_t key_end = eq_pos;
    while (key_start < key_end && isspace((unsigned char)input[key_start])) key_start++;
    while (key_end > key_start && isspace((unsigned char)input[key_end - 1])) key_end--;

    // Extract value (trim whitespace)
    size_t value_start = eq_pos + 1;
    size_t value_end = input_len;
    while (value_start < value_end && isspace((unsigned char)input[value_start])) value_start++;
    while (value_end > value_start && isspace((unsigned char)input[value_end - 1])) value_end--;

    result->key_start = input + key_start;
    result->key_length = key_end - key_start;
    result->value_start = input + value_start;
    result->value_length = value_end - value_start;

    return 0;
}

// Parse a list of key=value pairs separated by delimiter
int strparser_parse_kv_list(const char *input, size_t input_len, char delimiter,
                            strparser_kv_t *kvs, size_t max_kvs, size_t *kv_count) {
    if (!input || !kvs || !kv_count || max_kvs == 0) return -1;

    *kv_count = 0;

    if (input_len == 0) return 0;

    // First split by delimiter
    strparser_token_t *tokens = malloc(max_kvs * sizeof(strparser_token_t));
    if (!tokens) return -1;

    size_t token_count;
    if (strparser_split(input, input_len, delimiter, tokens, max_kvs, &token_count) != 0) {
        free(tokens);
        return -1;
    }

    // Parse each token as key=value
    for (size_t i = 0; i < token_count; i++) {
        if (*kv_count >= max_kvs) {
            free(tokens);
            return -1;
        }
        if (strparser_parse_kv(tokens[i].start, tokens[i].length, &kvs[*kv_count]) == 0) {
            (*kv_count)++;
        }
    }

    free(tokens);
    return 0;
}

// URL decode a string
int strparser_url_decode(const char *input, size_t input_len,
                         char *output, size_t output_size, size_t *output_len) {
    if (!input || !output || !output_len || output_size == 0) return -1;

    *output_len = 0;

    for (size_t i = 0; i < input_len; i++) {
        if (*output_len >= output_size - 1) return -1; // Buffer too small

        if (input[i] == '%' && i + 2 < input_len &&
            is_hex_digit(input[i + 1]) && is_hex_digit(input[i + 2])) {
            // Decode hex sequence
            int high = hex_to_int(input[i + 1]);
            int low = hex_to_int(input[i + 2]);
            output[*output_len] = (char)((high << 4) | low);
            (*output_len)++;
            i += 2;
        } else if (input[i] == '+') {
            // '+' represents space
            output[*output_len] = ' ';
            (*output_len)++;
        } else {
            output[*output_len] = input[i];
            (*output_len)++;
        }
    }

    output[*output_len] = '\0';
    return 0;
}

// Hex decode a string
int strparser_hex_decode(const char *input, size_t input_len,
                         uint8_t *output, size_t output_size, size_t *output_len) {
    if (!input || !output || !output_len) return -1;

    /*--------------------*/
    //bug
    char str[4] = "abc";
    char buf[12];
    strcpy(buf, "Buffer-Overflow");
    /*--------------------*/
    *output_len = 0;

    // Input length must be even for valid hex
    if (input_len % 2 != 0) return -1;

    size_t expected_output_len = input_len / 2;
    if (expected_output_len > output_size) return -1;

    for (size_t i = 0; i < input_len; i += 2) {
        if (!is_hex_digit(input[i]) || !is_hex_digit(input[i + 1])) {
            return -1;
        }
        int high = hex_to_int(input[i]);
        int low = hex_to_int(input[i + 1]);
        output[*output_len] = (uint8_t)((high << 4) | low);
        (*output_len)++;
    }

    return 0;
}
