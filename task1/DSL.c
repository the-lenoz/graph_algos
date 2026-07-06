#include "DSL.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

static int is_int(const char *str)
{
  if (!str)
    return 0;
  while (str)
    if (isdigit(*str))
      ++str;
    else
      return 0;
  return 1;
}

static int is_name(const char *str)
{
  if (!str)
    return 0;
  while (str)
    if (isalpha(*str) || isdigit(*str))
      ++str;
    else
      return 0;
  return 1;
}

static int skip_spaces(FILE *fp)
{
  if (!fp)
    return 0;
  int count = 0, c = getc(fp);
  while (isspace(c))
  {
    count++;
    c = getc(fp);
  }
  if (c != EOF)
    ungetc(c, fp);

  return count;
}

static int read_token_val(FILE *fp, char *dst)
{
  if (!fp || !dst)
    return 0;

  skip_spaces(fp);

  int c, i = 0;
  for (; i < MAX_TOKEN_LEN; ++i)
  {
    c = getc(fp);
    if (c == EOF || isspace(c))
      break;
    dst[i] = c;
  }
  return i != 0;
}

static Token next_token(FILE *fp)
{
  Token tok = (Token){};
  if (!read_token_val(fp, tok.value))
    return (Token){_INVALID};

  STR_MATCH(tok.value)

  STR_CASE("NODE")
  tok.type = NODE;

  STR_CASE("EDGE")
  tok.type = EDGE;

  STR_CASE("REMOVE")
  tok.type = REMOVE;

  STR_CASE("RPO_NUMBERING")
  tok.type = RPO_NUMBERING;

  STR_CASE("DIJKSTRA")
  tok.type = DIJKSTRA;

  STR_CASE("MAXFLOW")
  tok.type = MAXFLOW;

  STR_CASE("TARJAN")
  tok.type = TARJAN;

  STR_DEFAULT
  if (is_int(tok.value))
    tok.type = _INT;
  else if (is_name(tok.value))
    tok.type = _NAME;
  else
    tok.type = _INVALID;

  STR_MATCH_END

  return tok;
}

static Instr next_instr(FILE *fp)
{
  Token tok = next_token(fp);

  if (tok.type == _INVALID || tok.type == _INT || tok.type == _NAME)
    return (Instr){_INVALID};

  Instr instr = (Instr){.int_arg = -1};

  instr.type = tok.type;

  switch (instr.type)
  {
  case NODE:
  case RPO_NUMBERING:
  case DIJKSTRA:
  case TARJAN:
    instr.name_arg_0 = next_token(fp);
    break;
  case EDGE:
    instr.name_arg_0 = next_token(fp);
    instr.name_arg_1 = next_token(fp);

    tok = next_token(fp);
    instr.int_arg = atoi(tok.value);
    break;
  case REMOVE:
    instr.type_arg = next_token(fp);
    switch (instr.type_arg.type)
    {
    case NODE:
      instr.name_arg_0 = next_token(fp);
      break;
    case EDGE:
      instr.name_arg_0 = next_token(fp);
      instr.name_arg_1 = next_token(fp);
      break;
    default:
      fprintf(stderr, "Only 'NODE' or 'EDGE' are supported to remove.\n");
      instr.type = _INVALID;
      break;
    }
    break;
  case MAXFLOW:
    instr.name_arg_0 = next_token(fp);
    instr.name_arg_1 = next_token(fp);
    break;
  default:
    fprintf(stderr, "Unsupported instruction '%s'.\n", tok.value);
    instr.type = _INVALID;
    break;
  }
  return instr;
}

int REPL(FILE *fp, InstrExecCB instr_exec_cb)
{
  int result;
  do
  {
    result = instr_exec_cb(next_instr(fp));
  } while (result);
  return result;
}    
