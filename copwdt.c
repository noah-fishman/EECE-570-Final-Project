// Noah Fishman
// EECE 558 - Programming Assignment 1
// copwdt driver

#include <copwdt.h>
#include <MKL25Z4.h>

void configure_copwdt() {
	SIM->COPC = SIM_COPC_COPT(3) |
		SIM_COPC_COPCLKS(1) |
		SIM_COPC_COPW(0);
}

void feed_the_watchdog() {
	SIM->SRVCOP = 0x55;
	SIM->SRVCOP = 0xAA;
}
