// Noah Fishman
// EECE 558 - Programming Assignment 3
// RGB LED PWM Driver

#include <stdint.h>
#include <RGBLED_PWM.h>
#include <MKL25Z4.h>

#define RED_LED_LOC 18
#define GREEN_LED_LOC 19
#define BLUE_LED_LOC 1

void set_red_led_duty_cycle(uint16_t duty);
void set_green_led_duty_cycle(uint16_t duty);
void set_blue_led_duty_cycle(uint16_t duty);
void configure_rgbled();
void set_rgb_color_to( uint16_t red, uint16_t green, uint16_t blue);
void turn_off_rgbled();

void configure_rgbled()
{
	//configure red led port 
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[RED_LED_LOC] = PORT_PCR_MUX(3);
	//configure green led port
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[GREEN_LED_LOC] = PORT_PCR_MUX(3);
	//configure blue led port
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[1] = PORT_PCR_MUX(4);
	PTD->PDDR |= (1<<1);
	//configure red and green led timer
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
	TPM2->SC = TPM_SC_PS(0) |
		TPM_SC_CMOD(0) |
		TPM_SC_CPWMS(0);
	TPM2->MOD=0xFFFF;
	//configure blue led timer
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	TPM0->SC = TPM_SC_PS(0) |
		TPM_SC_CMOD(0) |
		TPM_SC_CPWMS(0);
	TPM0->MOD=0xFFFF;

	//configure red led channel
	TPM2->CONTROLS[0].CnSC =  TPM_CnSC_MSB(1)
		| TPM_CnSC_MSA(0)
		| TPM_CnSC_ELSB(0)
		// TPM_CnSC_CHF(1);
		// TPM_CnSC_CHIE(0);
		// TPM_CnSC_DMA(0);
		| TPM_CnSC_ELSA(1);
	//configure green led channel
	TPM2->CONTROLS[1].CnSC =  TPM_CnSC_MSB(1)
		| TPM_CnSC_MSA(0)
		| TPM_CnSC_ELSB(0)
		// TPM_CnSC_CHF(1);
		// TPM_CnSC_CHIE(0);
		// TPM_CnSC_DMA(0);
		| TPM_CnSC_ELSA(1);
	//configure blue led channel
	TPM0->CONTROLS[1].CnSC =  TPM_CnSC_MSB(1)
		| TPM_CnSC_MSA(0)
		| TPM_CnSC_ELSB(0)
		// TPM_CnSC_CHF(1);
		// TPM_CnSC_CHIE(0);
		// TPM_CnSC_DMA(0);
		| TPM_CnSC_ELSA(1);
	//turn off leds at config
	turn_off_rgbled();
	//turn on timer clocks
	TPM0->SC |= TPM_SC_CMOD(1);
	TPM2->SC |= TPM_SC_CMOD(1);
}

void set_rgb_color_to( uint16_t red, uint16_t green, uint16_t blue)
{
	set_red_led_duty_cycle(red);
	set_green_led_duty_cycle(green);
	set_blue_led_duty_cycle(blue);
}

void turn_off_rgbled()
{
	set_red_led_duty_cycle(0);
	set_green_led_duty_cycle(0);
	set_blue_led_duty_cycle(0);
}
void set_red_led_duty_cycle(uint16_t duty)
{
	TPM2->CONTROLS[0].CnV = duty;
}

void set_green_led_duty_cycle(uint16_t duty)
{
	TPM2->CONTROLS[1].CnV = duty;
}

void set_blue_led_duty_cycle(uint16_t duty)
{
	TPM0->CONTROLS[1].CnV = duty;
}
