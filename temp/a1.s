.data
_hello: .asciiz "hello\n"
.globl main

.text

main:
li $t5, 0
li $t4, 0
li $t3, 0

li $v0, 4
la $a0, _hello
syscall

move $v0, $0
jr $ra
