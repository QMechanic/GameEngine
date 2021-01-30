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
#include <vector> // std::vector

#include "canvas.hpp"


struct Wall {
	Vec2f p1, p2;
};
struct Camera {
	Vec2f pos, dir;
};

class Renderer {
public:
	Canvas cx;
	Camera camera;
	int rays;
	float fov, ang; // In Radians
	std::vector<Wall> walls;

	Renderer(float fov, int rays_) {
		// ========= Init player pos =============== //
		camera.pos.x = 100; //cx.xres/2;
		camera.pos.y = 100; //cx.yres/2;
		camera.dir.x = 0;
		camera.dir.y = 1;

		// ========= Init world stuff =============== //
		walls.push_back(Wall{Vec2f(150, 300), Vec2f(400, 700)});
		walls.push_back(Wall{Vec2f(400, 700), Vec2f(600, 400)});
		walls.push_back(Wall{Vec2f(1000, 1000), Vec2f(1200, 700)});
		walls.push_back(Wall{Vec2f(300, 500), Vec2f(2000, 800)});

		// other stuff
		rays = rays_;
		ang = fov/(float)rays;
	}

	// given four end points of two lines it finds a 5th point where the two lines intersect
	Vec2<float> ray_cast(const Vec2f ro, const Vec2f rd, const Wall l) {
		// calculate the distance to intersection point
		float t1 = ro.y - l.p1.y;
		float t2 = l.p2.y - l.p1.y;
		float t3 = l.p2.x - l.p1.x;
		float t4 = ro.x - l.p1.x;
		float denom = t2*rd.x - t3*rd.y;
		if (denom == (float)0)
			return Vec2f(-1, -1);
		float uA = (t3*t1 - t2*t4) / denom;
		float uB = (rd.x*t1 - rd.y*t4) / denom;

		// if uA and uB are between 0-1, lines are colliding
		if (uA < 0 || uB < 0 || uB > 1) // add uA > 10 for specific render distance
			return Vec2f(-1, -1);
		return Vec2f(ro.x + uA*rd.x, ro.y + uA*rd.y);
	}

	void render() {
		for (int i = 0; i < walls.size(); ++i)
			cx.line(walls[i].p1.x, walls[i].p1.y, walls[i].p2.x, walls[i].p2.y, 0xFF);

		float c = std::cos(ang);
		float s = std::sin(ang);
		Vec2f rd = camera.dir.copy();
		rd.rot(-ang*rays*0.5);
		for (int r = 0; r < rays; ++r) {
			float min_distSQ = INFINITY; // render distance
			Vec2f min_p(-1, -1);
			for (int i = 0; i < walls.size(); ++i) {
				Vec2f p = ray_cast(camera.pos, rd, walls[i]);
				if (p.x == -1 && p.y == -1)
					continue;
				float distSQ = sub(camera.pos, p).absSQ();
				if (distSQ < min_distSQ) {
					min_distSQ = distSQ;
					min_p = p.copy();
				}
			}
			if (min_p.x != -1 && min_p.y != -1) { // Checks if it collided
				cx.line((int)camera.pos.x, (int)camera.pos.y, (int)min_p.x, (int)min_p.y, 0xFF0000);
				cx.set_pixel((int)min_p.x, (int)min_p.y, 0xFFFFFF);
			}
			rd.rot_fast(c, s);
		}
		cx.set_pixel((int)camera.pos.x, (int)camera.pos.y, 0xFFFFFF);
	}
};

