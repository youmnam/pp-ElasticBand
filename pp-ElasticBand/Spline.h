#pragma once
#include <vector>
using namespace std;

struct cubicFun{
	double a,b,c,d;
	double range_min;
	double range_max;
	cubicFun(double a1,double b1,double c1, double d1, double min, double max){
		a = a1;
		b = b1;
		c = c1;
		d = d1;
		range_min = min;
		range_max = max;
	}

};
struct Point{
	double x;
	double y;
	Point(int xx,int yy)
	{
		x = xx;
		y = yy;
	}

};
class Spline{
	public:
		vector<Point> p;
		void interpolate();
		double evaluateY(double xto);
		Spline();
	private:
		vector<vector<double> > m;
		vector<cubicFun> functions;
		vector<double> sloveSystem();

		void processPoints();
		void sort();
};
