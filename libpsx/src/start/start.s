# This is the start code for the PSXSDK.
# It sets needed things up, and calls the setup and the main function.

# This has to be linked in as the first object when using ld, so that it
# appears at the start of the .text section.
# If a ldscript is being used, it is sufficient to specify this as the first
# startup object.

# nextvolume (2014-03-17):
# fixed possible stack corruption.

	.align 16
	.text
.global _start
#.global exit
.extern call_atexit_callbacks
#.global vblank_handler
.extern vblank_handler
.extern vblank_handler_callback

.extern rcnt_handler
.extern rcnt_handler_evfield
.extern rcnt_handler_callback

.extern run_bios
.extern is_load_delay_ok
.extern exit

.extern get_cop0_status
.extern set_cop0_status
.extern get_cop0_epc
#.global run_bios
#.global is_load_delay_ok

_start:
	li $29, 0x801fff00 # Load stack pointer
	li $k1, 0x1f800000            # set to hardware base
	
	addiu $sp, $sp, -24
	jal ResetEntryInt
	nop
	
	jal psxsdk_setup
	nop
	
	jal main
	nop
	
_real_exit:
	la $a0, progtermfmt
	move $a1, $v0
	
	jal printf
	nop

	jal call_atexit_callbacks
	nop

inf_loop:
	j inf_loop
	nop

# VBlank handler

vblank_handler:
	addi $sp, -120
.set noat
	sw $at, 0($sp)
	mfhi $at
	sw $at, 112($sp)
	mflo $at
	sw $at, 116($sp)
.set at
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
	
vblank_fire_user_handler:

	la $t0, vblank_handler_callback
	lw $t1, 0($t0)
	
	addiu $sp, $sp, -24
	jalr $t1
	nop
	addiu $sp, $sp, 24
	
vblank_acknowledge_irq:
	li $t0, 0x1f801070 # IPENDING
	
	lw $t1, 0($t0)
	nop
	nop
	xori $t1, $t1, 1 # Acknowledge VBlank IRQ
	sw $t1, 0($t0)
	
vblank_handler_end:
.set noat
	lw $at, 112($sp)
	nop
	mthi $at
	lw $at, 116($sp)
	nop
	mtlo $at
	lw $at, 0($sp)
.set at
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
	addi $sp, 120
	jr $ra
	nop

# Root counter handler

rcnt_handler:
	addi $sp, -120
.set noat
	sw $at, 0($sp)
	mfhi $at
	sw $at, 112($sp)
	mflo $at
	sw $at, 116($sp)
.set at
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

rcnt_fire_user_handler:
	la $t0, rcnt_handler_callback
	lw $t1, 0($t0)
	
	addiu $sp, $sp, -24
	jalr $t1
	nop
	addiu $sp, $sp, 24
	
rcnt_acknowledge_irq:
	li $t0, 0x1f801070 # IPENDING
	la $t2, rcnt_handler_evfield
	
	lw $t1, 0($t0)
	nop
	nop
	xor $t1, $t1, $t2 # Acknowledge Root Counter IRQ
	sw $t1, 0($t0)
	
rcnt_handler_end:
.set noat
	lw $at, 112($sp)
	nop
	mthi $at
	lw $at, 116($sp)
	nop
	mtlo $at
	lw $at, 0($sp)
.set at
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
	addi $sp, 120
	jr $ra
	nop

get_cop0_status:
	mfc0 $v0, $12
	jr $ra
	nop

set_cop0_status:
	mtc0 $a0, $12
	jr $ra
	nop
	
get_cop0_epc:
	mfc0 $2, $14
	jr $ra
	nop
	
run_bios:
	j 0xbfc00000
	nop
	
is_load_delay_ok:
	li $t2, 0
	li $t0, 0x0adecade
	la $t1, isldo_data
	.word 0x8d2a0000	#lw t2,0(t1)
	beq $t0, $t2, load_delay_not_ok
	nop
load_delay_ok:
	li $v0, 1
	jr $ra
	nop
load_delay_not_ok:
	li $v0, 0
	jr $ra
	nop

exit:
	move $s0, $a0
	
	la $a0, called_exit
	move $a1, $s0
	
	addiu $sp, $sp, -24
	jal printf
	nop
	addiu $sp, $sp, 24

	move $v0, $s0

	j _real_exit
	nop

	.align 16
	.data

called_exit:
	.string "Called exit(%d)\n"

progtermfmt:
	.string "Program terminated with return value %d\n"

isldo_data:
	.word 0x0adecade
