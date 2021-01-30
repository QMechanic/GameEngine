#pragma once

#include <stdio.h> // puts printf fprint
#include <stdlib.h> // EXIT_FAILURE
#include <string.h> // memcpy()
#include <unistd.h> // close()
#include <sys/ioctl.h> // ioctl()
#include <sys/mman.h> // mmap() PROT_READ
#include <linux/fb.h> // fb_, linux frame buffer
#include <fcntl.h> // open() O_RDWR
#include <linux/kd.h> // KDSETMODE KD_GRAPHICS KD_TEXT

#include <iostream> // std::cout std::cerr std::endl

#include "matrix.hpp"

// Context, class for handling everything for rendering and displaying the graphics
class Canvas {
public:
	// index = x + y*width
	uint32_t *fb, *buffer;
	int fb_fd, tty_fd, xres, yres, bytes_count, pixel_count, width;
	fb_var_screeninfo vinfo;
	fb_fix_screeninfo finfo;

	Canvas(void) {
		// Open file descriptor for frame buffer and get info
		fb_fd = open("/dev/fb0", O_RDWR);
		if (fb_fd == -1) {
			std::cerr << "Error: Cannot open frame buffer device" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
			std::cerr << "Error: cannot read fixed information" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
			std::cerr << "Error: cannot read variable information" << std::endl;
			exit(EXIT_FAILURE);
		}

		// ========================== Setting constants for frame buffer ========================== //
		// const uint32_t pixel_count = vinfo.xres * vinfo.yres;
		bytes_count = vinfo.yres * finfo.line_length; // this is slightly more, there are 16 dead pixels off the side
		pixel_count = bytes_count / sizeof(uint32_t);
		xres = vinfo.xres;
		yres = vinfo.yres;
		width = finfo.line_length / sizeof(uint32_t);

		// Get writable frame buffer memory
		fb = static_cast<uint32_t*>(mmap(0, bytes_count, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0));
		if (fb == MAP_FAILED) {
			close(fb_fd);
			std::cerr << "Error: mmap failed" << std::endl;
			exit(EXIT_FAILURE);
		}

		// Initializing buffer with memory
		buffer = new uint32_t[pixel_count];

		// Disabling so nothing can write text to the frame buffer
		tty_fd = open("/dev/tty2", O_RDWR);
		if (tty_fd == -1) {
			std::cerr << "Error: Cannot open tty2 file descriptor" << std::endl;
			exit(EXIT_FAILURE);
		}
		// if (ioctl(tty_fd, KDSETMODE, KD_GRAPHICS) == -1) {
		// 	std::cerr << "Error: cannot set graphics mode" << std::endl;
		// 	exit(EXIT_FAILURE);
		// }

		// ========================== Print Info about frame buffer ========================== //
		std::cout << "Fixed information:" << std::endl;
		std::cout << "id: " << finfo.id << std::endl;
		std::cout << "smem_start: " << finfo.smem_start << std::endl;
		std::cout << "smem_len: " << finfo.smem_len << std::endl;
		std::cout << "type: " << finfo.type << std::endl;
		std::cout << "type_aux: " << finfo.type_aux << std::endl;
		std::cout << "visual: " << finfo.visual << std::endl;
		std::cout << "xpanstep: " << finfo.xpanstep << std::endl;
		std::cout << "ypanstep: " << finfo.ypanstep << std::endl;
		std::cout << "ywrapstep: " << finfo.ywrapstep << std::endl;
		std::cout << "line_length: " << finfo.line_length << std::endl;
		std::cout << "mmio_start: " << finfo.mmio_start << std::endl;
		std::cout << "mmio_len: " << finfo.mmio_len << std::endl;
		std::cout << "accel: " << finfo.accel << std::endl;
		std::cout << "capabilities: " << finfo.capabilities << std::endl;
		// std::cout << ": " << finfo. << std::endl;

		std::cout << std::endl << "Variable information:" << std::endl;
		std::cout << "xres: " << vinfo.xres << std::endl;
		std::cout << "yres: " << vinfo.yres << std::endl;
		std::cout << "xres_virtual: " << vinfo.xres_virtual << std::endl;
		std::cout << "yres_virtual: " << vinfo.yres_virtual << std::endl;
		std::cout << "xoffset: " << vinfo.xoffset << std::endl;
		std::cout << "yoffset: " << vinfo.yoffset << std::endl;
		std::cout << "bits_per_pixel: " << vinfo.bits_per_pixel << std::endl;
		std::cout << "grayscale: " << vinfo.grayscale << std::endl;
		std::cout << "pixclock: " << vinfo.pixclock << std::endl;
		std::cout << "left_margin: " << vinfo.left_margin << std::endl;
		std::cout << "right_margin: " << vinfo.right_margin << std::endl;
		std::cout << "upper_margin: " << vinfo.upper_margin << std::endl;
		std::cout << "lower_margin: " << vinfo.lower_margin << std::endl;
		std::cout << "hsync_len: " << vinfo.hsync_len << std::endl;
		std::cout << "vsync_len: " << vinfo.vsync_len << std::endl;
		std::cout << "sync: " << vinfo.sync << std::endl;
		std::cout << "vmode: " << vinfo.vmode << std::endl;
		std::cout << "rotate: " << vinfo.rotate << std::endl;
		std::cout << "colorspace: " << vinfo.colorspace << std::endl;
		// std::cout << ": " << vinfo. << std::endl;

	}
	~Canvas() {
		// Resets to text mode, otherwise screen will be frozen
		ioctl(tty_fd, KDSETMODE, KD_TEXT);
		delete []buffer;
		close(fb_fd);
		close(tty_fd);
	}
	void update(void) {
		memcpy(fb, buffer, bytes_count);
	}
	void clear(void) {
		for (int i = 0; i < pixel_count; i++)
			buffer[i] = 0;
	}
	// Update this in future to take Vec2 input
	inline void set_pixel(int x, int y, uint32_t color) {
		x = (xres + (x % xres)) % xres;
		y = (yres + (y % yres)) % yres;
		if (x < 0 || x >= xres)
			throw "a";
		if (y < 0 || y >= yres)
			throw 1;
		buffer[x + y*width] = color;
	}
	// Update this in future to take Vec2 inputs
	void line(int x0, int y0, int x1, int y1, uint32_t color) {
		int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
		int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1; 
		int err = dx+dy, e2; /* error value e_xy */
	
		for(int i = 0; i < 10000; ++i){  /* loop */
			set_pixel(x0, y0, color);
			if (x0==x1 && y0==y1) break;
			e2 = 2*err;
			if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
			if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
		}
	}
};
