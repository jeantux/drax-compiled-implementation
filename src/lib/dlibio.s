/* Drax Lang - 2022 */

.global dsys_out

dsys_out:
  call   str_size
  mov    $1, %eax
  mov    $1, %edi
  syscall
  ret

str_size:
  mov    %esi, %edx

next_char:
  cmpb   $0x0,(%edx)
  je     finish
  inc    %edx
  jmp    next_char

finish:
  sub    %esi, %edx
  ret
