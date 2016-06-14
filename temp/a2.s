.data
_hello: .asciiz "hello\n"
_newline: .asciiz "\n"
.globl main

.text

main:
li $t0, 13
sw $t0, -4($sp)

li $v0, 1 # print_int
lw $a0, -4($sp)
syscall

li $v0, 4 # print_string
la $a0, _newline
syscall

move $v0, $0
jr $ra
