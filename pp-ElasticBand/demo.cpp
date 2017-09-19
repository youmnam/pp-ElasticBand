#include <cmath>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include "ElasticBand.h"
#include "Spline.h"


using namespace std;

//The matrix that hold the window drawing objects and path
cv::Mat space_image = cv::Mat(500,700, CV_8UC3,cv::Scalar(255,255,255));

//elastic band object
ElasticBand elasticB;


// to control the click actions on the window
enum clickState{START_POINT,END_POINT,LINE,OBS_POINT};
clickState cState=START_POINT;

// start and end point of the local path
int globalx1=-1; int globaly1=-1;
int globalx2=-1; int globaly2=-1;

// Drawing Functions //
void drawLine( cv::Mat img, cv::Point start, cv::Point end, cv::Scalar sc );
void drawCircle( cv::Mat img, cv::Point center ,float rad, int r,int g, int b);
void drawFilledCircle( cv::Mat img, cv::Point center ,float rad, int r,int g, int b);
vector<pair_i> getSplinePoints(int x1,int x2);

//the callback function when click on mouse event happen
static void onMouse( int event, int x, int y, int, void* );

//Main program
int main(int argc, char **argv)
{
	char space_window[] = "Elastic Band";

	while(true){
	cv::imshow( space_window, space_image );
	cv::setMouseCallback( space_window, onMouse, 0 );
	cv::waitKey(1000);
	}
}



static void onMouse( int event, int x, int y, int, void* )
{
    if( event == cv::EVENT_LBUTTONDOWN )
    {
		if(cState == START_POINT)
		{
			globalx1 = x;
			globaly1 = y;
			cout << x << " " << y <<endl;
			drawFilledCircle(space_image,cv::Point(globalx1,globaly1),8,255,0,0);
			cState=END_POINT;
		}
		else if(cState == END_POINT)
		{
			globalx2 = x;
			globaly2 = y;
			drawFilledCircle(space_image,cv::Point(globalx2,globaly2),8,0,0,255);
			cout << x << " " << y <<endl;
			cState=LINE;
		}
		else if(cState == OBS_POINT)
		{
			int obsrad = 10;
			elasticB._obstacle.push_back(Bubble(make_pair(x,y),obsrad));
			cout << x << " " << y <<endl;
			space_image = cv::Mat(500,700, CV_8UC3,cv::Scalar(255,255,255));

			elasticB.deformElastic();
			drawFilledCircle(space_image,cv::Point(globalx1,globaly1),8,255,0,0);
			drawFilledCircle(space_image,cv::Point(globalx2,globaly2),8,0,0,255);
			for(int i=0 ;i<elasticB._obstacle.size(); i++)
			{
				drawFilledCircle(space_image,cv::Point(elasticB._obstacle[i]._x,elasticB._obstacle[i]._y),obsrad,0,200,25);
			}
			vector<pair_i> curve =getSplinePoints(globalx1,globalx2);
			for(int i=0 ;i<curve.size(); i++)
			{
				drawFilledCircle(space_image,cv::Point(curve[i].first,curve[i].second),4,255,0,0);
			}


			for(int i=0 ;i<elasticB._elastic.size()-1; i++)
			{
				int bi= elasticB._elastic[i].size()-1;

				int ex1 = elasticB._elastic[i][bi]._x;
				int ey1 = elasticB._elastic[i][bi]._y;
				int er1 = elasticB._elastic[i][bi]._r;

				bi= elasticB._elastic[i+1].size()-1;

				int ex2 = elasticB._elastic[i+1][bi]._x;
				int ey2 = elasticB._elastic[i+1][bi]._y;
				int er2 = elasticB._elastic[i+1][bi]._r;
			//	drawCircle(space_image,cv::Point(ex1,ey1),er1,255,0,0);
			//	drawCircle(space_image,cv::Point(ex2,ey2),er2,255,0,0);
				drawLine(space_image,cv::Point(ex1,ey1),cv::Point(ex2,ey2),cv::Scalar(0,0,0));
			}
			//elasticB._obstacle.clear();
			cState=OBS_POINT;
		}
		if(cState == LINE )
		{
			//drawLine(space_image,cv::Point(globalx1,globaly1),cv::Point(globalx2,globaly2),cv::Scalar(0,0,0));
			cState=OBS_POINT;
			//start of elastic band creation
			//create set of particles based on the start and end information and number of particles to handel
			elasticB.initializeElastic(make_pair(globalx1,globaly1),make_pair(globalx2,globaly2),15,15);
			vector<pair_i> curve =getSplinePoints(globalx1,globalx2);

			for(int i=0 ;i<curve.size(); i++)
			{
				drawFilledCircle(space_image,cv::Point(curve[i].first,curve[i].second),4,255,0,0);
			}

			for(int i=0 ;i<elasticB._elastic.size()-1; i++)
			{
				int bi= elasticB._elastic[i].size()-1;
				cout << "bi=" << bi<< endl;
				int ex1 = elasticB._elastic[i][bi]._x;
				int ey1 = elasticB._elastic[i][bi]._y;
				int er1 = elasticB._elastic[i][bi]._r;

				bi= elasticB._elastic[i+1].size()-1;
				int ex2 = elasticB._elastic[i+1][bi]._x;
				int ey2 = elasticB._elastic[i+1][bi]._y;
				int er2 = elasticB._elastic[i+1][bi]._r;
				//drawCircle(space_image,cv::Point(ex1,ey1),er1,255,0,0);
				//drawCircle(space_image,cv::Point(ex2,ey2),er2,255,0,0);
				drawLine(space_image,cv::Point(ex1,ey1),cv::Point(ex2,ey2),cv::Scalar(0,0,0));
			}



		}
    }

    if( event == cv::EVENT_RBUTTONDOWN )
	{
	}
}


