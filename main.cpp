#pragma once

#include <stdio.h> // puts printf fprint
#include <stdlib.h> // EXIT_FAILURE
#include <string.h> // memcpy()
#include <unistd.h> // close()
#include <sys/ioctl.h> // ioctl()
#include <sys/mman.h> // mmap() PROT_READ
#include <linux/fb.h> // fb_, linux frame buffer
#include <fcntl.h> // open() O_RDWR
#include <errno.h> // errno
#include <linux/input.h> // struct input_event
#include <linux/input-event-codes.h> // macros for keys
#include <linux/kd.h> // KDSETMODE KD_GRAPHICS KD_TEXT

#include <iostream> // std::cout std::cerr std::endl
#include <chrono> // std::chrono::
#include <thread> // std::this_thread::
#include <bitset> // std::bitset

#include "canvas.hpp"
#include "input.hpp"

int main(int argc, char const *argv[]) {
	Canvas cx;
	Input kbd("/dev/input/by-id/usb-SONiX_USB_Keyboard-event-kbd");

	int x = 0;
	int y = 0;
	int x0 = 0;
	int y0 = 0;
	cx.clear();

	while (true) {
		auto time = std::chrono::high_resolution_clock::now();
		kbd.poll();

		if (kbd.key_states[KEY_W])
			y -= 10;
		if (kbd.key_states[KEY_A])
			x -= 10;
		if (kbd.key_states[KEY_S])
			y += 10;
		if (kbd.key_states[KEY_D])
			x += 10;
		if (kbd.key_states[KEY_U])
			cx.update();
		if (kbd.key_states[KEY_Q])
			break;

		// For wrapping and also because -1 % n == -1
		x = (cx.xres + x) % cx.xres;
		y = (cx.yres + y) % cx.yres;

		if (kbd.key_states[KEY_SPACE]) {
			cx.line(x0, y0, x, y, 0xFF0000);
			x0 = x;
			y0 = y;
		}

		cx.update();
		// cx.fb[x + y*cx.width] = 0xFF;

		time += std::chrono::milliseconds(100); // How often it should run
		std::this_thread::sleep_until(time);
	}

	return 0;
}
