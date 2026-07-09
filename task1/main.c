#include "DSL.h"
#include "algo.h"

Graph *g;
#ifdef DEBUG
#define DEBUG_LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

int exec_instr(Instr instr)
{
  switch (instr.type)
  {
  case NODE:
    DEBUG_LOG("Create node '%s'.\n", instr.name_arg_0.value);
    Graph_add_node(g, instr.name_arg_0.value);
    break;
  case EDGE:
    DEBUG_LOG("Create edge %s->%s with weight %d.\n",
              instr.name_arg_0.value, instr.name_arg_1.value, instr.int_arg);
    Graph_add_edge(g, instr.name_arg_0.value, instr.name_arg_1.value, instr.int_arg);
    break;
  case REMOVE:
    DEBUG_LOG("Remove '%s'.\n", instr.type_arg.value);
    switch (instr.type_arg.type)
    {
    case NODE:
      Graph_remove_node(g, instr.name_arg_0.value);
    case EDGE:
      Graph_remove_edge(g, instr.name_arg_0.value, instr.name_arg_1.value);
    default:
      break;
    }
    break;
  case RPO_NUMBERING:
    DEBUG_LOG("Run RPO from '%s'.\n", instr.name_arg_0.value);
    Graph_run_RPO(g, instr.name_arg_0.value);
    break;
  case DIJKSTRA:
    DEBUG_LOG("Run Dijkstra from '%s'.\n", instr.name_arg_0.value);
    Graph_run_Dijkstra(g, instr.name_arg_0.value);
    break;
  case MAXFLOW:
    DEBUG_LOG("Find max flow %s->%s.\n", instr.name_arg_0.value, instr.name_arg_1.value);
    Graph_run_max_flow(g, instr.name_arg_0.value, instr.name_arg_1.value);
    break;
  case TARJAN:
    DEBUG_LOG("Run Tarjan from '%s'.\n", instr.name_arg_0.value);
    Graph_run_Tarjan(g, instr.name_arg_0.value);
    break;
  default:
    return 0;
  }
  return 1;
}

int main(void)
{
  g = Graph_init();
  REPL(stdin, exec_instr);
  Graph_destroy(g);
}
