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
#include <cmath> // std::sin std::cos

#include "matrix.hpp"
#include "canvas.hpp"
#include "input.hpp"
#include "renderer.hpp"

int main(int argc, char const *argv[]) {
	Input kbd("/dev/input/by-id/usb-SONiX_USB_Keyboard-event-kbd");
	Renderer renderer(1.3, 100);

	while (true) {
		auto time = std::chrono::high_resolution_clock::now();
		kbd.poll();

		if (kbd.key_states[KEY_W])
			renderer.camera.pos.y -= 5;
		if (kbd.key_states[KEY_A])
			renderer.camera.pos.x -= 5;
		if (kbd.key_states[KEY_S])
			renderer.camera.pos.y += 5;
		if (kbd.key_states[KEY_D])
			renderer.camera.pos.x += 5;
		if (kbd.key_states[KEY_LEFT])
			renderer.camera.dir.rot(0.05);
		if (kbd.key_states[KEY_RIGHT])
			renderer.camera.dir.rot(-0.05);
		// if (kbd.key_states[KEY_U])
		if (kbd.key_states[KEY_Q])
			break;

		renderer.cx.clear();
		renderer.render();
		renderer.cx.update();

		time += std::chrono::milliseconds(5); // How often it should run, this will be improved later
		std::this_thread::sleep_until(time);
	}

	return 0;
}
