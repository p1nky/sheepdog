#ifndef TRACE_H
#define TRACE_H

#define INSN_SIZE       5       /* call(1b) + offset(4b) = 5b */

#ifndef __ASSEMBLY__
#include <stdlib.h>

#include "sheepdog_proto.h"
#include "sheep.h"
#include "list.h"
#include "util.h"
#include "../work.h"

struct ipinfo {
	const char *file;           /* Source code filename for EIP */
	int line;                   /* Source code linenumber for EIP */
	const char *fn_name;        /* Name of function containing EIP */
	int fn_namelen;             /* Length of function name */
	unsigned long fn_addr;      /* Address of start of function */
	int fn_narg;                /* Number of function arguments */
};

struct caller {
	struct list_head list;
	struct hlist_node hash;
	unsigned long mcount;
	int namelen;
	const char *name;
};

typedef void (*trace_func_t)(unsigned long ip, unsigned long *parent_ip);
/* Type of the callback handlers for function entry and return */
typedef void (*trace_func_graph_ret_t)(struct trace_graph_item *);
typedef void (*trace_func_graph_ent_t)(struct trace_graph_item *);

/* graph.c */

/* stabs.c */
int get_ipinfo(unsigned long ip, struct ipinfo *info);

/* mcount.S */
void mcount(void);
void mcount_call(void);
void trace_caller(void);
void trace_call(unsigned long, unsigned long *);
extern const unsigned char NOP5[];
void trace_return_caller(void);
unsigned long trace_return_call(void);

/* trace.c */
#ifdef ENABLE_TRACE
  int trace_init_signal(void);
  int trace_init(void);
  int register_trace_function(trace_func_t func);
  int trace_enable(void);
  int trace_disable(void);
  struct caller *trace_lookup_ip(unsigned long ip, bool create);
  int trace_buffer_pop(void *buf, uint32_t len);
  void trace_buffer_push(int cpuid, struct trace_graph_item *item);
  void trace_register_thread(pthread_t id);
  void trace_unregister_thread(pthread_t id);

#else
  static inline int trace_init_signal(void) { return 0; }
  static inline int trace_init(void) { return 0; }
  static inline int trace_enable(void) { return 0; }
  static inline int trace_disable(void) { return 0; }
  static inline int trace_buffer_pop(void *buf, uint32_t len) { return 0; }
  static inline void trace_buffer_push(
	  int cpuid, struct trace_graph_item *item) { return; }
  static inline void trace_register_thread(pthread_t id) { return; }
  static inline void trace_unregister_thread(pthread_t id) { return; }

#endif /* ENABLE_TRACE */

#define register_tracer(new)			\
static void __attribute__((constructor))	\
register_ ## _tracer(void) 			\
{  						\
	register_trace_function(new);		\
}

#endif /* __ASSEMBLY__ */
#endif
