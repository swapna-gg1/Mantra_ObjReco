/*******************************************************************************
  SYS PORTS Static Functions for PORTS System Service

  Company:
    Microchip Technology Inc.

  File Name:
    plib_pio.c

  Summary:
    PIO function implementations for the PIO PLIB.

  Description:
    The PIO PLIB provides a simple interface to manage peripheral
    input-output controller.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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
//DOM-IGNORE-END

#include "plib_pio.h"
#include "interrupts.h"



/******************************************************************************
  Function:
    PIO_Initialize ( void )

  Summary:
    Initialize the PIO library.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_Initialize ( void )
{
 /* Port A Peripheral function GPIO configuration */
   PIOA_REGS->PIO_MSKR = 0xa8000400LU;
   PIOA_REGS->PIO_CFGR = 0x0U;

 /* Port A Pin 10 configuration */
   PIOA_REGS->PIO_MSKR = 0x400U;
   PIOA_REGS->PIO_CFGR = (PIOA_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;

 /* Port A Pin 27 configuration */
   PIOA_REGS->PIO_MSKR = 0x8000000U;
   PIOA_REGS->PIO_CFGR = (PIOA_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;

 /* Port A Pin 29 configuration */
   PIOA_REGS->PIO_MSKR = 0x20000000U;
   PIOA_REGS->PIO_CFGR = (PIOA_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x200U;

 /* Port A Pin 31 configuration */
   PIOA_REGS->PIO_MSKR = 0x80000000LU;
   PIOA_REGS->PIO_CFGR = (PIOA_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;

 /* Port A Latch configuration */
   PIOA_REGS->PIO_SODR = 0x0LU;
   PIOA_REGS->PIO_CODR = 0xa8000400LU & ~0x0LU;

 /* Port B Peripheral function GPIO configuration */
   PIOB_REGS->PIO_MSKR = 0x2U;
   PIOB_REGS->PIO_CFGR = 0x0U;

 /* Port B Pin 1 configuration */
   PIOB_REGS->PIO_MSKR = 0x2U;
   PIOB_REGS->PIO_CFGR = (PIOB_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;

 /* Port B Latch configuration */
   PIOB_REGS->PIO_CODR = 0x2U;


 /* Port D Peripheral function A configuration */
   PIOD_REGS->PIO_MSKR = 0xcU;
   PIOD_REGS->PIO_CFGR = 0x1U;

 /* Port D Peripheral function GPIO configuration */
   PIOD_REGS->PIO_MSKR = 0x80000U;
   PIOD_REGS->PIO_CFGR = 0x0U;

 /* Port D Latch configuration */
   PIOD_REGS->PIO_CODR = 0x80000U;





}

// *****************************************************************************
// *****************************************************************************
// Section: PIO APIs which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t PIO_PortRead ( PIO_PORT port )

  Summary:
    Read all the I/O lines of the selected port.

  Description:
    This function reads the live data values on all the I/O lines of the
    selected port.  Bit values returned in each position indicate corresponding
    pin levels.
    1 = Pin is high.
    0 = Pin is low.

    This function reads the value regardless of pin configuration, whether it is
    set as as an input, driven by the PIO Controller, or driven by a peripheral.

  Remarks:
    If the port has less than 32-bits, unimplemented pins will read as
    low (0).
    Implemented pins are Right aligned in the 32-bit return value.
*/
uint32_t PIO_PortRead(PIO_PORT port)
{
    return PIO_REGS->PIO_GROUP[port].PIO_PDSR;
}


// *****************************************************************************
/* Function:
    void PIO_PortWrite (PIO_PORT port, uint32_t mask, uint32_t value);

  Summary:
    Write the value on the masked I/O lines of the selected port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortWrite(PIO_PORT port, uint32_t mask, uint32_t value)
{
    PIO_REGS->PIO_GROUP[port].PIO_MSKR = mask;
    PIO_REGS->PIO_GROUP[port].PIO_ODSR = value;
}


// *****************************************************************************
/* Function:
    uint32_t PIO_PortLatchRead ( PIO_PORT port )

  Summary:
    Read the latched value on all the I/O lines of the selected port.

  Remarks:
    See plib_pio.h for more details.
*/
uint32_t PIO_PortLatchRead(PIO_PORT port)
{
    return PIO_REGS->PIO_GROUP[port].PIO_ODSR;
}


// *****************************************************************************
/* Function:
    void PIO_PortSet ( PIO_PORT port, uint32_t mask )

  Summary:
    Set the selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortSet(PIO_PORT port, uint32_t mask)
{
    PIO_REGS->PIO_GROUP[port].PIO_SODR = mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortClear ( PIO_PORT port, uint32_t mask )

  Summary:
    Clear the selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortClear(PIO_PORT port, uint32_t mask)
{
    PIO_REGS->PIO_GROUP[port].PIO_CODR = mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortToggle ( PIO_PORT port, uint32_t mask )

  Summary:
    Toggles the selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortToggle(PIO_PORT port, uint32_t mask)
{
    /* Write into Clr and Set registers */
    PIO_REGS->PIO_GROUP[port].PIO_MSKR = mask;
    PIO_REGS->PIO_GROUP[port].PIO_ODSR ^= mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortInputEnable ( PIO_PORT port, uint32_t mask )

  Summary:
    Enables selected IO pins of a port as input.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortInputEnable(PIO_PORT port, uint32_t mask)
{
    PIO_REGS->PIO_GROUP[port].PIO_MSKR = mask;
    PIO_REGS->PIO_GROUP[port].PIO_CFGR &= ~PIO_CFGR_DIR_Msk;
}

// *****************************************************************************
/* Function:
    void PIO_PortOutputEnable ( PIO_PORT port, uint32_t mask )

  Summary:
    Enables selected IO pins of a port as output(s).

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortOutputEnable(PIO_PORT port, uint32_t mask)
{
    PIO_REGS->PIO_GROUP[port].PIO_MSKR = mask;
    PIO_REGS->PIO_GROUP[port].PIO_CFGR |= PIO_CFGR_DIR_Msk;
}


/*******************************************************************************
 End of File
*/
