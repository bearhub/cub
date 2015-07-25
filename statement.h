#ifndef STATEMENT_H
#define STATEMENT_H

#include "expression.h"

struct block_statement;

typedef enum {
  S_BLOCK,
  S_BREAK,
  S_CLASS,
  S_CONTINUE,
  // S_DECLARE, // individualized, without initialization, for generation
  S_DEFINE, // from parser
  S_DO_WHILE,
  S_EXPRESSION,
  S_FUNCTION,
  S_IF,
  S_RETURN,
  S_WHILE
} statement_type;

// also needs generation data
typedef struct statement {
  statement_type type;
  struct statement *next, *parent;
} statement;

typedef struct /*extends statement*/ {
  statement_type type;
  statement *next, *parent;
  char *label;
  expression *condition;
  struct block_statement *body;

  // code generation
  size_t block_head, block_tail;
  bool tail_used;
} loop_statement;

// MUST be copied due to block<->ast incompatibilities
typedef struct symbol_entry {
  char *symbol_name;
  union {
    type *type;
    class *classtype;
    function *function;
  };
  struct symbol_entry *next;

  // code generation
  bool exists;
  size_t instruction;
} symbol_entry;

typedef struct block_statement /*extends statement*/ {
  statement_type type;
  statement *next, *parent;
  statement *body;
  function *fn_parent;
  symbol_entry *class_head, *class_tail, *function_head, *function_tail,
    *type_head, *type_tail, *variable_head, *variable_tail;
} block_statement;

typedef struct /*extends statement*/ {
  statement_type type;
  statement *next, *parent;
  char *symbol_name;
  class *classtype;
} class_statement;

// break/continue
typedef struct /*extends statement*/ {
  statement_type type;
  statement *next, *parent;
  // TODO: switch statements mean target should be a statement
  union {
    char *label;
    loop_statement *target;
  };
} control_statement;

// typedef struct /*extends statement*/ {
//   statement_type type;
//   statement *next, *parent;
//   type *symbol_type;
//   char *symbol_name;
// } declare_statement;

typedef struct define_clause {
  char *symbol_name;
  expression *value;
  struct define_clause *next;
} define_clause;

typedef struct /*extends statement*/ {
  statement_type type;
  statement *next, *parent;
  type *symbol_type;
  define_clause *clause;
} define_statement;

typedef struct /*extends statement*/ {
  statement_type type;
  statement *next, *parent;
  expression *value;
} expression_statement;

typedef struct /*extends statement*/ {
  statement_type type;
  statement *next, *parent;
  char *symbol_name;
  function *function;
} function_statement;

typedef struct /*extends statement*/ {
  statement_type type;
  statement *next, *parent;
  expression *condition;
  block_statement *first, *second;
} if_statement;

typedef struct /*extends statement*/ {
  statement_type type;
  statement *next, *parent;
  expression *value;
  function *target;
} return_statement;

statement *s_block(statement *body);
statement *s_break(char *label);
statement *s_class(class *classtype);
statement *s_continue(char *label);
statement *s_declare(type *type, char *name);
statement *s_define(type *type, define_clause *clause);
statement *s_expression(expression *value);
statement *s_function(function *function);
statement *s_if(expression *condition, block_statement *first,
  block_statement *second);
statement *s_loop(statement_type type, expression *condition,
  block_statement *body);
statement *s_return(expression *value);

#endif
