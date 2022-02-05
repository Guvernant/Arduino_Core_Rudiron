#include "uart.hpp"




bool UART::begin(uint32_t baudRate)
{
    PORT_InitTypeDef PortInit;
    UART_InitTypeDef UART_InitStructure;

    /* Enables the HSI clock on PORTB,PORTD */
    // RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB,ENABLE);

    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD,ENABLE);

    /* Fill PortInit structure*/
    PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;
    PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;
    PortInit.PORT_PD = PORT_PD_DRIVER;
    PortInit.PORT_GFEN = PORT_GFEN_OFF;
    PortInit.PORT_FUNC = PORT_FUNC_ALTER;
    PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
    PortInit.PORT_MODE = PORT_MODE_DIGITAL;

    // /* Configure PORTB pins 5 (UART1_TX) as output */
    // PortInit.PORT_OE = PORT_OE_OUT;
    // PortInit.PORT_Pin = PORT_Pin_5;
    // PORT_Init(MDR_PORTB, &PortInit);

    // /* Configure PORTB pins 6 (UART1_RX) as input */
    // PortInit.PORT_OE = PORT_OE_IN;
    // PortInit.PORT_Pin = PORT_Pin_6;
    // PORT_Init(MDR_PORTB, &PortInit);

    /* Configure PORTD pins 1 (UART2_TX) as output */
    PortInit.PORT_OE = PORT_OE_OUT;
    PortInit.PORT_Pin = PORT_Pin_1;
    PORT_Init(MDR_PORTD, &PortInit);

    /* Configure PORTD pins 0 (UART1_RX) as input */
    PortInit.PORT_OE = PORT_OE_IN;
    PortInit.PORT_Pin = PORT_Pin_0;
    PORT_Init(MDR_PORTD, &PortInit);

    // /* Enables the CPU_CLK clock on UART1,UART2 */
    // RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);

    // /* Set the HCLK division factor = 2 for UART1,UART2*/
    // UART_BRGInit(MDR_UART1, UART_HCLKdiv1);

    /* Enables the CPU_CLK clock on UART1,UART2 */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);

    /* Set the HCLK division factor = 2 for UART1,UART2*/
    UART_BRGInit(MDR_UART2, UART_HCLKdiv1);

    /* Initialize UART_InitStructure */
    UART_InitStructure.UART_BitRate                =  baudRate;
    UART_InitStructure.UART_WordLength              = UART_WordLength8b;
    UART_InitStructure.UART_StopBits                = UART_StopBits1;
    UART_InitStructure.UART_Parity                  = UART_Parity_No;
    UART_InitStructure.UART_FIFOMode                = UART_FIFO_OFF;
    UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;

    /* Configure UART1 parameters*/
    // bool status = UART_Init (MDR_UART1,&UART_InitStructure);

    bool status = UART_Init (MDR_UART2,&UART_InitStructure);

    if (status){
        /* Enables UART1 peripheral */
        // UART_Cmd(MDR_UART1,ENABLE);

        UART_Cmd(MDR_UART2,ENABLE);
    }

    return status;
}


void UART::end()
{
    UART_Cmd(MDR_UART1,DISABLE);
    RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, DISABLE);

    PORT_InitTypeDef PortInit;
    PortInit.PORT_Pin = PORT_Pin_5 | PORT_Pin_6;
    PORT_Init(MDR_PORTB, &PortInit);
}


int UART::available()
{
    /* Check RXFF flag*/
    // return UART_GetFlagStatus (MDR_UART1, UART_FLAG_RXFF);
    return UART_GetFlagStatus (MDR_UART2, UART_FLAG_RXFF);
}


int UART::peek()
{
    return read();
}


int UART::read()
{
    /* Check RXFF flag*/
    // while (UART_GetFlagStatus (MDR_UART1, UART_FLAG_RXFF)!= SET)
    // {
    // }

    // /* Recive data*/
    // return UART_ReceiveData (MDR_UART1);

    /* Check RXFF flag*/
    while (UART_GetFlagStatus (MDR_UART2, UART_FLAG_RXFF)!= SET)
    {
    }

    /* Recive data*/
    return UART_ReceiveData (MDR_UART2);
}


int UART::availableForWrite()
{
    // return UART_GetFlagStatus (MDR_UART1, UART_FLAG_TXFE);

    return UART_GetFlagStatus (MDR_UART2, UART_FLAG_TXFE);
}


void UART::flush()
{
    // while (UART_GetFlagStatus (MDR_UART1, UART_FLAG_TXFE)!= SET)
    // {
    // }

    while (UART_GetFlagStatus (MDR_UART2, UART_FLAG_TXFE)!= SET)
    {
    }
}


size_t UART::write(uint8_t byte){
    // /* Check TXFE flag*/
    // while (UART_GetFlagStatus (MDR_UART1, UART_FLAG_TXFE)!= SET)
    // {
    // }

    // /* Send Data from UART1 */
    // UART_SendData (MDR_UART1,(uint16_t)byte);

    // return true;

    /* Check TXFE flag*/
    while (UART_GetFlagStatus (MDR_UART2, UART_FLAG_TXFE)!= SET)
    {
    }

    /* Send Data from UART2 */
    UART_SendData (MDR_UART2,(uint16_t)byte);

    return true;
};
