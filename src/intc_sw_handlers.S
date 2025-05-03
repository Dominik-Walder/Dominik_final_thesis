#
# Freescale Semiconductor Inc.
# (c) Copyright 2014 Freescale Semiconductor, Inc.
# ALL RIGHTS RESERVED.
#
#*******************************************************************************
#
# $File Name: MPC5777C_INTC_sw_handlers_vle_SPE.s$
#
# $Date: Mar-17-2014$
#
# $Version: 1.0.2.0$
#
# Description: MPC5777C SW Vector Mode Handlers (with SPE enabled)
#
#*******************************************************************************

    .globl   IVOR4_Handler


    .equ  INTC0_IACKR_PRC0, 0xFFF48010  # MPC5777C Core #0 Interrupt Acknowledge
                                        # Register address
    .equ  INTC0_IACKR_PRC1, 0xFFF48014  # MPC5777C Core #1 Interrupt Acknowledge
                                        # Register address

    .equ  INTC0_EOIR_PRC0,  0xFFF48018  # MPC5777C Core #0 End Of Interrupt Register
                                        # address
    .equ  INTC0_EOIR_PRC1,  0xFFF4801C  # MPC5777C Core #1 End Of Interrupt Register
                                        # address

IVOR4_Handler:

#*******************************************************************************
# Prologue
#*******************************************************************************
prolog:
    e_stwu r1, -0x80 (r1)       # Create stack frame and store back chain
    e_stw  r0, 0x2C (r1)        # Save working registers r0 low order word
    mfmsr  r0                   # Load MSR into r0
    se_bseti r0, 6              # Set MSR bit 6 (SPE)
    mtmsr  r0                   # Update MSR with SPE bit enabled
    se_isync                    # Wait for MSR[SPE] bit to be set
    evmergelohi r0,r0,r0        # r0 high word to r0 low word
    e_stw  r0, 0x28 (r1)        # Save r0 low order word (prev. high order word)

    mfSRR1 r0                   # Store SRR1 (must be done before enabling EE)
    e_stw  r0, 0x10 (r1)
    mfSRR0 r0                   # Store SRR0 (must be done before enabling EE)
    e_stw  r0, 0x0C (r1)
    mfLR   r0                   # Store LR (Store now since LR will be used
                                # for ISR Vector)
    e_stw  r0, 0x14 (r1)

    evstdd r3, 0x30 (r1)        # Store a working register

    e_lis  r3, INTC0_IACKR_PRC0@h   # Store address of IACKR in r3
    e_or2i r3, INTC0_IACKR_PRC0@l
    e_lwz  r3, 0(r3)            # Store contents of IACKR in r3
                                # (this is vector table address)
    e_lwz  r0, 0(r3)            # Read ISR address from ISR Vector Table
                                # address

    mtLR   r0                   # Store ISR address to LR to use for branching
                                # later

    wrteei 1                    # Set MSR[EE]=1 (must wait a couple clocks
                                # after reading IACKR)

    evstdd r12, 0x78 (r1)       # Store rest of gprs
    evstdd r11, 0x70 (r1)
    evstdd r10, 0x68 (r1)
    evstdd r9,  0x60 (r1)
    evstdd r8,  0x58 (r1)
    evstdd r7,  0x50 (r1)
    evstdd r6,  0x48 (r1)
    evstdd r5,  0x40 (r1)
    evstdd r4,  0x38 (r1)

    mfCR   r0                   # Store CR
    e_stw  r0,  0x20 (r1)
    mfXER  r0                   # Store XER
    e_stw  r0,  0x1C (r1)
    mfCTR  r0                   # Store CTR
    e_stw  r0,  0x18 (r1)

    se_blrl                     # Branch to ISR, but return here

#*******************************************************************************
# Prologue
#*******************************************************************************
epilog:

    mbar 0                      # Ensure interrupt flag has finished clearing
                                # before writing to INTC_EIOR

    e_lwz  r0, 0x14 (r1)        # Restore LR
    mtLR   r0
    e_lwz  r0, 0x18 (r1)        # Restore CTR
    mtCTR  r0
    e_lwz  r0, 0x1C (r1)        # Restore XER
    mtXER  r0

    e_lwz  r0, 0x20 (r1)        # Restore CR
    mtcrf  0xff, r0
    evldd  r5,  0x40 (r1)
    evldd  r6,  0x48 (r1)
    evldd  r7,  0x50 (r1)
    evldd  r8,  0x58 (r1)
    evldd  r9,  0x60 (r1)
    evldd  r10, 0x68 (r1)
    evldd  r11, 0x70 (r1)
    evldd  r12, 0x78 (r1)

    wrteei 0                    # Disable interrupts

    se_li    r3, 0
    e_lis    r4, INTC0_EOIR_PRC0@ha     # Load upper half of EIOR address to r4
    e_add16i r4, r4, INTC0_EOIR_PRC0@l  # Load lower half of EIOR address to R4
    e_stw    r3, 0(r4)          # Write 0 to INTC_EOIR, address 0xFFF4 8018

    evldd    r3, 0x30 (r1)      # Restore r3 after INTC_EOIR uses this reg.
    evldd    r4, 0x38 (r1)      # Restore r4 after INTC_EOIR uses this reg.

    e_lwz    r0, 0x0C (r1)      # Restore SRR0
    mtSRR0   r0
    e_lwz    r0, 0x10 (r1)      # Restore SRR1
    mtSRR1   r0
    evldd    r0, 0x28 (r1)      # Restore working register r0
    e_add16i r1,r1, 0x80        # Restore space on stack

    se_rfi                      # End of Interrupt - re-enables interrupts
