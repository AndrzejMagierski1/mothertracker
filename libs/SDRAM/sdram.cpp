
#include "fsl_device_registers.h"
#include "fsl_sdram.h"


#include "sdram.h"




//__NOINIT(EXTERNAL_RAM) uint16_t sdram_writeBuffer[SDRAM_EXAMPLE_DATALEN];
//__NOINIT(EXTERNAL_RAM) uint16_t sdram_writeBuffer2[SDRAM_EXAMPLE_DATALEN];
//uint16_t internall_writeBuffer[SDRAM_EXAMPLE_DATALEN];




int Extern_SDRAM_Init(void)
{


    uint32_t clockSrc;

   // uint16_t *sdram = (uint16_t *)SDRAM_START_ADDRESS; /* SDRAM start address. */

    /* Hardware initialize. */
    uint32_t soptReg;
    uint32_t fbReg;
    BOARD_InitPins();


 //   gpio_pin_config_t out_config = { kGPIO_DigitalOutput, 0,};

 //   GPIO_PinInit(GPIOD , 14U, &out_config);
    //GPIO_PinInit(GPIOD , 15U, &out_config);
    /* Set clock out to flexbus CLKOUT. */
    CLOCK_SetClkOutClock(0);

    /* Sets the Flexbus security level*/
    soptReg = SIM->SOPT2 & ~SIM_SOPT2_FBSL_MASK;
    SIM->SOPT2 = soptReg | SIM_SOPT2_FBSL(3);

    /* Enable the FB_BE_xx_yy signal in Flexbus */
    CLOCK_EnableClock(kCLOCK_Flexbus0);

    fbReg = FB->CSPMCR & ~FB_CSPMCR_GROUP2_MASK;
    FB->CSPMCR = fbReg | FB_CSPMCR_GROUP2(2);
    fbReg = FB->CSPMCR & ~FB_CSPMCR_GROUP3_MASK;
    FB->CSPMCR = fbReg | FB_CSPMCR_GROUP3(2);
    fbReg = FB->CSPMCR & ~FB_CSPMCR_GROUP4_MASK;
    FB->CSPMCR = fbReg | FB_CSPMCR_GROUP4(2);
    fbReg = FB->CSPMCR & ~FB_CSPMCR_GROUP5_MASK;
    FB->CSPMCR = fbReg | FB_CSPMCR_GROUP5(2);
    /* SDRAM initialize. */
    clockSrc = 60000000;

    if (SDRAM_Init(EXAMPLE_SDRAMC, SDRAM_START_ADDRESS, clockSrc) != kStatus_Success)
    {
    	asm( "BKPT 255" );
    }

 /*
    while(datalen--)
    {
    	internall_writeBuffer[index]=index/2;
    	internall_writeBuffer[index]=index/2;
    	index++;
    }
   // PRINTF("\r\n adr1: %x, adr2: %x \r\n",&internall_writeBuffer[0],&internall_writeBuffer[1]);
    datalen = SDRAM_EXAMPLE_DATALEN;
    index=0;
    while(datalen--)
    {
    	sdram_writeBuffer[index]=index/2;
    	sdram_writeBuffer2[index]=index/2;
    	index++;
    }
    //PRINTF("\r\n adr1: %x, adr2: %x \r\n",&sdram_writeBuffer[0],&sdram_writeBuffer[1]);

    datalen = SDRAM_EXAMPLE_DATALEN;
    index=0;
    while(datalen--)
    {
    	uint16_t zmienna=0;
    	zmienna=sdram_writeBuffer[index]+internall_writeBuffer[index];
       // PRINTF("\r\n sdram: %d, internal: %d, suma: %d \r\n",sdram_writeBuffer[index],internall_writeBuffer[index],zmienna);
        index++;

    }


    while (1)
    {
        datalen = SDRAM_EXAMPLE_DATALEN;
        index=0;
//        GPIO_PortToggle(GPIOD, 1u << 14U);
        while(datalen--)
        {
        	internall_writeBuffer[index]=internall_writeBuffer[index]+internall_writeBuffer[index];
        	index++;
        }
       // GPIO_PortToggle(GPIOD, 1u << 14U);

        datalen = SDRAM_EXAMPLE_DATALEN;
        index=0;
      //  GPIO_PortToggle(GPIOD, 1u << 15U);
        while(datalen--)
        {
        	sdram_writeBuffer[index]=sdram_writeBuffer[index]+sdram_writeBuffer[index];
        	index++;
        }

      //  GPIO_PortToggle(GPIOD, 1u << 15U);

    }

    */

    return 0;
}

