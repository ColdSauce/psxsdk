.global _internal_cdrom_handler
.set noat


_internal_cdrom_handler:
	addi $sp, -112
	sw $at, 0($sp)
	sw $v0, 4($sp)
	sw $v1, 8($sp)
	sw $a0, 12($sp)
	sw $a1, 16($sp)
	sw $a2, 20($sp)
	sw $a3, 24($sp)
	sw $t0, 28($sp)
	sw $t1, 32($sp)
	sw $t2, 36($sp)
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
	sw $s8, 104($sp)
	sw $ra, 108($sp)
	
# Do not run code if cdrom interrupt is not enabled	
	
	li $t0, 0x1f801074
	lw $t1, 0($t0)
	andi $t1, $t1, 4
	beq $t1, $zero, cdrom_handler_end
	nop

# Do not run code if cdrom interrupt is not pending
	
	li $t0, 0x1f801070
	lw $t1, 0($t0)
	andi $t1, $t1, 4
	beq $t1, $zero, cdrom_handler_end
	nop

# If the CDROM command isn't direct 
# (direct = sent by us and not by the BIOS' ISO9660 routines)
# exit and let the BIOS do its work.

cdrom_check_direct_cmd:
	la $t0, cdrom_command_direct
	lw $t1, 0($t0)
	beq $t1, $zero, cdrom_handler_end
	nop

cdrom_fire_user_handler:	
	la $t0, cdrom_handler_callback
	lw $t1, 0($t0)
	
	jalr $t1
	nop

# Remove bit for CDROM interrupt (bit 2) from pending interrupts mask.
	
cdrom_handler_remove_pending:
	li $t0, 0x1f801070
	lw $t1, 0($t0)
	xori $t1, $t1, 4
	sw $t1, 0($t0)

cdrom_handler_end:

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
	lw $s8, 104($sp)
	lw $ra, 108($sp)
	addi $sp, 112
	jr $ra
	nop
