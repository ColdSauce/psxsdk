# PSX SDK System calls file

.text

# Console functions

.global bios_putchar
.global bios_puts
.global printf
.extern __stdio_direction
.extern sio_printf

bios_putchar:
	li $9, 0x3c
	j 0xa0
	nop
	
bios_puts:
	li $9, 0x3e
	j 0xa0
	nop
	
printf:
	la $9, __stdio_direction
	lw $10, 0($9)
	beq $10, $0, use_bios_printf
	nop
	j sio_printf
	nop
use_bios_printf:
	li $9, 0x3f
	j 0xa0
	nop

# Memory functions
# These are not used by PSXSDK, and are here just for completeness.

.global InitHeap
.global FlushCache

InitHeap:
	li $9, 0x39
	j 0xa0
	nop
	
FlushCache:
	li $9, 0x44
	j 0xa0
	nop

# GPU functions
.global GPU_dw
.global mem2vram
.global SendGPU
.global GPU_cw
.global GPU_cwb
.global SendPrimitive
.global GetGPUStatus

GPU_dw:
	li $9, 0x46
	j 0xa0
	nop

mem2vram:
	li $9, 0x47
	j 0xa0
	nop

.global ResetEntryInt

ResetEntryInt:
	li $9, 0x18
	j 0xb0
	nop

# ???
.global GetKernelDate
.global GetKernelRomVersion
.global GetRamSize

GetKernelDate:
	li $9, 0xb4
	li $4, 0
	j 0xa0
	nop

GetKernelRomVersion:
	li $9, 0xb4
	li $4, 2
	j 0xa0
	nop

GetRamSize:
	li $9, 0xb4
	li $4, 5
	j 0xa0
	nop

# Event functions
.global OpenEvent
.global EnableEvent
.global CloseEvent
.global DisableEvent
.global WaitEvent
.global TestEvent
.global DeliverEvent

OpenEvent:
	li $9, 0x08
	j 0xb0
	nop

EnableEvent:
	li $9, 0x0c
	j 0xb0
	nop

CloseEvent:
	li $9, 0x09
	j 0xb0
	nop
	
DisableEvent:
	li $9, 0x0d
	j 0xb0
	nop

DeliverEvent:
	li $9, 0x07
	j 0xb0
	nop
	
WaitEvent:
	li $9, 0x0a
	j 0xb0
	nop
	
TestEvent:
	li $9, 0x0b
	j 0xb0
	nop

# File I/O functions
.global open
.global lseek
.global read
.global write
.global close
.global cd
.global firstfile
.global nextfile
.global rename
.global remove

open:
	li $9, 0x32
	j 0xb0
	nop
	
lseek:
	li $9, 0x33
	j 0xb0
	nop
	
read:
	li $9, 0x34
	j 0xb0
	nop
	
write:
	li $9, 0x35
	j 0xb0
	nop
	
close:
	li $9, 0x36
	j 0xb0
	nop
	
cd:
	li $9, 0x40
	j 0xb0
	nop
	
firstfile:
	li $9, 0x42
	j 0xb0
	nop
	
nextfile:
	li $9, 0x43
	j 0xb0
	nop

rename:
	li $9, 0x44
	j 0xb0
	nop
	
remove:
	li $9, 0x45
	j 0xb0
	nop
	
# Exception / Interrupt functions

.global EnterCriticalSection
.global ExitCriticalSection
.global SysEnqIntRP
.global SysDeqIntRP
	
EnterCriticalSection:
	li $a0, 1
	syscall
	nop
	jr $ra
	nop

ExitCriticalSection:
	li $a0, 2
	syscall
	nop
	jr $ra
	nop

SysEnqIntRP:
	li $9, 0x02
	j 0xc0
	nop
	
SysDeqIntRP:
	li $9, 0x03
	j 0xc0
	nop
	
# Filesystem functions

.global _96_init
.global _96_remove
.global _bu_init

_96_init:
	li $9, 0x71
	j 0xa0
	nop
	
_96_remove:
	li $9, 0x72
	j 0xa0
	nop
	
_bu_init:
	li $9, 0x70
	j 0xa0
	nop
	
# Executable loading functions

.global LoadExec

LoadExec:
	li $9, 0x51
	j 0xa0
	nop

# Memory card routines

.global InitCARD
.global StartCARD
.global StopCARD
.global _card_info
.global _card_load
.global _card_auto
.global _card_write
.global _card_read
.global _card_status
.global _new_card

InitCARD:
	li $9, 0x4a
	j 0xb0
	nop
	
StartCARD:
	li $9, 0x4b
	j 0xb0
	nop

StopCARD:
	li $9, 0x4c
	j 0xb0
	nop

_card_info:
	li $9, 0xab
	j 0xa0
	nop
	
_card_load:
	li $9, 0xac
	j 0xa0
	nop
	
_card_auto:
	li $9, 0xad
	j 0xa0
	nop

_card_write:
	li $9, 0x4e
	j 0xb0
	nop
	
_card_read:
	li $9, 0x4f
	j 0xb0
	nop

_new_card:
	li $9, 0x50
	j 0xb0
	nop
	
_card_status:
	li $9, 0x5c
	j 0xb0
	nop

# Device functions

.global PrintInstalledDevices

PrintInstalledDevices:
	li $9, 0x49
	j 0xb0
	nop

.global BIOSWarmReboot

BIOSWarmReboot:
	li $9, 0xa0
	nop
	j 0xa0