void BOARD_InitPins(void)
{
    /* Port A Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortA);
    /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);
    /* Port C Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);
    /* Port D Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortD);
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);



    /* PORTA6 (pin M10) is configured as CLKOUT */
    PORTA_PCR6=PORT_PCR_MUX(5);

    /* PORTB0 (pin G13) is configured as SDRAM_CAS_b */
    PORTB_PCR0=PORT_PCR_MUX(5);

    /* PORTB1 (pin G12) is configured as SDRAM_RAS_b */
    PORTB_PCR1=PORT_PCR_MUX(5);

    /* PORTB2 (pin G11) is configured as SDRAM_WE */
    PORTB_PCR2=PORT_PCR_MUX(5);

    /* PORTB3 (pin G10) is configured as SDRAM_CS0_b */
    PORTB_PCR3=PORT_PCR_MUX(5);

    /* PORTC16 (pin D7) is configured as SDRAM_DQM2 */
    PORTC_PCR16=PORT_PCR_MUX(5);

    /* PORTC17 (pin C7) is configured as SDRAM_DQM3 */
    PORTC_PCR17=PORT_PCR_MUX(5);

    /* PORTD7 (pin E5) is configured as SDRAM_CKE */
    PORTD_PCR7=PORT_PCR_MUX(5);




    /* PORTB18 (pin D12) is configured as SDRAM_A23 */
    PORTB_PCR18=PORT_PCR_MUX(5);

    /* PORTC0 (pin B13) is configured as SDRAM_A22 */
    PORTC_PCR0=PORT_PCR_MUX(5);

    /* PORTC1 (pin B12) is configured as SDRAM_A21 */
    PORTC_PCR1=PORT_PCR_MUX(5);

    /* PORTC2 (pin A13) is configured as SDRAM_A20 */
    PORTC_PCR2=PORT_PCR_MUX(5);

    /* PORTC4 (pin B11) is configured as SDRAM_A19 */
    PORTC_PCR4=PORT_PCR_MUX(5);

    /* PORTC5 (pin A11) is configured as SDRAM_A18 */
    PORTC_PCR5=PORT_PCR_MUX(5);

    /* PORTC6 (pin A10) is configured as SDRAM_A17 */
    PORTC_PCR6=PORT_PCR_MUX(5);

    /* PORTC7 (pin B10) is configured as SDRAM_A16 */
    PORTC_PCR7=PORT_PCR_MUX(5);

    /* PORTC8 (pin C10) is configured as SDRAM_A15 */
    PORTC_PCR8=PORT_PCR_MUX(5);

    /* PORTC9 (pin C9) is configured as SDRAM_A14 */
    PORTC_PCR9=PORT_PCR_MUX(5);

    /* PORTC10 (pin A8) is configured as SDRAM_A13 */
    PORTC_PCR10=PORT_PCR_MUX(5);

    /* PORTD2 (pin A4) is configured as SDRAM_A12 */
    PORTD_PCR2=PORT_PCR_MUX(5);

    /* PORTD3 (pin B4) is configured as SDRAM_A11 */
    PORTD_PCR3=PORT_PCR_MUX(5);

    /* PORTD4 (pin B5) is configured as SDRAM_A10 */
    PORTD_PCR4=PORT_PCR_MUX(5);

    /* PORTD5 (pin C4) is configured as SDRAM_A9 */
    PORTD_PCR5=PORT_PCR_MUX(5);


    /* PORTB20 (pin D10) is configured as SDRAM_D31 */
    PORTB_PCR20=PORT_PCR_MUX(5);

    /* PORTB21 (pin D9) is configured as SDRAM_D30 */
    PORTB_PCR21=PORT_PCR_MUX(5);

    /* PORTB22 (pin C13) is configured as SDRAM_D29 */
    PORTB_PCR22=PORT_PCR_MUX(5);

    /* PORTB23 (pin C12) is configured as SDRAM_D28 */
    PORTB_PCR23=PORT_PCR_MUX(5);

    /* PORTC12 (pin B9) is configured as SDRAM_D27 */
    PORTC_PCR12=PORT_PCR_MUX(5);

    /* PORTC13 (pin B8) is configured as SDRAM_D26 */
    PORTC_PCR13=PORT_PCR_MUX(5);

    /* PORTC14 (pin C8) is configured as SDRAM_D25 */
    PORTC_PCR14=PORT_PCR_MUX(5);

    /* PORTC15 (pin D8) is configured as SDRAM_D24 */
    PORTC_PCR15=PORT_PCR_MUX(5);

    /* PORTB6 (pin F12) is configured as SDRAM_D23 */
    PORTB_PCR6=PORT_PCR_MUX(5);

    /* PORTB7 (pin F11) is configured as SDRAM_D22 */
    PORTB_PCR7=PORT_PCR_MUX(5);

    /* PORTB8 (pin F10) is configured as SDRAM_D21 */
    PORTB_PCR8=PORT_PCR_MUX(5);

    /* PORTB9 (pin F9) is configured as SDRAM_D20 */
    PORTB_PCR9=PORT_PCR_MUX(5);

    /* PORTB10 (pin G9) is configured as SDRAM_D19 */
    PORTB_PCR10=PORT_PCR_MUX(5);

    /* PORTB11 (pin E13) is configured as SDRAM_D18 */
    PORTB_PCR11=PORT_PCR_MUX(5);

    /* PORTB16 (pin F8) is configured as SDRAM_D17 */
    PORTB_PCR16=PORT_PCR_MUX(5);

    /* PORTB17 (pin D13) is configured as SDRAM_D16 */
    PORTB_PCR17=PORT_PCR_MUX(5);

    /* PORTE16 (pin H3) is configured as UART2_TX */
    PORTE_PCR16=PORT_PCR_MUX(3);

    /* PORTE17 (pin F5) is configured as UART2_RX */
    PORTE_PCR17=PORT_PCR_MUX(3);
}
