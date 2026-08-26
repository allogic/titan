#include <ti_pch.h>

static LONG WINAPI dmalloc_veh_proc(struct _EXCEPTION_POINTERS *exception_info);

static char s_time_buffer[64] = {0};

static void *s_veh_handler = 0;

static uint64_t s_stack_index = 0;
static uint64_t s_stack_capacity = 0;

static dalloc_t **s_alloc_stack = 0;

void dmalloc_create(void) {
  SymInitialize(GetCurrentProcess(), 0, 1);

  s_veh_handler = AddVectoredExceptionHandler(1, dmalloc_veh_proc);

  s_stack_index = 0;
  s_stack_capacity = TI_DMALLOC_STACK_CAPACITY;

  s_alloc_stack = (dalloc_t **)malloc(sizeof(dalloc_t *) * TI_DMALLOC_STACK_CAPACITY);
}
void *dmalloc_alloc(char const *file_name, char const *function_name, uint64_t line_number, uint64_t data_size, uint32_t zero_data, void const *data_ref) {
  uint64_t block_size = TI_DMALLOC_PAGE_SIZE + sizeof(dalloc_t) + data_size + TI_DMALLOC_PAGE_SIZE;
  uint64_t total_pages = (block_size + TI_DMALLOC_PAGE_SIZE - 1) / TI_DMALLOC_PAGE_SIZE;
  uint64_t total_size = total_pages * TI_DMALLOC_PAGE_SIZE;
  uint64_t writable_pages = total_pages - 2;
  uint64_t writable_size = writable_pages * TI_DMALLOC_PAGE_SIZE;

  uint8_t *mapping = 0;
  uint8_t *front_guard = 0;
  uint8_t *writable = 0;
  uint8_t *rear_guard = 0;
  uint8_t *data = 0;

  dalloc_t *dalloc = 0;

  uint32_t old_protect = 0;

  mapping = VirtualAlloc(0, total_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

  if (mapping == 0) {
    return 0;
  }

  front_guard = mapping;
  writable = mapping + TI_DMALLOC_PAGE_SIZE;
  rear_guard = mapping + total_size - TI_DMALLOC_PAGE_SIZE;

  data = mapping + total_size - TI_DMALLOC_PAGE_SIZE - data_size;
  dalloc = (dalloc_t *)(data - sizeof(dalloc_t));

  dalloc->file_name = file_name;
  dalloc->function_name = function_name;
  dalloc->mapping = mapping;
  dalloc->data = data;
  dalloc->line_number = line_number;
  dalloc->total_pages = total_pages;
  dalloc->total_size = total_size;
  dalloc->writable_pages = writable_pages;
  dalloc->writable_size = writable_size;
  dalloc->data_size = data_size;
  dalloc->stack_depth = (uint64_t)RtlCaptureStackBackTrace(1, TI_DMALLOC_STACK_DEPTH, dalloc->stack, 0);
  dalloc->time = time(0);

#ifdef TI_DMALLOC_ENABLE_TRACE
  strftime(s_time_buffer, sizeof(s_time_buffer), "%Y-%m-%d %H:%M:%S", localtime(&dalloc->time));

  printf("[%s] %s:%llu %llu bytes allocated\n",
         s_time_buffer,
         dalloc->function_name,
         dalloc->line_number,
         dalloc->data_size);
#endif

  if (s_stack_index >= s_stack_capacity) {

    uint64_t new_stack_capacity = s_stack_capacity * 2;
    dalloc_t **new_alloc_stack = (dalloc_t **)malloc(sizeof(dalloc_t *) * new_stack_capacity);

    memcpy(new_alloc_stack, s_alloc_stack, sizeof(dalloc_t *) * s_stack_index);

    free(s_alloc_stack);

    s_stack_capacity = new_stack_capacity;
    s_alloc_stack = new_alloc_stack;
  }

  s_alloc_stack[s_stack_index++] = dalloc;

  if (zero_data) {
    memset(data, 0, data_size);
  }

  if (data_ref) {
    memcpy(data, data_ref, data_size);
  }

  if (VirtualProtect(front_guard, TI_DMALLOC_PAGE_SIZE, PAGE_READWRITE | PAGE_GUARD, &old_protect) == 0) {

    VirtualFree(mapping, 0, MEM_RELEASE);

    return 0;
  }

  if (VirtualProtect(rear_guard, TI_DMALLOC_PAGE_SIZE, PAGE_READWRITE | PAGE_GUARD, &old_protect) == 0) {

    VirtualFree(mapping, 0, MEM_RELEASE);

    return 0;
  }

  return data;
}
void dmalloc_free(char const *file_name, char const *function_name, uint64_t line_number, void *data) {
  if (data == 0) {
    return;
  }

  dalloc_t *dalloc = (dalloc_t *)(((uint8_t *)data) - sizeof(dalloc_t));

#ifdef TI_DMALLOC_ENABLE_TRACE
  strftime(s_time_buffer, sizeof(s_time_buffer), "%Y-%m-%d %H:%M:%S", localtime(&dalloc->time));

  printf("[%s] %s:%llu %llu bytes freed\n",
         s_time_buffer,
         function_name,
         line_number,
         dalloc->data_size);
#endif

  uint64_t alloc_index = s_stack_index;

  while (alloc_index--) {

    if (s_alloc_stack[alloc_index] == dalloc) {
      break;
    }
  }

  dalloc_t *tmp_dalloc = s_alloc_stack[s_stack_index - 1];
  s_alloc_stack[s_stack_index - 1] = s_alloc_stack[alloc_index];
  s_alloc_stack[alloc_index] = tmp_dalloc;

  s_stack_index--;

  VirtualFree(dalloc->mapping, 0, MEM_RELEASE);
}
void dmalloc_destroy(void) {
  HANDLE process = GetCurrentProcess();

  uint64_t total_size = 0;
  uint64_t alloc_index = s_stack_index;

  while (alloc_index--) {
    total_size += s_alloc_stack[alloc_index]->data_size;
  }

  if (s_stack_index) {

    printf(
      "***********************************\n"
      "\n"
      "DMALLOC MEMORY LEAK\n"
      "  Allocations : %llu\n"
      "  Size        : %llu\n",
      s_stack_index,
      total_size);

    printf("\n");
  }

  alloc_index = s_stack_index;

  while (alloc_index--) {

    dalloc_t *dalloc = s_alloc_stack[alloc_index];

    printf("Allocation:\n");

    uint64_t stack_index = 0;
    uint64_t stack_count = dalloc->stack_depth;

    while (stack_index < stack_count) {

      CHAR symbol_buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {0};
      PSYMBOL_INFO symbol_info = (PSYMBOL_INFO)symbol_buffer;

      symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
      symbol_info->MaxNameLen = MAX_SYM_NAME;

      DWORD64 address = (DWORD64)dalloc->stack[stack_index];
      DWORD64 displacement = 0;
      DWORD32 line_displacement = 0;

      if (SymFromAddr(process, address, &displacement, symbol_info)) {

        IMAGEHLP_LINE64 line = {
          .SizeOfStruct = sizeof(IMAGEHLP_LINE64),
        };

        if (SymGetLineFromAddr64(process, address, &line_displacement, &line)) {

          printf("  #%llu %s:%lu\n", stack_index, line.FileName, line.LineNumber);
        }
      }

      stack_index++;
    }

    printf("\n");
  }

  if (s_stack_index) {
    printf("***********************************\n");
  }

  free(s_alloc_stack);

  s_stack_index = 0;
  s_stack_capacity = 0;

  s_alloc_stack = 0;

  RemoveVectoredExceptionHandler(s_veh_handler);

  SymCleanup(process);
}

static LONG WINAPI dmalloc_veh_proc(struct _EXCEPTION_POINTERS *exception_info) {
  PCONTEXT context = exception_info->ContextRecord;
  PEXCEPTION_RECORD exception = exception_info->ExceptionRecord;

  if (exception->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) {

    DWORD64 access_type = (DWORD64)exception->ExceptionInformation[0];
    DWORD64 fault_address = (DWORD64)exception->ExceptionInformation[1];

    DWORD64 alloc_index = s_stack_index;

    while (alloc_index--) {

      dalloc_t *dalloc = dalloc = s_alloc_stack[alloc_index];

      DWORD64 mapping_begin = (DWORD64)dalloc->mapping;
      DWORD64 mapping_end = mapping_begin + dalloc->total_size;

      DWORD64 data_begin = (DWORD64)dalloc->data;
      DWORD64 data_end = data_begin + dalloc->data_size;

      if ((fault_address >= mapping_begin) && (fault_address < mapping_end)) {

        printf(
          "***********************************\n"
          "\n"
          "DMALLOC GUARD VIOLATION\n"
          "  Fault       : %p\n"
          "  Instruction : %p\n",
          (PVOID)fault_address,
          (PVOID)context->Rip);

        if (access_type == 0) {
          printf("  Access      : READ\n");
        } else if (access_type == 1) {
          printf("  Access      : WRITE\n");
        } else if (access_type == 8) {
          printf("  Access      : EXECUTE\n");
        } else {
          printf("  Access      : UNKNOWN\n");
        }

        if (fault_address < data_begin) {
          printf("  Region      : FRONT GUARD\n");
        } else if (fault_address >= data_end) {
          printf("  Region      : REAR GUARD\n");
          printf("  Overrun     : %llu\n", (DWORD64)(fault_address - data_end));
        } else {
          printf("  Region      : DATA\n");
          printf("  Offset      : %llu\n", (DWORD64)(fault_address - data_begin));
        }

        printf("\n");

        HANDLE process = GetCurrentProcess();
        HANDLE thread = GetCurrentThread();

        STACKFRAME64 stack_frame = {
          .AddrPC = {
            .Offset = context->Rip,
            .Mode = AddrModeFlat,
          },
          .AddrFrame = {
            .Offset = context->Rsp,
            .Mode = AddrModeFlat,
          },
          .AddrStack = {
            .Offset = context->Rsp,
            .Mode = AddrModeFlat,
          },
        };

        printf("Strack Trace:\n");

        DWORD64 depth = 0;

        while (StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, &stack_frame, context, 0, SymFunctionTableAccess64, SymGetModuleBase64, 0)) {

          CHAR symbol_buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {0};
          PSYMBOL_INFO symbol_info = (PSYMBOL_INFO)symbol_buffer;

          symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
          symbol_info->MaxNameLen = MAX_SYM_NAME;

          DWORD64 address = stack_frame.AddrPC.Offset;
          DWORD64 displacement = 0;
          DWORD32 line_displacement = 0;

          if (SymFromAddr(process, address, &displacement, symbol_info)) {

            IMAGEHLP_LINE64 line = {
              .SizeOfStruct = sizeof(IMAGEHLP_LINE64),
            };

            if (SymGetLineFromAddr64(process, address, &line_displacement, &line)) {

              printf("  #%llu %s:%lu\n", depth, line.FileName, line.LineNumber);

              depth++;
            }
          }
        }

        printf("\n");
        printf("***********************************\n");

        ExitProcess(EXIT_FAILURE);
      }
    }

    return EXCEPTION_CONTINUE_SEARCH;
  }

  return EXCEPTION_CONTINUE_SEARCH;
}
