#ifndef ANALYZE_H
#define ANALYZE_H

#include "statement.h"

#define ST_CLASS 1
#define ST_FUNCTION 2
#define ST_TYPE 4
#define ST_VARIABLE 8

#define fail(err, place, ...) do {\
  fprintf(stderr, err " at %zu:%zu\n", ##__VA_ARGS__, place->line, place->offset);\
  exit(1);\
} while (0);

expression *bool_cast(expression *value);
void explicit_cast(expression *value);
expression *implicit_cast(expression *value, type *expected);
expression *numeric_promotion(expression*, bool allow_floats);
type_type binary_numeric_conversion(expression *value);
type_type binary_numeric_promotion(expression *value, bool allow_floats);
void ternary_numeric_promotion(expression *value);
type *resolve_type(block_statement *block, type *type);
void assert_condition(expression *value);

block_statement *parent_scope(block_statement *block, bool *escape);
loop_statement *get_label(control_statement*);
symbol_entry *get_entry(symbol_entry*, const char*);
symbol_entry *new_symbol_entry(char*);
symbol_entry *add_symbol(block_statement*, uint8_t, char*);
symbol_entry *get_symbol(block_statement*, char*, uint8_t *symbol_type);

void analyze(block_statement*);

#endif
