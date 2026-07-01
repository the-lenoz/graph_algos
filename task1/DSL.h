#ifndef DSL_H
#define DSL_H

#include <string.h>
#include <stdio.h>

#define STR_MATCH(val)                      \
  const char *_str_switch_internal = (val); \
  if (0)                                    \
  {
#define STR_CASE(pattern)                          \
  }                                                \
  else if (!strcmp(pattern, _str_switch_internal)) \
  {
#define STR_DEFAULT \
  }                 \
  else              \
  {
#define STR_MATCH_END }

#define MAX_TOKEN_LEN 24

typedef enum
{
  _INVALID = 0,
  NODE,
  EDGE,
  REMOVE,
  RPO_NUMBERING,
  DIJKSTRA,
  MAXFLOW,
  TARJAN,
  _INT,
  _NAME
} TokenType;

typedef struct
{
  TokenType type;
  char value[MAX_TOKEN_LEN + 1];
} Token;

typedef struct
{
  TokenType type;

  Token type_arg;

  Token name_arg_0;
  Token name_arg_1;

  int int_arg;
} Instr;

typedef int (*InstrExecCB)(Instr);

int REPL(FILE *fp, InstrExecCB instr_exec_cb);

#endif // DSL_H
