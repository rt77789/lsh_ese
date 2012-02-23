
#include "complex.h"
#include <cmath>

Complex::Complex():re(0), im(0) {
}

Complex::Complex(double _re, double _im):re(_re), im(_im) {
}

Complex&
Complex::operator=(const Complex &b) {
	re = b.real(), im = b.imagin();
	return *this;
}

Complex::~Complex() {
}

Complex
Complex::add(const Complex &b) const {
	return Complex(re + b.real(), im + b.imagin());
}

Complex
Complex::minus(const Complex &b) const {
	return Complex(re - b.real(), im - b.imagin());
}

Complex
Complex::times(const Complex &b) const {
	return Complex(re * b.real() - im * b.imagin(), re * b.imagin() + im * b.real());
}

Complex
Complex::divide(const Complex &b) const {
	return times(b.reciprocal());
}

Complex
Complex::operator+(const Complex &b) const {
	return add(b);
}

Complex
Complex::operator-(const Complex &b) const {
	return minus(b);
}

Complex
Complex::operator*(const Complex &b) const {
	return times(b);
}

Complex
Complex::operator/(const Complex &b) const {
	return divide(b);
}

Complex
Complex::times(double c) const {
	return Complex(re * c, im * c);
}

Complex
Complex::conjugate() const {
	return Complex(re, -im);
}

Complex
Complex::reciprocal() const {
	double scale = re * re + im * im;
	if(fabs(scale) <= eps) {
		throw;
	}
	return Complex(re / scale, -im / scale);
}

double
Complex::real() const {
	return re;
}
double
Complex::imagin() const {
	return im;
}

Complex
Complex::exp() const {
	double t1 = std::exp(re);
	return Complex(t1 * std::cos(im), std::exp(re) * std::sin(im));
}

Complex
Complex::sin() const {
	return Complex(std::sin(re) * std::cosh(im), std::cos(re) * std::sinh(im));
}

Complex
Complex::cos() const {
	return Complex(std::cos(re) * std::cosh(im), -std::sin(re) * std::sinh(im));
}

Complex
Complex::tan() const {
	return sin().divide(cos());
}
