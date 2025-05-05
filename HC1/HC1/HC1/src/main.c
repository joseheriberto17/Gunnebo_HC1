#include <asf.h>

//pines de entrada
#define PHASE_A_PIN PIO_PB11_IDX
#define PHASE_A_PIN_MASK PIO_PB11
#define PHASE_A_PIN_PORT PIOB

#define PHASE_B_PIN PIO_PB13_IDX
#define PHASE_B_PIN_MASK PIO_PB13
#define PHASE_B_PIN_PORT PIOB

#define INDEX_PIN PIO_PB14_IDX
#define INDEX_PIN_MASK PIO_PB14
#define INDEX_PIN_PORT PIOB


//pines de salida
#define SEN_I_PIN PIO_PB0_IDX
#define SEN_I_PIN_MASK PIO_PB0
#define SEN_I_PIN_PORT PIOB

#define SEN_S_PIN PIO_PB1_IDX
#define SEN_S_PIN_MASK PIO_PB1
#define SEN_S_PIN_PORT PIOB

#define PIC_1_PIN PIO_PA8_IDX
#define PIC_1_PIN_MASK PIO_PA8
#define PIC_1_PIN_PORT PIOA



// variables globales
volatile int position = 0;
volatile uint8_t last_state = 0;
volatile bool new_dir = false;
volatile bool last_dir = false;
volatile bool error_dir = false;
volatile bool end_pase = false;
volatile uint8_t pase = 0;

// prototipos de funciones
void configure_pins(void);
void handle_phase_change(const uint32_t id, const uint32_t mask);
void handle_index(const uint32_t id, const uint32_t mask);

// Handler llamado por fase A o fase B, encoder tipo X4
void handle_phase_change(const uint32_t id, const uint32_t mask) {
	if (id == ID_PIOB) {
		if (mask & (PHASE_A_PIN_MASK|PHASE_B_PIN_MASK)){
			uint8_t new_state = (pio_get(PHASE_A_PIN_PORT, PIO_INPUT, PHASE_A_PIN_MASK) ? 1 : 0) |
			(pio_get(PHASE_A_PIN_PORT, PIO_INPUT, PHASE_B_PIN_MASK) ? 2 : 0);
			
			end_pase = pio_get_pin_value(INDEX_PIN);
			
			// condicion de paso completo
			if (end_pase){
				if(!error_dir && (abs(position) > 40)){
					pio_toggle_pin(PIC_1_PIN);
					position = 0;
				}
				error_dir = false;
				last_dir = new_dir;
			}
			if (!error_dir){				
				// Actualizar la posición basada en los cambios de estado
				if (new_state != last_state) {
					if ((last_state == 0b01 && new_state == 0b11) ||
					(last_state == 0b11 && new_state == 0b10) ||
					(last_state == 0b10 && new_state == 0b00) ||
					(last_state == 0b00 && new_state == 0b01)) {
						if(!end_pase){
							pio_set(SEN_S_PIN_PORT,SEN_S_PIN_MASK);
							pio_clear(SEN_I_PIN_PORT,SEN_I_PIN_MASK);
							} else {
							pio_clear(SEN_S_PIN_PORT,SEN_S_PIN_MASK);
							pio_clear(SEN_I_PIN_PORT,SEN_I_PIN_MASK);
						}
						new_dir = true;
						position++; // Movimiento en un sentido
					} else if ((last_state == 0b01 && new_state == 0b00) ||
					(last_state == 0b00 && new_state == 0b10) ||
					(last_state == 0b10 && new_state == 0b11) ||
					(last_state == 0b11 && new_state == 0b01)) {
						if(!end_pase){
							pio_set(SEN_I_PIN_PORT,SEN_I_PIN_MASK);
							pio_clear(SEN_S_PIN_PORT,SEN_S_PIN_MASK);
							} else {
							pio_clear(SEN_S_PIN_PORT,SEN_S_PIN_MASK);
							pio_clear(SEN_I_PIN_PORT,SEN_I_PIN_MASK);
						}
						new_dir = false;
						position--; // Movimiento en el sentido contrario
					}
					
					// condicion cuando se devuelve a la direccion correcta
					if ((new_dir != last_dir) && !end_pase)  {
						error_dir = true;
						pio_set(SEN_S_PIN_PORT,SEN_S_PIN_MASK);
						pio_set(SEN_I_PIN_PORT,SEN_I_PIN_MASK);
					}
					
					last_state = new_state;
				}
			}
		}
	}
}

void configure_pins(void) {
	// Activar el reloj del periférico PIOB
	pmc_enable_periph_clk(ID_PIOB);

	// Salidas GPIO
	pio_configure(SEN_I_PIN_PORT, PIO_OUTPUT_0, SEN_I_PIN_MASK , PIO_DEFAULT);
	pio_configure(SEN_S_PIN_PORT, PIO_OUTPUT_0, SEN_S_PIN_MASK , PIO_DEFAULT);
	pio_configure(PIC_1_PIN_PORT, PIO_OUTPUT_0, PIC_1_PIN_MASK , PIO_DEFAULT);

	
	// Entrada GPIO
	pio_configure(PHASE_A_PIN_PORT,PIO_INPUT,PHASE_A_PIN_MASK,PIO_DEFAULT);
	pio_configure(PHASE_B_PIN_PORT,PIO_INPUT,PHASE_B_PIN_MASK,PIO_DEFAULT);
	
	
	pio_configure_interrupt(PHASE_A_PIN_PORT,PHASE_A_PIN_MASK, PIO_IT_EDGE);
	pio_configure_interrupt(PHASE_B_PIN_PORT,PHASE_B_PIN_MASK, PIO_IT_EDGE);
	pio_configure_interrupt(INDEX_PIN_PORT,INDEX_PIN_MASK, PIO_IT_EDGE);
	pio_handler_set(PIOB, ID_PIOB, PHASE_A_PIN_MASK|PHASE_B_PIN_MASK|INDEX_PIN_MASK, PIO_IT_EDGE, handle_phase_change);
	// Habilitar la interrupción en el periférico y en el NVIC
	pio_enable_interrupt(PIOB, PHASE_A_PIN_MASK|PHASE_B_PIN_MASK|INDEX_PIN_MASK);
	NVIC_EnableIRQ(PIOB_IRQn);
}

int main(void) {
	sysclk_init();
	board_init();
	configure_pins();
	
	while (1){
		
		
	}
}