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

#include "device.h"
#include "plib_clk.h"



/*********************************************************************************
Initialize Slow clock
*********************************************************************************/
static void initSlowClk(void)
{
    SCKC_REGS->SCKC_CR = (SCKC_REGS->SCKC_CR & ~SCKC_CR_OSCSEL_Msk) | SCKC_CR_OSCSEL_XTAL;
}

/*********************************************************************************
Initialize Main clock
*********************************************************************************/
static void initMainClk(void)
{
    /* Enable Main Crystal Oscillator */
    PMC_REGS->CKGR_MOR = (PMC_REGS->CKGR_MOR & ~CKGR_MOR_MOSCXTST_Msk) | CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCXTST(0) | CKGR_MOR_MOSCXTEN_Msk | CKGR_MOR_ONE_Msk;

    /* Wait until the main oscillator clock is ready */
    while ( (PMC_REGS->PMC_SR & PMC_SR_MOSCXTS_Msk) != PMC_SR_MOSCXTS_Msk)
    {
        /* Nothing to do */
    }

    /* Main Crystal Oscillator is selected as the Main Clock (MAINCK) source.
    Switch Main Clock (MAINCK) to Main Crystal Oscillator clock */
    PMC_REGS->CKGR_MOR |= CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCSEL_Msk;

    /* Wait until MAINCK is switched to Main Crystal Oscillator */
    while ( (PMC_REGS->PMC_SR & PMC_SR_MOSCSELS_Msk) != PMC_SR_MOSCSELS_Msk)
    {
        /* Nothing to do */
    }


    /* Disable the RC Oscillator */
    PMC_REGS->CKGR_MOR = CKGR_MOR_KEY_PASSWD | (PMC_REGS->CKGR_MOR & ~CKGR_MOR_MOSCRCEN_Msk);
}

/*********************************************************************************
Initialize PLLA (PLLACK)
*********************************************************************************/
static void CLK_PLLAInitialize(void)
{
    /* Configure and Enable PLLA */
    PMC_REGS->CKGR_PLLAR = CKGR_PLLAR_ONE_Msk | CKGR_PLLAR_PLLACOUNT(63U) |
                              CKGR_PLLAR_MULA(40) |
                              CKGR_PLLAR_DIVA(1U)|
                              CKGR_PLLAR_OUTA(2U);

    while ( (PMC_REGS->PMC_SR & PMC_SR_LOCKA_Msk) != PMC_SR_LOCKA_Msk)
    {
        /* Nothing to do */
    }

}

/*********************************************************************************
Initialize Master clock (MCK)
*********************************************************************************/

static void CLK_MasterClockInitialize(void)
{
    /* Program PMC_MCKR.PRES and wait for PMC_SR.MCKRDY to be set   */
    PMC_REGS->PMC_MCKR = (PMC_REGS->PMC_MCKR & ~PMC_MCKR_PRES_Msk) | PMC_MCKR_PRES_CLOCK;
    while ((PMC_REGS->PMC_SR & PMC_SR_MCKRDY_Msk) != PMC_SR_MCKRDY_Msk)
    {
        /* Nothing to do */
    }

    /* Program PMC_MCKR.MDIV and Wait for PMC_SR.MCKRDY to be set   */
    PMC_REGS->PMC_MCKR = (PMC_REGS->PMC_MCKR & ~PMC_MCKR_MDIV_Msk) | PMC_MCKR_MDIV_PCK_DIV3;
    while ((PMC_REGS->PMC_SR & PMC_SR_MCKRDY_Msk) != PMC_SR_MCKRDY_Msk)
    {
        /* Nothing to do */
    }

    /* Program PMC_MCKR.CSS and Wait for PMC_SR.MCKRDY to be set    */
    PMC_REGS->PMC_MCKR = (PMC_REGS->PMC_MCKR & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_PLLA_CLK;
    while ((PMC_REGS->PMC_SR & PMC_SR_MCKRDY_Msk) != PMC_SR_MCKRDY_Msk)
    {
        /* Nothing to do */
    }

}

/*********************************************************************************
Initialize UTMI PLL  (UPLLCK)
*********************************************************************************/

static void CLK_UTMIPLLInitialize(void)
{
    /* Set the UTMI reference clock */
    uint32_t sfr_utmiclktrim_val = SFR_REGS->SFR_UTMICKTRIM & ~SFR_UTMICKTRIM_FREQ_Msk;
    SFR_REGS->SFR_UTMICKTRIM = sfr_utmiclktrim_val | SFR_UTMICKTRIM_FREQ_24;

    /* Enable UPLL and configure UPLL lock time */
    PMC_REGS->CKGR_UCKR = CKGR_UCKR_UPLLEN_Msk | CKGR_UCKR_UPLLCOUNT(15U);

    /* Wait until PLL Lock occurs */
    while ((PMC_REGS->PMC_SR & PMC_SR_LOCKU_Msk) != PMC_SR_LOCKU_Msk)
    {
        /* Wait for PLL lock to rise */
    }
}

/*********************************************************************************
UTMI PLL Enable/Disable
*********************************************************************************/
void CLK_UTMIPLLEnable(void)
{
    /* Enable the UTMI PLL */
    PMC_REGS->CKGR_UCKR |= CKGR_UCKR_UPLLEN_Msk;

    /* Wait until PLL Lock occurs */
    while ((PMC_REGS->PMC_SR & PMC_SR_LOCKU_Msk) != PMC_SR_LOCKU_Msk)
    {
        /* Wait for PLL lock to rise */
    }
}

void CLK_UTMIPLLDisable(void)
{
    /* Disable the UTMI PLL */
    PMC_REGS->CKGR_UCKR &= ~CKGR_UCKR_UPLLEN_Msk;
}


/*********************************************************************************
Initialize USB FS clock
*********************************************************************************/

static void CLK_USBClockInitialize ( void )
{
    /* Configure Full-Speed USB Clock source and Clock Divider */
    PMC_REGS->PMC_USB = PMC_USB_USBDIV(9)  | PMC_USB_USBS_Msk;

    /* Enable Full-Speed USB Clock Output */
    PMC_REGS->PMC_SCER = PMC_SCER_UHP(1);
}

/*********************************************************************************
Initialize Generic clock
*********************************************************************************/




/*********************************************************************************
Initialize Peripheral clock
*********************************************************************************/

static void CLK_PeripheralClockInitialize(void)
{
    /* Enable clock for the selected peripherals, since the rom boot will turn on
     * certain clocks turn off all clocks not expressly enabled */
    PMC_REGS->PMC_PCER0=0x2042000U;
    PMC_REGS->PMC_PCDR0=~0x2042000U;
    PMC_REGS->PMC_PCER1=0x8U;
    PMC_REGS->PMC_PCDR1=~0x8U;
}




/*********************************************************************************
Initialize DDR Clock
*********************************************************************************/
static void initDDRClk(void)
{
    PMC_REGS->PMC_SCER |= PMC_SCER_DDRCK_Msk;
}

/*********************************************************************************
Clock Initialize
*********************************************************************************/

void CLK_Initialize( void )
{
    /* Initialize slow clock generator */
    initSlowClk();

    /* Initialize main clock generator */
    initMainClk();

    /* Initialize PLLA clock generator */
    CLK_PLLAInitialize();

    /* Initialize UTMI PLL */
    CLK_UTMIPLLInitialize();

    /* Initialize Master Clock */
    CLK_MasterClockInitialize();
    /* Initialize USB Clock */
    CLK_USBClockInitialize();

    /* Initialize Peripheral Clock */
    CLK_PeripheralClockInitialize();

    /* Initialize DDR Clock */
    initDDRClk();

}

