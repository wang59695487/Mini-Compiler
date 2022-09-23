
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
	addi $sp,$sp,-32
	sw $t0,0($sp)
	sw $t1,4($sp)
	sw $t2,8($sp)
	sw $t3,12($sp)
	sw $t4,16($sp)
	sw $t5,20($sp)
	sw $t6,24($sp)
	sw $s0,28($sp)
	li $s0,0
	move $t2,$s0
	li $s0,1
	move $t1,$s0
	li $s0,0
	move $t4,$s0
	li $s0,1
	move $t0,$s0
	addi $sp,$sp,-4
	sw $ra,0($sp)
	jal read
	lw $ra,0($sp)
	move $s0,$v0
	addi $sp,$sp,4
	move $t3,$s0
	add $s0,$t2,$t1
	move $t4,$s0
label_0:
	li $s0,1
	sub $t6,$t3,$s0
	slt $s0,$t0,$t6
	bne $s0,$zero,label_1
	j label_2
label_1:
	move $t2,$t1
	move $t1,$t4
	add $t6,$t2,$t1
	move $t4,$t6
	li $t6,1
	add $t5,$t0,$t6
	move $t0,$t5
	j label_0
label_2:
	move $a0,$t4
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
	lw $t6,24($sp)
	lw $s0,28($sp)
	addi $sp,$sp,32
