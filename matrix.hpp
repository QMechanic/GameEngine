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

#include <cmath>
#include <iostream> // std::cout std::cerr std::endl

template <typename T>
class Vec2 {
public:
	T x, y;

	Vec2(T x_ = 0, T y_ = 0) {x = x_; y = y_;}
	T absSQ() {return x*x + y*y;}
	T dot(const Vec2<T> a) {return x*a.x + y*a.y;}
	T cross(const Vec2<T> a) {return x*a.y - y*a.x;}
	T abs() {return std::sqrt(x*x + y*y);}
	Vec2<T> copy() {return Vec2<T>(x, y);}
	void mul(T k) {x *= k; y *= k;}
	void div(T k) {x /= k; y /= k;}
	void add(const Vec2<T> a) {x += a.x; y += a.y;}
	void sub(const Vec2<T> a) {x -= a.x; y -= a.y;}
	void rot(const T ang) {T c = std::cos(ang), s = std::sin(ang), t = x; x = t*c - y*s; y = t*s + y*c;}
	void rot_fast(const T c, const T s) {T t = x; x = t*c - y*s; y = t*s + y*c;} // rotates vector with pre-calculated cosine and sine
	void print() {std::cout << "(" << x << ", " << y << ")" << std::endl;}
	// T () {return ;}
};

template <typename T> T dot(const Vec2<T> a, Vec2<T> b) {return a.x*b.x + a.y*b.y;}
template <typename T> T abs(const Vec2<T> a) {return std::sqrt(a.x*a.x + a.y*a.y);}
template <typename T> T cross(const Vec2<T> a, Vec2<T> b) {return a.x*b.y - a.y*b.x;}
template <typename T> T absSQ(const Vec2<T> a) {return a.x*a.x + a.y*a.y;}
template <typename T> Vec2<T> add(const Vec2<T> a, const Vec2<T> b) {return Vec2<T>(a.x + b.x, a.y + b.y);}
template <typename T> Vec2<T> sub(const Vec2<T> a, const Vec2<T> b) {return Vec2<T>(a.x - b.x, a.y - b.y);}
template <typename T> Vec2<T> mul(const T k, const Vec2<T> a) {return Vec2<T>(k*a.x, k*a.y);}


using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;

