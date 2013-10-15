#include <stdlib.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "errno.h"
#include "utils.h"

#include "dmx.h"
#include "pin.h"
#include "pwm.h"

#ifdef DMX_UART_ID
#    if !CAT2(U2X, DMX_UART_ID)
#        error "The defined DMX_UART_ID is invalid"
#    endif
#else
#    if U2X2
#        define DMX_UART_ID	2
#    elif U2X1
#        define DMX_UART_ID	1
#    elif U2X0
#        define DMX_UART_ID	0
#    elif U2X
#        define DMX_UART_ID
#    else
#        error "No UART can be used"
#    endif
#endif

#define Rx(n, x)	CAT3(n, DMX_UART_ID, x)
#define R(n)		CAT2(n, DMX_UART_ID)

// The configured dmx address
static uint16_t dmx_address = 0;
// The number of channels we listen to
static uint8_t dmx_channel_count = 0;

void dmx_init(uint16_t address, uint8_t count)
{
    // Set the channels config
    dmx_address = address;
    dmx_channel_count = count;

    // Configure the receive pin
    pin_direction_input(DMX_RECV_PORT, DMX_RECV_PIN, 1);
    // Setup the serial port for 8n2 at 250kbps
# define BAUD 250000
# include <util/setbaud.h>
    Rx(UBRR,H) = UBRRH_VALUE;
    Rx(UBRR,L) = UBRRL_VALUE;
#if USE_2X
    Rx(UCSR,A) |= BIT(R(U2X));
#else
    Rx(UCSR,A) &= ~BIT(R(U2X));
#endif
    Rx(UCSR,C) = (1<<R(USBS)) | (3<<Rx(UCSZ,0));
    // Enable the receiver with interrupts
    Rx(UCSR,B) = BIT(R(RXCIE)) | BIT(R(RXEN));
}

void __attribute__((weak))
dmx_received_channel(uint16_t channel, uint8_t data)
{}

void __attribute__((weak))
dmx_receive_start(void)
{}

ISR(Rx(USART, _RX_vect))
{
    static uint16_t channel = 0xFFFF;
    uint8_t uart_state = Rx(UCSR,A);
    uint8_t data = R(UDR);

    // If there was an overrun re sync
    if (uart_state & BIT(R(DOR))) {
        channel = 0xFFFF;
        return;
    }

    // Assume that frame error are due to start of frame break
    if (uart_state & BIT(R(FE))) {
        channel = 0;
        return;
    }

    // Not synced yet
    if(channel == 0xFFFF)
        return;

    // Synced, however we only support packet type 0
    if (channel == 0) {
        if (data != 0 || dmx_address == 0) {
            channel = 0xFFFF;
            return;
        }
        // Notify the user code
        dmx_receive_start();
        channel = 1;
        return;
    }

    // Receiving some data, call the user code if that's one our channel
    if (channel >= dmx_address && channel < dmx_address + dmx_channel_count)
        dmx_received_channel(channel, data);

    channel += 1;
}
