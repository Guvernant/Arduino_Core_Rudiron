#include "nrf24.h"


bool nRF24::begin(bool receiver, bool irq)
{
    nRF24_GPIO_Init();

    Rudiron::SPI::getSPI2().begin();

    //работа через прерывания
    if (irq){
        nRF24_InitIRQ();
    }

    // RX/TX disabled
    nRF24_CE_L;
    Rudiron::CLK::delay_millis(10);

    int repeat = 10;
    while (!nRF24_Check() && repeat > 0) {
        Rudiron::CLK::delay_millis(10);
        repeat--;
    }
    if (repeat <= 0){
        end();
        return false;
    }

    // Initialize the nRF24L01 to its default state
    nRF24_Init();

    nRF24_SetRFChannel(40);

    // Set data rate
    nRF24_SetDataRate(nRF24_DR_250kbps);

    // Set CRC scheme
    nRF24_SetCRCScheme(nRF24_CRC_2byte);

    // Set address width, its common for all pipes (RX and TX)
    nRF24_SetAddrWidth(5);

    // Configure RX PIPE
    static const uint8_t nRF24_ADDR[] = { 0xF0, 0xF0, 0xF0, 0xF0, 0xF0 };

    //Адрес трубы приемника
    nRF24_SetAddr(nRF24_PIPE0, nRF24_ADDR);


    if (receiver){
        nRF24_SetRXPipe(nRF24_PIPE0, nRF24_AA_ON, 32);

        nRF24_SetTXPower(nRF24_TXPWR_18dBm);

        // Set operational mode (PRX == receiver)
        nRF24_SetOperationalMode(nRF24_MODE_RX);

        // Clear any pending IRQ flags
        nRF24_ClearIRQFlags();

        // Wake the transceiver
        nRF24_SetPowerMode(nRF24_PWR_UP);

        // Put the transceiver to the RX mode
        nRF24_CE_H;
    }
    else{
        //Адрес трубы передатчика
        nRF24_SetAddr(nRF24_PIPETX, nRF24_ADDR);

        // Set TX power (maximum)
        nRF24_SetTXPower(nRF24_TXPWR_0dBm);

        // Configure auto retransmit: 10 retransmissions with pause of 2500s in between
        nRF24_SetAutoRetr(nRF24_ARD_2500us, 10);

        // Enable Auto-ACK for pipe#0 (for ACK packets)
        nRF24_EnableAA(nRF24_PIPE0);

        /// Set operational mode (PTX == transmitter)
        nRF24_SetOperationalMode(nRF24_MODE_TX);

        // Clear any pending IRQ flags
        nRF24_ClearIRQFlags();

        // Wake the transceiver
        nRF24_SetPowerMode(nRF24_PWR_UP);
    }

    return true;
}


void nRF24::end()
{
    Rudiron::SPI::getSPI2().end();
    nRF24_GPIO_DeInit();
    nRF24_DeInitIRQ();
}

bool nRF24::available()
{
    return nRF24_GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY;
}


nRF24_RXResult nRF24::read(uint8_t buffer[NRF24_PAYLOAD_LENGTH])
{
    // Pipe number
    nRF24_RXResult pipe = nRF24_RX_EMPTY;

    // Length of received payload
    uint8_t payload_length;
    if (nRF24_GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
        // Get a payload from the transceiver
        pipe = nRF24_ReadPayload(buffer, &payload_length);
    }

    return pipe;
}


nRF24_TXResult nRF24::write(uint8_t buffer[NRF24_PAYLOAD_LENGTH])
{
    uint8_t packets_lost = 0;
    nRF24_TXResult tx_res = nRF24_TransmitPacket(buffer, NRF24_PAYLOAD_LENGTH);
    uint8_t otx = nRF24_GetRetransmitCounters();
    uint8_t otx_plos_cnt = (otx & nRF24_MASK_PLOS_CNT) >> 4; // packets lost counter
    //uint8_t otx_arc_cnt  = (otx & nRF24_MASK_ARC_CNT); // auto retransmissions counter
    switch (tx_res) {
        case nRF24_TX_SUCCESS:
            //UART_SendStr("OK");
            break;
        case nRF24_TX_TIMEOUT:
            //UART_SendStr("TIMEOUT");
            break;
        case nRF24_TX_MAXRT:
            //UART_SendStr("MAX RETRANSMIT");
            packets_lost += otx_plos_cnt;
            nRF24_ResetPLOS();
            break;
        default:
            //UART_SendStr("ERROR");
            break;
    }

    return tx_res;
}
