
#ifndef LSH_ESE_COMPLEX_H_XIAOE
#define LSH_ESE_COMPLEX_H_XIAOE

class Complex {
	public:
		Complex(double _re, double _im);
		Complex();
		Complex& operator=(const Complex&);

		~Complex();

		Complex add(const Complex &b) const;
		Complex minus(const Complex &b) const;
		Complex times(const Complex &b) const;
		Complex divide(const Complex &b) const;

		Complex operator+(const Complex &b) const;
		Complex operator-(const Complex &b) const;
		Complex operator*(const Complex &b) const;
		Complex operator/(const Complex &b) const;

		Complex times(double c) const;

		//# Just return the conjugate.
		Complex conjugate() const;
		//# 1 / c.
		Complex reciprocal() const ;

		double real() const ;
		double imagin() const ;

		Complex exp() const;
		Complex sin() const;
		Complex cos() const;
		Complex tan() const;

	private:
		double re;
		double im;
		static const double eps = 1e-8;
};
#endif
