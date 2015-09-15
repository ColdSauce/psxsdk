
.text
.global __psxsdk_exception_manager

__psxsdk_exception_manager:

# Save registers on stack

.set noat
	addiu $sp, $sp, -112
	sw $at, 0($sp)
	sw $v0, 4($sp)
	sw $v1, 8($sp)
	sw $a0, 12($sp)
	sw $a1, 16($sp)
	sw $a2, 20($sp)
	sw $a3, 24($sp)
	sw $t0,  28($sp)
	sw $t1,  32($sp)
	sw $t2,  36($sp)
	sw $t3, 40($sp)
	sw $t4, 44($sp)
	sw $t5, 48($sp)
	sw $t6, 52($sp)
	sw $t7, 56($sp)
	sw $s0, 60($sp)
	sw $s1, 64($sp)
	sw $s2, 68($sp)
	sw $s3, 72($sp)
	sw $s4, 76($sp)
	sw $s5, 80($sp)
	sw $s6, 84($sp)
	sw $s7, 88($sp)
	sw $t8, 92($sp)
	sw $t9, 96($sp)
	sw $gp, 100($sp)
	sw $fp, 104($sp)
	sw $ra, 108($sp)
.set at	
	
# Execute real exception handler
	jal __psxsdk_real_exception_handler
	nop

# Load registers from stack

.set noat
	lw $at, 0($sp)
	lw $v0, 4($sp)
	lw $v1, 8($sp)
	lw $a0, 12($sp)
	lw $a1, 16($sp)
	lw $a2, 20($sp)
	lw $a3, 24($sp)
	lw $t0, 28($sp)
	lw $t1, 32($sp)
	lw $t2, 36($sp)
	lw $t3, 40($sp)
	lw $t4, 44($sp)
	lw $t5, 48($sp)
	lw $t6, 52($sp)
	lw $t7, 56($sp)
	lw $s0, 60($sp)
	lw $s1, 64($sp)
	lw $s2, 68($sp)
	lw $s3, 72($sp)
	lw $s4, 76($sp)
	lw $s5, 80($sp)
	lw $s6, 84($sp)
	lw $s7, 88($sp)
	lw $t8, 92($sp)
	lw $t9, 96($sp)
	lw $gp, 100($sp)
	lw $fp, 104($sp)
	lw $ra, 108($sp)
	addiu $sp, $sp, 112
.set at		
	
# Get exception return address..
	mfc0 $k0, $14
# Exit from exception handler
.set noreorder # Do not let the assembler fill the delay slot!
	jr $k0
	rfe
.set reorder # The assembler can fill the delay slot again.
