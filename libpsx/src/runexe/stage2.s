.global PSX_RunExeStage2
.global PSX_RunExeStage2_END

PSX_RunExeStage2:
# a0 = address of PSX-EXE buffer

# Load initial program counter in t0
	lw $t0, 0x10($a0)
# Load text section address in t1
	lw $t1, 0x18($a0)
# Load text section size in t2
	lw $t2, 0x1c($a0)

# Load initial sp register value in sp
	lw $sp, 0x30($a0)

# t3 = current source address
	addiu $t3, $a0, 0x800

# t4 = current destination address
	addu $t4, $t1, $zero

copyExeLoop:
	beq $t2, $zero, copyExeLoop_End
	nop
copyExeLoop_2:
	lb $t5, 0($t3)
	nop
	sb $t5, 0($t4)
	addiu $t3, $t3, 1
	addiu $t4, $t4, 1
	addiu $t2, $t2, -1
	beq $zero, $zero, copyExeLoop
	
copyExeLoop_End:
# Jump to the program we loaded
	jr $t0
	nop

PSX_RunExeStage2_END:
