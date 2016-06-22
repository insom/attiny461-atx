#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define PIN_LED1 PB4
#define PIN_BUTTON1 PB3
#define PIN_BUTTON1_MASK (1 << PB3)

uint8_t toggle = 0;
uint8_t c = 0;
uint8_t power = 0;
uint8_t debounce = 0;
uint8_t a = 0;

#define OFF_STATE (1 << PIN_BUTTON1) | (1 << PIN_LED1)
#define ON_STATE (1 << PIN_BUTTON1)

ISR(TIMER0_OVF_vect) {
    if (debounce == 1) {
        if((PINB & PIN_BUTTON1_MASK) > 0) {
            debounce = 0;
        } else {
            goto end;
        }
    }
    if (power == 0) {
        if((PINB & PIN_BUTTON1_MASK) == 0) {
            // Power up!
            power = 1;
            PORTB = ON_STATE;
        }
    } else {
        // Count up consecutive periods that the button was held low.
        if((PINB & PIN_BUTTON1_MASK) == 0) {
            c++;
        } else {
            c = 0;
        }
        if(c == 22) {
            // Power down!
            power = 0;
            debounce = 1;
            PORTB = OFF_STATE; // Default state
        }
    }
end:
    a = 0;
}

int main(void) {
    PORTB = OFF_STATE; // Enable pull up.
    DDRB = (1 << PIN_LED1); // Output.

    // Timer registers
    TCCR0A = 0;
    TCCR0B = (1 << CS02) | (1 << CS00); // /1024 Prescaler.
    TIMSK = (1 << TOIE0);

    sei(); // Enable interrupts.

    // Now: do pointless work!

    for(;;) {
        a++;
    }

    return 0;
}
