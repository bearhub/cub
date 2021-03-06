#include <string.h>

#include "analyze.h"
#include "xalloc.h"

block_statement *parent_scope(block_statement *block, bool *escape) {
  statement *parent = block->parent;

  while (parent) {
    switch (parent->type) {
    case S_BLOCK:
      return (block_statement*) parent;
    case S_FUNCTION:
      if (escape) *escape = true;
      // fallthrough
    case S_DEFINE:
    case S_DO_WHILE:
    case S_EXPRESSION:
    case S_IF:
    case S_LET:
    case S_RETURN:
    case S_WHILE:
      break;
    // shouldn't ever be a parent
    case S_BREAK:
    case S_CONTINUE:
    case S_TYPEDEF:
    // shouldn't currently be a parent
    case S_CLASS:
      abort();
    }

    parent = parent->parent;
  }

  // dead end
  return NULL;
}

loop_statement *get_label(control_statement *control) {
  const char *label = control->label;

  for (statement *node = control->parent; node; node = node->parent) {
    switch (node->type) {
    case S_DO_WHILE:
    case S_WHILE: {
      loop_statement *loop = (loop_statement*) node;
      if (!label || (loop->label && strcmp(loop->label, label) == 0)) {
        return loop;
      }
      // fallthrough
    }
    case S_BLOCK:
    case S_IF:
      break;
    case S_FUNCTION:
      return NULL;
    // these shouldn't ever be a parent
    case S_BREAK:
    case S_CLASS:
    case S_CONTINUE:
    case S_DEFINE:
    case S_EXPRESSION:
    case S_LET:
    case S_RETURN:
    case S_TYPEDEF:
      abort();
    }
  }

  return NULL;
}

static symbol_entry **find_entry(symbol_entry **head,
    const char *symbol_name) {
  symbol_entry **entry = head;
  for (; *entry; entry = &(*entry)->next) {
    if (strcmp(symbol_name, (*entry)->symbol_name) == 0) {
      break;
    }
  }

  return entry;
}

symbol_entry *get_entry(symbol_entry *head, const char *symbol_name) {
  return *find_entry(&head, symbol_name);
}

symbol_entry *new_symbol_entry(char *symbol_name) {
  symbol_entry *entry = xmalloc(sizeof(*entry));
  entry->symbol_name = symbol_name;
  entry->next = NULL;
  return entry;
}

symbol_entry *add_symbol(block_statement *block, uint8_t type,
    char *symbol_name) {
  symbol_entry **entry, **tail;

  switch (type) {
  case ST_CLASS:
    entry = &block->class_head;
    tail = &block->class_tail;
    break;
  case ST_FUNCTION:
    entry = &block->function_head;
    tail = &block->function_tail;
    break;
  case ST_TYPE:
    entry = &block->type_head;
    tail = &block->type_tail;
    break;
  case ST_VARIABLE:
    entry = &block->variable_head;
    tail = &block->variable_tail;
    break;
  }

  entry = find_entry(entry, symbol_name);

  if (*entry) {
    fprintf(stderr, "symbol '%s' already defined\n", symbol_name);
    exit(1);
  }

  symbol_entry *new = new_symbol_entry(symbol_name);

  *entry = new;
  *tail = new;

  return new;
}

symbol_entry *get_symbol(block_statement *block, char *symbol_name,
    uint8_t *symbol_type) {
  uint8_t symtype = *symbol_type;

  bool escape = false;
  do {
    uint8_t symbol_types[] = {ST_FUNCTION, ST_VARIABLE, ST_CLASS, ST_TYPE};

    for (size_t i = 0; i < sizeof(symbol_types) / sizeof(*symbol_types); i++) {
      uint8_t compare_symbol_type = symbol_types[i];

      if (!(compare_symbol_type & symtype)) {
        continue;
      }

      symbol_entry *entry;

      switch (compare_symbol_type) {
      case ST_CLASS: entry = block->class_head; break;
      case ST_FUNCTION: entry = block->function_head; break;
      case ST_TYPE: entry = block->type_head; break;
      case ST_VARIABLE: entry = block->variable_head; break;
      }

      entry = get_entry(entry, symbol_name);
      if (entry != NULL) {
        *symbol_type = compare_symbol_type;

        return entry;
      }
    }

    block = parent_scope(block, &escape);
  } while (block);

  return NULL;
}
