#include <asf.h>

#define PHASE_A_PIN PIO_PB11_IDX
#define PHASE_B_PIN PIO_PB13_IDX
#define INDEX_PIN   PIO_PB14_IDX  // Pin para la señal de confirmación

volatile int position = 0;
volatile uint8_t last_state = 0;
volatile bool giroEnProgreso = false;
volatile bool giroCompletado = false;

// Prototipo de la función de manejo de cambios de fase
void handle_phase_change(const uint32_t id, const uint32_t mask);

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

		if (new_state != last_state) {
			if ((last_state == 0b01 && new_state == 0b11) ||
			(last_state == 0b11 && new_state == 0b10) ||
			(last_state == 0b10 && new_state == 0b00) ||
			(last_state == 0b00 && new_state == 0b01)) {
				position++;
			} else if ((last_state == 0b01 && new_state == 0b00) ||
			(last_state == 0b00 && new_state == 0b10) ||
			(last_state == 0b10 && new_state == 0b11) ||
			(last_state == 0b11 && new_state == 0b01)) {
				position--;
			}
			last_state = new_state;
			giroEnProgreso = true;
		}

		if (mask & (1 << INDEX_PIN)) {
			if (giroEnProgreso && !giroCompletado) {
				giroCompletado = true;
				} else {
				position = 0;
				giroEnProgreso = false;
				giroCompletado = false;
			}
		}
	}
}

int main(void) {
	sysclk_init();
	board_init();
	configure_pins();

	while (1) {
		// Implementa cualquier otra lógica de supervisión necesaria
	}

	//return 1;
}

//void PIOB_Handler(void) {
	//uint32_t status = pio_get_interrupt_status(PIOB);
	//handle_phase_change(ID_PIOB, status);
//}