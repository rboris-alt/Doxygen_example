#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define HIGH 1
#define LOW 0

#define OUTPUT 1
#define INPUT 0

#define PORT_B 0
#define PORT_C 1
#define PORT_D 2

#define DIODE_PIN 5

volatile uint32_t ms = 0;

void pinPulse(uint8_t port, uint8_t pin, uint32_t period);

void pinSetValue(uint8_t port, uint8_t pin, uint8_t value);

void pinInit(uint8_t port, uint8_t pin, uint8_t direction);

void timer0DelayMs(uint32_t delay_length);

uint32_t timer0Millis();

void timer0Init();


int16_t main() {
	uint32_t period = 1000; 	// Period jednog treptaja
	uint8_t repetitions = 5;    // Broj treptaja
	int16_t i;

	// Inicijalizacija
	pinInit( PORT_B, DIODE_PIN, OUTPUT);
	timer0Init();

	// Glavna petlja
	while (1)
	{
		// Treptanje
		for (i = 0; i < repetitions; i++)
			pinPulse( PORT_B, DIODE_PIN, period);

		// Kraj
		while (1)
			;
	}

	return 0;
}

/***********************************************************/

void pinPulse(uint8_t port, uint8_t pin, uint32_t period)
{
	// Poluperioda u kojoj pin ima visoku vrednost
	pinSetValue(port, pin, HIGH);
	timer0DelayMs(period / 2);

	// Poluperioda u kojoj pin ima nisku vrednost
	pinSetValue(port, pin, LOW);
	timer0DelayMs(period / 2);
}

/***********************************************************/

void pinSetValue(uint8_t port, uint8_t pin, uint8_t value)
{
	// Postavljanje vrednosti pina
	switch (port)
	{
		case PORT_B:
			if (value == HIGH)
				PORTB |= 1 << pin;
			else
				PORTB &= ~(1 << pin);
			break;

		case PORT_C:
			if (value == HIGH)
				PORTC |= 1 << pin;
			else
				PORTC &= ~(1 << pin);
			break;

		case PORT_D:
			if (value == HIGH)
				PORTD |= 1 << pin;
			else
				PORTD &= ~(1 << pin);
			break;

		default:
			break;
	}
}

/***********************************************************/

void pinInit(uint8_t port, uint8_t pin, uint8_t direction)
{
	// Inicijalizacija smera pina
	switch (port)
	{
		case PORT_B:
			if (direction == OUTPUT)
				DDRB |= 1 << pin;
			else
				DDRB &= ~(1 << pin);
			break;

		case PORT_C:
			if (direction == OUTPUT)
				DDRC |= 1 << pin;
			else
				DDRC &= ~(1 << pin);
			break;

		case PORT_D:
			if (direction == OUTPUT)
				DDRD |= 1 << pin;
			else
				DDRD &= ~(1 << pin);
			break;

		default:
			break;
	}
}

/***********************************************************/

void timer0DelayMs(uint32_t delay_length)
{
	// trenutak pocevsi od kog se racuna pauza
	uint32_t t0 = timer0Millis();
	// implementacija pauze
	while (timer0Millis() - t0 < delay_length)
		;
}

/***********************************************************/

uint32_t timer0Millis()
{
	uint32_t tmp;
	cli(); 			// Zabrana prekida
	tmp = ms; 		// Ocitavanje vremena
	sei(); 			// Dozvola prekida

	return tmp;
}

/***********************************************************/

void timer0Init()
{
	TCCR0A = 0x02; // tajmer 0: CTC mod

	// Provera frekvencije takta prilikom kompilacije
	#if F_CPU > 20000000
	#error "Frekvencija takta mora biti manja od 20 MHz!"
	#endif

	// Inicijalizacija promenljivih za preskaler i periodu tajmera 0
	uint32_t n = F_CPU / 1000;
	uint8_t clksel = 1;

	// Odredjivanje vrednosti preskalera i periode tajmera 0
	while (n > 255)
	{
		n /= 8;
		clksel++;
	}

	TCCR0B = clksel; 					// tajmer 0: Podesavanje preskalera
	OCR0A = (uint8_t)(n & 0xff) - 1;	// tajmer 0: Podesavanje periode
	TIMSK0 = 0x02;						// tajmer 0: Dozvola prekida
	sei();								// Globalna dozvola prekida
}

/***********************************************************/

ISR(TIMER0_COMPA_vect)
{
	// Inkrementovanje broja milisekundi koje su prosle od pokretanja aplikacije
	ms++;
}

