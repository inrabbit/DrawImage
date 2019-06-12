#ifndef DEF_POINT
#define DEF_POINT

template <class T>
struct Point
{
	T x, y;
	Point() {}
	Point(T _x, T _y) : x(_x), y(_y) {}
	Point operator+(Point p) const {
		return Point(x + p.x, y + p.y);
	}
	Point operator-(Point p) const {
		return Point(x - p.x, y - p.y);
	}
	Point operator*(T a) const {
		return Point(a * x, a * y);
	}
	Point operator/(T a) const {
		return Point(x / a, y * a);
	}
	Point& operator+=(Point p){
		x += p.x;
		y += p.y;
		return *this;
	}
	Point& operator-=(Point p){
		x -= p.x;
		y -= p.y;
		return *this;
	}
	T norm() const { return x * x + y * y; }
};

typedef Point<float> FPoint;
typedef Point<double> DPoint;

#endif
