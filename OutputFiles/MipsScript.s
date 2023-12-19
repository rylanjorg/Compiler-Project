.text
.globl main

getSum:
	jr $ra
triple:
	jr $ra
addAndPrint:
	add $t6, $t4, $t5
	move $t7, $t6
	move $a0, $t7 
	li $v0, 1
	syscall
	jr $ra
main:
	li $t1, 10
	li $t2, 5
	jal getSum 
	li $t8, 15
	move $a0, $t8 
	li $v0, 1
	syscall
	li $t3, 15
	jal triple 
	li $t9, 45
	move $a0, $t9 
	li $v0, 1
	syscall
	li $t4, 10
	li $t5, 5
	jal addAndPrint 
	li $v0, 10
	syscall
