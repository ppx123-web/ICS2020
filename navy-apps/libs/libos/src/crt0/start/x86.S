.globl  _start
_start:
  movl $0, %ebp
  movl %eax, %esp
  pushl %esp
  call call_main
