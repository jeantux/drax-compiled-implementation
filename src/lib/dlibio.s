/* Drax Lang - 2022 */

.global dputs
.global dputi

/* i386 syscalls */
dputs:
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


dputi:
  call itoa
  call print_buffer
  ret

/* %edi -> buffer | %edx */
itoa: 
  mov %edi, %eax
  mov $0x0, %ecx
          
  itoa_loop:
    inc %ecx
    mov $0x0, %edx
    mov $0xA, %ebx
    div %ebx
    cmp $0x0, %eax
    je itoa_end
    add $0x30, %edx
    push %edx
    jmp itoa_loop

  itoa_end:
    add $0x30, %edx
    push %edx
    mov $0x0, %eax
    mov %ecx, %edx

  stack2buffer:
    pop %edi
    mov %edi, buffer(%eax)
    dec %ecx
    inc %eax
    cmp $0x0, %ecx
    je s2c_end
    jmp stack2buffer

  s2c_end:
    ret

print_buffer:
  mov  $4, %eax
  mov  $1, %ebx
  mov  $buffer, %ecx
  int  $0x80
  ret
