/**
 * @file main.c
 * @brief Aplikacija koja implementira ogranicen broj treptanja diode
 * @author Ime Prezime
 * @date dd-mm-yyyy
 * @version 1.0
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

/// Makro za podesavanje visoke vrednosti signala na pinu
#define HIGH 1
/// Makro za podesavanje niske vrednosti signala na pinu
#define LOW 0

/// Makro za podesavanje izlaznog smera pina
#define OUTPUT 1
/// Makro za podesavanje ulaznog smera pina
#define INPUT 0

/// Makro za selektovanje porta B
#define PORT_B 0
/// Makro za selektovanje porta C
#define PORT_C 1
/// Makro za selektovanje porta D
#define PORT_D 2

/// Makro za selektovanje pina na koji je povezana dioda
#define DIODE_PIN 5

/// Promenljiva koja skladisti broj milisekundi proteklih od pokretanja aplikacije
volatile uint32_t ms = 0;

/**
 * pinPulse - Funkcija koja implementiran podizanje i spustanje vrednosti na pinu
 * odgovarajucom brzinom
 * @param port - ulaz tipa uint8_t - Port na kojem je potrebno implementirati funkcionalnost
 * @param pin - ulaz tipa uint8_t - Pin na kojem je potrebno implementirati funkcionalnost
 * @param period - ulaz tipa uint32_t - Perioda promene vrednosti na pinu
 * @return Nema povratnu vrednost
 */
void pinPulse(uint8_t port, uint8_t pin, uint32_t period);

/**
 * pinSetValue - Funkcija koja postavlja vrednost na pinu
 * @param port - ulaz tipa uint8_t - Port na kojem je pin ciju vrednost potrebno postaviti
 * @param pin - ulaz tipa uint8_t - Pin ciju je vrednost potrebno postaviti
 * @param value - ulaz tipa uint8_t - Vrednost koju je potrebno postaviti na pin
 * @return Nema povratnu vrednost
 */
void pinSetValue(uint8_t port, uint8_t pin, uint8_t value);

/**
 * pinInit - Funkcija koja implementiran inicijalizaciju pina
 * @param port - ulaz tipa uint8_t - Port na kojem je pin koji je potrebno inicijalizovati
 * @param pin - ulaz tipa uint8_t - Pin koji je potrebno inicijalizovati
 * @param direction - ulaz tipa uint8_t - Smer prema kojem je potrebno inicijalizovati pin
 * @return Nema povratnu vrednost
 */
void pinInit(uint8_t port, uint8_t pin, uint8_t direction);

/**
 * timer0DelayMs - Funkcija koja implementira pauzu u broju milisekundi koji je prosledjen
 * kao parametar
 * @param delay_length - ulaz tipa uint32_t - Duzina pauze u milisekundama
 * @return Nema povratnu vrednost
 */
void timer0DelayMs(uint32_t delay_length);

/**
 * timer0Millis - Funkcija koja, na bezbedan nacin, vraca kao povratnu vrednost broj
 * milisekundi proteklih od pocetka merenja vremena
 * @return Broj milisekundi proteklih od pocetka merenja vremena
 */
uint32_t timer0Millis();

/**
 * timer0Init - Funkcija koja inicijalizuje timer 0 tako da pravi prekide
 * svake milisekunde
 * @return Nema povratnu vrednost
 */
void timer0Init();

/**
 * main - funkcija koja implementiran glavni deo aplikacije
 * @return Nema povratnu vrednost
 */
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

/**
 * ISR - prekidna rutina tajmera 0 u modu CTC
 */
ISR(TIMER0_COMPA_vect)
{
	// Inkrementovanje broja milisekundi koje su prosle od pokretanja aplikacije
	ms++;
}