vector<pair_i> getSplinePoints(int x1,int x2)
{
	int start = 0;
	int end=0;
	if(x1<x2)
	{
		start = x1;
		end = x2;
	}
	else{
		start = x2;
		end = x1;
	}
	 // Create a clamped spline of degree 3 in 2D consisting of 7 control points.
	int nctrp =  elasticB._elastic.size();
	Spline s;

	for(int i = 0; i< nctrp ; i++)
    {
    	int bi= elasticB._elastic[i].size()-1;
    	s.p.push_back(Point(elasticB._elastic[i][bi]._x, elasticB._elastic[i][bi]._y));
    	cout << elasticB._elastic[i][bi]._x << " "<< elasticB._elastic[i][bi]._y <<endl;
    }


      s.interpolate();    // X needs to be sorted, strictly increasing

      vector<pair_i> curve;
      	    for(int k = start; k<=end ; k++)
      	    {
      	    	double value=s.evaluateY((double)k);
      	    	cout << k << " ----> " << value <<endl;
      			curve.push_back(make_pair(k,value));
      	    }
      	    return curve;
	/*ts::BSpline spline(3, 2, nctrp , TS_CLAMPED);

	    // Setup the control points.
	    std::vector<ts::rational> ctrlp = spline.ctrlp();


	    spline.setCtrlp(ctrlp);

	    vector<pair_i> curve;
	    for(int k = start; k<=end ; k++)
	    {
			std::vector<ts::rational> result = spline.evaluate(k).result();
			std::cout << "x = " << result[0] << ", y = " << result[1] << std::endl;
			curve.push_back(make_pair(result[0],result[1]));
	    }
	    return curve;*/
}

void  drawLine( cv::Mat img, cv::Point start, cv::Point end, cv::Scalar sc )
{
	  int thickness = 2;
	  int lineType = 8;
	  cv::line( img,
	    start,
	    end,
	    sc,
	    thickness,
	    lineType );
}
void drawFilledCircle( cv::Mat img, cv::Point center ,float rad, int r,int g, int b )
{
	  int thickness = -1;
	  int lineType = 8;

	  cv::circle( img,
	      center,
	      rad,
	      cv::Scalar( b, g, r ),
	      thickness,
	      lineType );
}

void drawCircle( cv::Mat img, cv::Point center ,float rad, int r,int g, int b )
{
	  int thickness = 1;
	  int lineType = 8;

	  cv::circle( img,
	      center,
	      rad,
	      cv::Scalar( b, g, r ),
	      thickness,
	      lineType );
}

