#include "Spline.h"
#include <cmath>
#include <iostream>
using namespace std;


Spline::Spline()
{

}

void Spline:: interpolate()
{
	processPoints();
	int row = 0;
		int solutionIndex = (p.size() - 1) * 4;

		// initialize matrix
		m.resize(solutionIndex);// rows
		for (int i = 0; i < (p.size() - 1) * 4; i++)
		{
			for (int j = 0; j <= (p.size() - 1) * 4; j++)
			{
				m[i].push_back(0); // fill with zeros
			}
		}

		// splines through p equations
		for (int functionNr = 0; functionNr < p.size()-1; functionNr++, row += 2) {
			Point p0 = p[functionNr], p1 = p[functionNr+1];
			m[row][functionNr*4+0] = pow(p0.x, 3);
			m[row][functionNr*4+1] = pow(p0.x, 2);
			m[row][functionNr*4+2] = pow(p0.x, 1);
			m[row][functionNr*4+3] = 1;
			m[row][solutionIndex] = p0.y;
			m[row+1][(functionNr)*4+0] = pow(p1.x, 3);
			m[row+1][(functionNr)*4+1] = pow(p1.x, 2);
			m[row+1][(functionNr)*4+2] = pow(p1.x, 1);
			m[row+1][(functionNr)*4+3] = 1;
			m[row+1][solutionIndex] = p1.y;
		}

		// first derivative
		for (int functionNr = 0; functionNr < p.size() - 2; functionNr++, row++) {
			Point p1 = p[functionNr+1];
			m[row][functionNr*4+0] = 3*pow(p1.x, 2);
			m[row][functionNr*4+1] = 2*p1.x;
			m[row][functionNr*4+2] = 1;
			m[row][functionNr*4+4] = -3*pow(p1.x, 2);
			m[row][functionNr*4+5] = -2*p1.x;
			m[row][functionNr*4+6] = -1;
		}

		// second derivative
		for (int functionNr = 0; functionNr < p.size() - 2; functionNr++, row++) {
			Point p1 = p[functionNr+1];
			m[row][functionNr*4+0] = 6*p1.x;
			m[row][functionNr*4+1] = 2;
			m[row][functionNr*4+4] = -6*p1.x;
			m[row][functionNr*4+5] = -2;
		}

		// natural spline
			m[row][0+0] = 6*p[0].x;
			m[row++][0+1] = 2;
			m[row][solutionIndex-4+0] = 6*p[p.size()-1].x;
			m[row][solutionIndex-4+1] = 2;

			vector<double> coefficients = sloveSystem();
			for (int i = 0; i < coefficients.size(); i += 4)
			{
				functions.push_back(cubicFun(
					coefficients[i],
					coefficients[i+1],
					coefficients[i+2],
					coefficients[i+3],
					p[i/4].x, p[(i/4)+1].x ));
				cout <<  coefficients[i] << " "<<coefficients[i+1] << " [" << p[i/4].x <<"," << p[(i/4)+1].x  << "]"<<endl;
			}
			/**/
}

double Spline::evaluateY(double xto)
{
	for(int i =0; i<functions.size(); i++)
	{
		if(xto <= functions[i].range_max && xto >= functions[i].range_min)
			return functions[i].a*pow(xto,3.0)+functions[i].b*pow(xto,2.0)+functions[i].c*xto+ functions[i].d;
	}
	return 0;
}

vector<double> Spline::sloveSystem()
{
	  int n = m.size();

	    for (int i=0; i<n; i++) {
	        // Search for maximum in this column
	        double maxEl = abs(m[i][i]);
	        int maxRow = i;
	        for (int k=i+1; k<n; k++) {
	            if (abs(m[k][i]) > maxEl) {
	                maxEl = abs(m[k][i]);
	                maxRow = k;
	            }
	        }

	        // Swap maximum row with current row (column by column)
	        for (int k=i; k<n+1;k++) {
	            double tmp = m[maxRow][k];
	            m[maxRow][k] = m[i][k];
	            m[i][k] = tmp;
	        }

	        // Make all rows below this one 0 in current column
	        for (int k=i+1; k<n; k++) {
	            double c = -m[k][i]/m[i][i];
	            for (int j=i; j<n+1; j++) {
	                if (i==j) {
	                    m[k][j] = 0;
	                } else {
	                    m[k][j] += c * m[i][j];
	                }
	            }
	        }
	    }

	    // Solve equation Ax=b for an upper triangular matrix A
	    vector<double> x(n);
	    for (int i=n-1; i>=0; i--) {
	        x[i] = m[i][n]/m[i][i];
	        for (int k=i-1;k>=0; k--) {
	            m[k][n] -= m[k][i] * x[i];
	        }
	    }
	    return x;
}
void Spline::processPoints()
{
	for (int i = 0; i < p.size()-1; i++) {
		if (i < p.size() - 1 && p[i].x == p[i + 1].x) {
			// two points have the same x-value

			// check if the y-value is the same
			if (p[i].y == p[i + 1].y) {
				// remove the latter
				p.erase(p.begin()+i);
				i--;
			}
			else {
				cout << "SameXDifferentY" <<endl;
				return;
			}
		}
	}

	if (p.size() < 2) {
		cout << "NotEnoughPoints" <<endl;
		return;
	}
	sort();
}
void Spline::sort()
{
  for(int i=0; i<p.size(); i++)
  {
	for(int j=0; j<p.size(); j++)
	{
		if(p[i].x < p[j].x)
		{
			Point temp = p[i];
			p[i] = p[j];
			p[j] = temp;
		}
	 }
   }
}
