/* Drax Lang - 2022 */

.global dsys_out

str_size:
  mov    %rsi, %rdx

next_char:
  cmpb   $0x0,(%rdx)
  je     finish
  inc    %rdx
  jmp    next_char

finish:
  sub    %rsi, %rdx
  ret

dsys_out:
  call   str_size
  mov    $1, %rax
  mov    $1, %rdi
  syscall
  ret
