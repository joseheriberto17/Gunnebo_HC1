#include <asf.h>

#define PHASE_A_PIN PIO_PB11_IDX
#define PHASE_B_PIN PIO_PB13_IDX
#define INDEX_PIN   PIO_PB14_IDX  // Pin para la señal de confirmación

volatile int position = 0;
volatile uint8_t last_state = 0;
volatile bool giroEnProgreso = false;
volatile bool giroCompletado = false;

void configure_pins(void) {
    pio_configure_pin(PHASE_A_PIN, PIO_INPUT | PIO_PULLUP);
    pio_configure_pin(PHASE_B_PIN, PIO_INPUT | PIO_PULLUP);
    pio_configure_pin(INDEX_PIN, PIO_INPUT | PIO_PULLUP);

    pio_handler_set(PIOB, ID_PIOB, PHASE_A_PIN | PHASE_B_PIN | INDEX_PIN, PIO_IT_EDGE, handle_phase_change);
    pio_enable_interrupt(PIOB, PHASE_A_PIN | PHASE_B_PIN | INDEX_PIN);
    NVIC_EnableIRQ(PIOB_IRQn);
}

void handle_phase_change(const uint32_t id, const uint32_t mask) {
    if (id == ID_PIOB) {
        uint8_t new_state = (pio_get(PIOB, PIO_TYPE_PIO_INPUT, PHASE_A_PIN) ? 1 : 0) |
                            (pio_get(PIOB, PIO_TYPE_PIO_INPUT, PHASE_B_PIN) ? 2 : 0);

        // Actualizar la posición basada en los cambios de estado
        if (new_state != last_state) {
            if ((last_state == 0b01 && new_state == 0b11) ||
                (last_state == 0b11 && new_state == 0b10) ||
                (last_state == 0b10 && new_state == 0b00) ||
                (last_state == 0b00 && new_state == 0b01)) {
                position++; // Movimiento en un sentido
            } else if ((last_state == 0b01 && new_state == 0b00) ||
                       (last_state == 0b00 && new_state == 0b10) ||
                       (last_state == 0b10 && new_state == 0b11) ||
                       (last_state == 0b11 && new_state == 0b01)) {
                position--; // Movimiento en el sentido contrario
            }
            last_state = new_state;
            giroEnProgreso = true;
        }

        // Chequear si el sensor de posición inicial se activa
        if (mask & (1 << INDEX_PIN)) {
            if (giroEnProgreso && !giroCompletado) {
                // Si el giro se devuelve antes de completarse
                giroEnProgreso = false;
                position = 0; // Resetear la posición si el giro no se completa
            }
            giroCompletado = false;
        }
    }
}

int main(void) {
    sysclk_init();
    board_init();
    configure_pins();

    while (1) {
        if (position != 0 && !giroCompletado) {
            // Procesar la posición actual, asumiendo que el giro no se ha completado
        }
        if (giroCompletado) {
            // Acciones a realizar si el giro se completa exitosamente
        }
    }

    return 0;
}

void PIOB_Handler(void) {
    uint32_t status = pio_get_interrupt_status(PIOB);
    handle_phase_change(ID_PIOB, status);
}