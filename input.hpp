#pragma once

#include <string.h> // memcpy()
#include <unistd.h> // close()
#include <fcntl.h> // open() O_RDONLY O_NONBLOCK
#include <errno.h> // errno
#include <linux/input.h> // struct input_event
#include <linux/input-event-codes.h> // macros for keys

#include <bitset> // std::bitset

class Input {
public:
	std::bitset<65536> key_states;
	struct input_event ev;
	ssize_t n;
	int fd;

	Input(const char *dev) {
		// Create file descriptor, and importantly, non-blocking
		fd = open(dev, O_RDONLY | O_NONBLOCK);
		if (fd == -1) {
			fprintf(stderr, "Cannot open %s: %s.\n", dev, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	~Input() {
		close(fd);
	}

	void poll(void) {
		n = read(fd, &ev, sizeof(ev));
		while (n != -1) {
			if (n != sizeof ev) {
				errno = EIO;
				fprintf(stderr, "I/O error when reading event device: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			if (ev.type == EV_KEY)
				key_states[ev.code] = ev.value != 0; // this can be changed
			n = read(fd, &ev, sizeof(ev));
		}
	}
};
