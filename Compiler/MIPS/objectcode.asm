
.data
_hint: .asciiz "Input:"
_ret: .asciiz "\n"
.globl main
.text
read:
    li $v0,4
    la $a0,_hint
    syscall
    li $v0,5
    syscall
    jr $ra
print:
    li $v0,1
    syscall
    li $v0,4
    la $a0,_ret
    syscall
    move $v0,$0
    jr $ra
main:
	addi $sp,$sp,-24
	sw $t0,0($sp)
	sw $t1,4($sp)
	sw $t2,8($sp)
	sw $t3,12($sp)
	sw $t4,16($sp)
	sw $t5,20($sp)
	li $t5,0
	move $t1,$t5
	li $t5,0
	move $t0,$t5
	li $t5,0
	move $t2,$t5
	addi $sp,$sp,-4
	sw $ra,0($sp)
	jal read
	lw $ra,0($sp)
	move $t5,$v0
	addi $sp,$sp,4
	move $t0,$t5
label_0:
	slt $t5,$t1,$t0
	bne $t5,$zero,label_1
	j label_2
label_1:
	li $t4,1
	add $t3,$t1,$t4
	move $t1,$t3
	add $t4,$t2,$t1
	move $t2,$t4
	j label_0
label_2:
	move $a0,$t2
	addi $sp,$sp,-4
	sw $ra,0($sp)
	jal print
	lw $ra,0($sp)
	addi $sp,$sp,4
	lw $t0,0($sp)
	lw $t1,4($sp)
	lw $t2,8($sp)
	lw $t3,12($sp)
	lw $t4,16($sp)
	lw $t5,20($sp)
	addi $sp,$sp,24
