//#include <asf.h>
//
//#define PHASE_A_PIN        PIO_PB11_IDX
//#define PHASE_A_PIN_MASK   PIO_PB11
//
//#define PHASE_B_PIN        PIO_PB13_IDX
//#define PHASE_B_PIN_MASK   PIO_PB13
//
//#define LED_SENS_S_PIN     PIO_PB0_IDX
//#define LED_SENS_S_MASK    PIO_PB0
//
//#define LED_SENS_I_PIN     PIO_PB1_IDX
//#define LED_SENS_I_MASK    PIO_PB1
//
//static void configure_pins(void) {
//pmc_enable_periph_clk(ID_PIOB);
//
//// Configurar PHASE_A y PHASE_B como entradas
//pio_configure_pin(PHASE_A_PIN, PIO_INPUT | PIO_DEFAULT);
//pio_configure_pin(PHASE_B_PIN, PIO_INPUT | PIO_DEFAULT);
//
//// Configurar LED_SENS_S y LED_SENS_I como salidas
//pio_configure_pin(LED_SENS_S_PIN, PIO_OUTPUT_0 | PIO_DEFAULT);
//pio_configure_pin(LED_SENS_I_PIN, PIO_OUTPUT_0 | PIO_DEFAULT);
//}
//
//int main(void) {
//sysclk_init();
//board_init();
//configure_pins();
//
//while (1) {
//uint8_t phase_a = pio_get(PIOB, PIO_INPUT, PHASE_A_PIN_MASK);
//uint8_t phase_b = pio_get(PIOB, PIO_INPUT, PHASE_B_PIN_MASK);
//
//// LED_SENS_S reacciona a PHASE_B
//if (phase_b) {
//pio_set(PIOB, LED_SENS_S_MASK);
//} else {
//pio_clear(PIOB, LED_SENS_S_MASK);
//}
//
//// LED_SENS_I reacciona a PHASE_A
//if (phase_a) {
//pio_set(PIOB, LED_SENS_I_MASK);
//} else {
//pio_clear(PIOB, LED_SENS_I_MASK);
//}
//
//delay_ms(10);  // Pequeño retardo para evitar rebotes
//}
//}
//
//
#include <asf.h>

void configure_led(void);
void pin_interrupt_handler(const uint32_t id, const uint32_t mask);

// Handler que será llamado cuando ocurra la interrupción
void pin_interrupt_handler(const uint32_t id, const uint32_t mask) {
	if (id == ID_PIOB && (mask & PIO_PB11)) {
		if (pio_get(PIOB,PIO_INPUT,PIO_PB11)){
			pio_set(PIOB,PIO_PB0);
			} else {
			pio_clear(PIOB,PIO_PB0);
		}
	}
	if (id == ID_PIOB && (mask & PIO_PB13)) {
		if (pio_get(PIOB,PIO_INPUT,PIO_PB13)){
			pio_set(PIOB,PIO_PB1);
			} else {
			pio_clear(PIOB,PIO_PB1);
		}
	}
}

void configure_led(void) {
	// Activar el reloj del periférico PIOB
	pmc_enable_periph_clk(ID_PIOB);

	// Salidas GPIO
	pio_configure(PIOB, PIO_OUTPUT_0, PIO_PB0|PIO_PB1 , PIO_DEFAULT);
	
	// Entrada GPIO
	pio_configure(PIOB,PIO_INPUT,PIO_PB11|PIO_PB13,PIO_DEFAULT);
	pio_configure_interrupt(PIOB, PIO_PB11|PIO_PB13, PIO_IT_EDGE);
	pio_handler_set(PIOB, ID_PIOB, PIO_PB11|PIO_PB13, PIO_IT_EDGE, pin_interrupt_handler);
	
	// Habilitar la interrupción en el periférico y en el NVIC
	pio_enable_interrupt(PIOB, PIO_PB11|PIO_PB13);
	NVIC_EnableIRQ(PIOB_IRQn);
}

int main(void) {
	sysclk_init();
	board_init();
	configure_led();
	
	while (1){
		
	}
}