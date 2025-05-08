/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
void undefined_instruction_irq_handler (void);
void software_interrupt_irq_handler(void);
void data_abort_irq_handler(void);
void prefetch_abort_irq_handler(void);

void __attribute((weak, noreturn)) undefined_instruction_irq_handler (void)
{
    while(true)
    {
        /* Spin forever */
    }
}

void __attribute((weak, noreturn)) software_interrupt_irq_handler(void)
{
    while(true)
    {
        /* Spin forever */
    }
}

void __attribute((weak, noreturn)) data_abort_irq_handler(void)
{
    uint32_t pc, sp, lr;
    uint32_t dfsr, dfar;
    // Inline assembly to capture SP, LR, and PC (as best as possible)
    __asm__ volatile (
        "mov %[sp], sp\n\t"
        "mov %[lr], lr\n\t"
        "mov %[pc], r15\n\t"
        : [sp] "=r" (sp), [lr] "=r" (lr), [pc] "=r" (pc)
    );
    __asm__ volatile("mrc p15, 0, %0, c5, c0, 0" : "=r"(dfsr)); // Read DFSR
    __asm__ volatile("mrc p15, 0, %0, c6, c0, 0" : "=r"(dfar)); // Read DFAR
    printf("DATA ABORT: PC=0x%08lX SP=0x%08lX LR=0x%08lX\n", pc, sp, lr);
    printf("DFSR=0x%08lX DFAR=0x%08lX\n", dfsr, dfar);
    while (1);
    
    //while(true)
    //{
        /* Spin forever */
    //}
}

void __attribute((weak, noreturn)) prefetch_abort_irq_handler(void)
{
    while(true)
    {
        /* Spin forever */
    }
}

