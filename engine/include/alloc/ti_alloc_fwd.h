#ifndef TI_ALLOC_FWD_H
#define TI_ALLOC_FWD_H

typedef struct dalloc_t {
  char const *file_name;
  char const *function_name;
  void *mapping;
  void *data;
  void *stack[TI_DMALLOC_STACK_DEPTH];
  uint64_t line_number;
  uint64_t total_pages;
  uint64_t total_size;
  uint64_t writable_pages;
  uint64_t writable_size;
  uint64_t data_size;
  uint64_t stack_depth;
  time_t time;
} dalloc_t;

#endif // TI_ALLOC_FWD_H
