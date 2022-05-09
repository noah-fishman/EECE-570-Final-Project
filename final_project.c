#include "RGBLED_PWM.h"
#include "copwdt.h"
#include "systick.h"
#include "tsi0.h"
#include "sw1.h"
#include <stdint.h>
#include <stdbool.h>

#define THRESHOLD 0x002B

enum tsi_press {NO_PRESS,LEFT,RIGHT};
enum press {NOT_PRESS,PRESS};
enum tsi_press tsi_press_location(uint16_t res);
enum press button_press();
void fade_colors();

void main() {
	uint16_t tsi_result = 0;
	enum tsi_press location;
	enum press button;
	_Bool direction = 0; // 0 is left, 1 is right
	_Bool button_pressed = 0;
	asm("CPSID i");
	configure_copwdt();
	configure_systick();
	configure_rgbled();
	configure_tsi0();
	configure_sw1();
	asm("CPSIE i");

	while(1) {
		asm("WFI");
		if (!systick_has_fired())
			continue;

		if (tsi0_scan_is_complete())
			tsi_result = tsi0_get_scan_result();
		if (!tsi0_scan_is_in_progress())
			tsi0_start_scan(10);
		if (tsi_result < 40)
			tsi_result = 0;

		location = tsi_press_location(tsi_result);
		button = button_press();
		if (location == LEFT)
			direction = 0;
		else if (location == RIGHT)
			direction = 1;
			
		if (direction == 0)
			turn_off_rgbled();
		else
			fade_colors();		


		feed_the_watchdog();
	}
}

enum tsi_press tsi_press_location(uint16_t res) {
	
	static uint8_t count;
	static enum {NOT_PRESSED,DEB_PRESS,RIGHT_PRESS,
		LEFT_PRESS,DEB_RELEASE} state = NOT_PRESSED;
	switch(state) {
		default:
		case NOT_PRESSED:
			count = 0;
			if (res == 0) {
				state = DEB_PRESS;
				count = 1;
			}
			else
				state = NOT_PRESSED;
			break;

		case DEB_PRESS:
			if (count < 10)
				state = DEB_PRESS;
			else {
				if (res < THRESHOLD)
					state = RIGHT_PRESS;
				//else if ((res >= 0x00AF) && (res < 0x0FFF))
				//	state = MIDDLE_PRESS;
				else
					state = LEFT_PRESS;
				count = 1;
			}
			count++;
			break;

		case RIGHT_PRESS:
			if (res == 0)
				state = DEB_RELEASE;
			else {
				if (res >= THRESHOLD)
					state = LEFT_PRESS;
				else {
					state = RIGHT_PRESS;
					return RIGHT;
				}
			}
			break;

		case LEFT_PRESS:
			if (res == 0)
				state = DEB_RELEASE;
			else {
				if (res < THRESHOLD)
					state = RIGHT_PRESS;
				else {
					state = LEFT_PRESS;
					return LEFT;
				}
			}
			break;

		case DEB_RELEASE:
			if (count < 10)
				state = DEB_RELEASE;
			else {
				state = NOT_PRESSED;
				count = 0;
			}
			count++;
			break;
	}
	return NO_PRESS;
}

void fade_colors() {
	
	static uint16_t count;
	static uint16_t r_duty = 0;
	static uint16_t g_duty = 0;
	static uint16_t b_duty = 0;
	static uint16_t duty_cycle;

	static enum {START,FADE_RED,FADE_GREEN,FADE_BLUE} state = START;
	switch(state) {
		//default:
		case START:
			r_duty = 0xFFFF;
			state = FADE_RED;

		case FADE_RED:
			count++;
			r_duty -= 13;
			g_duty += 13;
			set_rgb_color_to(r_duty,g_duty,0);
			if (count >= 5000) {
				state = FADE_GREEN;
				count = 0;
			}
			else
				state = FADE_RED;
			break;

		case FADE_GREEN:
			count++;
			g_duty -= 13;
			b_duty += 13;
			set_rgb_color_to(0,g_duty,b_duty);
			if (count >= 5000) {
				state = FADE_BLUE;
				count = 0;
			}
			else
				state = FADE_GREEN;
			break;

		case FADE_BLUE:
			count++;
			b_duty -= 13;
			r_duty += 13;
			set_rgb_color_to(r_duty,0,b_duty);
			if (count >= 5000) {
				state = FADE_RED;
				count = 0;
			}
			else
				state = FADE_BLUE;
			break;

	}
}

enum press button_press() {

	static uint16_t lengthCount;

	static enum {NOT_PRESSED,DEBOUNCE_PRESS,PRESSED,DEBOUNCE_RELEASE} state = NOT_PRESSED;
	switch(state) {
		//default:
		case NOT_PRESSED:
			lengthCount = 0;
			if (sw1_is_pressed()) {
				state = DEBOUNCE_PRESS;
				lengthCount = 1;
			}
			else
				state = NOT_PRESSED;
			break;

		case DEBOUNCE_PRESS:
			if (sw1_is_pressed() && (lengthCount < 5))
				state = DEBOUNCE_PRESS;
			else if (sw1_is_pressed() && (lengthCount >= 5))
				state = PRESSED;
			else if (sw1_is_not_pressed())
				state = NOT_PRESSED;
			lengthCount++;
			break;

		case PRESSED:
			lengthCount = 0;
			if (sw1_is_pressed())
				state = PRESSED;
			else if (sw1_is_not_pressed()) {
				state = DEBOUNCE_RELEASE;
				return PRESS;
			}
			break;

		case DEBOUNCE_RELEASE:
			lengthCount++;
			if (lengthCount < 5)
				state = DEBOUNCE_RELEASE;
			else if (sw1_is_not_pressed() && (lengthCount >= 5)) {
				state = NOT_PRESSED;
			}
			break;
	}
	return NOT_PRESS;
}
