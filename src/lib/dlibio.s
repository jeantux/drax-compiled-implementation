/* Drax Lang - 2022 */

.global dsys_out

/* i386 syscalls */
dsys_out:
  call   str_size
  mov    $4, %eax
  mov    $1, %ebx
  int    $0x80
  ret

str_size:
  mov    %ecx, %edx

next_char:
  cmpb   $0x0,(%edx)
  je     finish
  inc    %edx
  jmp    next_char

finish:
  sub    %ecx, %edx
  ret
