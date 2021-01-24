#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc = *args;
  void* argv;
  argv = (void*)(args + 1);
  void* envp;
  envp = (void*)(args + 2 + argc);
  exit(main(argc, (char**)argv, (char**)envp));
  /* char *empty[] =  {NULL };
  environ = empty;
  exit(main(0, empty, empty)); */
  assert(0);
}
