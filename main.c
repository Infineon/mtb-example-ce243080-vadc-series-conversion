/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the PWM-Triggered VADC Series
*              Conversion Template for ModusToolbox.
*
* Related Document: See README.md
*
*******************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/


#include "cybsp.h"
#include "cy_utils.h"
#include "cy_retarget_io.h"

/*******************************************************************************
* Global Variables
*******************************************************************************/
Cy_VADC_RESULT_SIZE_t adc_result_G0Ch0 = 0;
Cy_VADC_RESULT_SIZE_t adc_result_G0Ch1 = 0;
Cy_VADC_RESULT_SIZE_t adc_result_G0Ch2 = 0;

/*******************************************************************************
* Function Name: NVIC_Config
********************************************************************************
* Summary:
* Configures the NVIC for the VADC Group 0 queue complete interrupt (IRQ17).
* Sets the interrupt priority and enables the IRQ.
*
* Parameters:
*  none
*
* Return:
*  none
*
*******************************************************************************/
void NVIC_Config(void)
{
    NVIC_SetPriority(IRQ17_IRQn, 1u);
    NVIC_EnableIRQ(IRQ17_IRQn);
}


/*******************************************************************************
* Function Name: IRQ17_Handler
********************************************************************************
* Summary:
* Interrupt handler for the VADC Group 0 queue complete event (IRQ17). Reads
* conversion results for all three channels and clears the request-source
* event flag to allow retriggering on the next PWM trigger.
*
* Parameters:
*  none
*
* Return:
*  none
*
*******************************************************************************/
void IRQ17_Handler(void)
{
    adc_result_G0Ch0 = Cy_VADC_GROUP_GetResult(VADC_G0, 0);
    adc_result_G0Ch1 = Cy_VADC_GROUP_GetResult(VADC_G0, 1);
    adc_result_G0Ch2 = Cy_VADC_GROUP_GetResult(VADC_G0, 2);

    /* Acknowledge interrupt and re-arm the queue trigger. */
    Cy_VADC_GROUP_QueueClearReqSrcEvent(vadc_0_group_0_HW);
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* Entry point of the application. Initializes board peripherals, configures
* the VADC group, sets up interrupt routing, configures the NVIC, initializes
* the debug UART, and enters the main loop to print ADC channel results.
*
* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* VADC group init and power-up must be done explicitly after cybsp_init(). */
    Cy_VADC_GROUP_Init(vadc_0_group_0_HW, &vadc_0_group0_init_config);
    Cy_VADC_GROUP_InputClassInit(vadc_0_group_0_HW, vadc_0_0_iclass_0, CY_VADC_GROUP_CONV_STD, 0U);
    Cy_VADC_GROUP_InputClassInit(vadc_0_group_0_HW, vadc_0_0_iclass_1, CY_VADC_GROUP_CONV_STD, 1U);
    Cy_VADC_GROUP_SetPowerMode(vadc_0_group_0_HW, CY_VADC_GROUP_POWERMODE_NORMAL);

    /* Route queue complete event to Group SR0 */
    Cy_VADC_GROUP_QueueSetReqSrcEventInterruptNode(vadc_0_group_0_HW, (Cy_VADC_SR_t) CY_VADC_SR_GROUP_SR0);

    /* Map VADC Group 0 SR0 to NVIC IRQ17 */
    Cy_SCU_SetInterruptControl(IRQ17_IRQn, CY_SCU_IRQCTRL_VADC0_G0SR0_IRQ17);

    NVIC_Config();

    cy_retarget_io_init(CYBSP_DEBUG_UART_HW);

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    printf("**PSOC Control C1: VADC series conversion**\r\n\n");

    for (;;)
    {
        printf("G0 channel 0: %d, G0 channel 1: %d, G0 channel 2: %d \r\n", adc_result_G0Ch0, adc_result_G0Ch1, adc_result_G0Ch2);
        Cy_Delay(100);
    }
}

/* [] END OF FILE */
