/*
 * ElasticBand.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: youmna
 */
#include "ElasticBand.h"
#include <iostream>
#include <cmath>
using namespace std;

void ElasticBand:: initializeElastic(pair_i start,pair_i end,int nparticles, int ir)
{
	_nparticles= nparticles;
	_initradius=ir;

	getAllParticles(_fullelastic,start,end);
	int stepsz = _fullelastic.size()/nparticles;

	//Initialize elastic array
	_elastic.push_back(vector<Bubble>());
	_elastic.push_back(vector<Bubble>());
	for(int i=stepsz; i<_fullelastic.size(); i+=stepsz)
	{
		_elastic.push_back(vector<Bubble>());
	}

	//fill elastic array of equally spaced particles that connect start and ends.
	int j=0;
	_elastic[j++].push_back(Bubble(start,_initradius));
	for(int i=stepsz; i<_fullelastic.size(); i+=stepsz)
	{
		_elastic[j++].push_back(Bubble(_fullelastic[i],_initradius));
	}
	_elastic[j].push_back(Bubble(end,_initradius));

}

void ElasticBand:: deformElastic()
{
	for(int i =1; i<_elastic.size()-2 ; i++)
	{
		respondToobstacles(_elastic[i-1],i);
		respondToobstacles(_elastic[i],i);
		respondToobstacles(_elastic[i+1],i+1);

		//applyContractionForce(i);
		//applyMotionConstraint(_elastic[i]);
		/*if(!isBubbleIntersect(_elastic[i][_elastic[i].size()-1],_elastic[i+1][_elastic[i+1].size()-1])){
			cout<< " i=" << i <<" before :" << _elastic.size() <<endl;
			insertParticles(i,i+1);
			cout<< " i=" << i <<" after :" << _elastic.size() <<endl;

			for(int j =i; j<i+5 ; j++){
				cout << _elastic[j][_elastic[j].size()-1]._x <<"  ^ " << _elastic[j][_elastic[j].size()-1]._y  <<endl;
			}
			cout << i <<endl;
			//cin >> i ;
		}*/
	}
}

void ElasticBand::applyContractionForce(int i)
{
	Bubble &qim1  = _elastic[i-1][_elastic[i-1].size()-1];
	Bubble &qi    = _elastic[i][_elastic[i].size()-1];
	Bubble &qi1   = _elastic[i+1][_elastic[i+1].size()-1];

	float d1     = sqrt(pow((qi1._x - qi._x ),2.0)+pow((qi1._y - qi._y),2.0));
	float d2     = sqrt(pow((qi._x - qim1._x ),2.0)+pow((qi._y - qim1._y),2.0));

	float kc = 1;
	float fx = kc * ((qi1._x - qi._x / d1) + (qim1._x - qi._x / d2));
	float fy = kc * ((qi1._y - qi._y /d1) + (qim1._y - qi._y / d2));

	qi._x = qi._x+ 1* fx ;
	qi._y = qi._y+ 1* fy;

}
void ElasticBand::insertParticles(int s,int e)
{
	Bubble startb = _elastic[s][_elastic[s].size()-1];
	Bubble endb   = _elastic[e][_elastic[e].size()-1];
	vector<pair_i> toinsert;
	vector<vector<Bubble> >  subelastic;
	getAllParticles(toinsert,make_pair(startb._x,startb._y),make_pair(endb._x,endb._y));
	int stepsz = toinsert.size()/_nparticles;

	int c=0;
	if(stepsz ==0) stepsz = 20;

	for(int i =1; i<toinsert.size() ; i+=stepsz)
	{
		vector<Bubble> v;
		v.push_back(Bubble(make_pair(toinsert[i].first,toinsert[i].second),_initradius));
		cout << "puttting :" << toinsert[i].first <<" "<< toinsert[i].second <<endl;
		subelastic.push_back(v);
		c++;
	}

	_elastic.insert(_elastic.begin()+s+1,subelastic.begin(),subelastic.end());

	for(int i =s; i<=s+c+1; i++)
	{
		cout << _elastic[i][_elastic[i].size()-1]._x <<"  & " << _elastic[i][_elastic[i].size()-1]._y  <<endl;
	}


}

void ElasticBand::respondToobstacles(vector<Bubble> &currparticle,int i)
{
	for(int j=0; j<_obstacle.size(); j++)
	{
		int k=currparticle.size()-1;
		while(isBubbleIntersect(currparticle[k],_obstacle[j]))
		{
			 cout << "Intersection" <<endl;
			 moveParticle(currparticle[k], _obstacle[j], i);
			 k++;
		}
	}
	int k=currparticle.size()-1;
	while(k!=0)
	{
		k=currparticle.size()-1;
		int c=0;
		for(int j=0; j<_obstacle.size(); j++)
		{
			if(!isBubbleIntersect(currparticle[k],_obstacle[j]))
			{
				c++;
			}
		}
		if(c==_obstacle.size()-1)
		{
			currparticle.erase(currparticle.end());
			k--;
		}else{
			break;
		}
	}
}
void ElasticBand:: moveParticle(Bubble b1, Bubble b2,int i)
{
	int di = b1._y - b2._y;
	int dj = b1._x - b2._x;


	if(di>0)   //i+1  bottom intersection
	{
		b1._y = b1._y + _initradius;
	}
	else if(di<0)   //i-1 up intersection
	{
		b1._y = b1._y - _initradius;
	}
	else if(dj< 0) // j+1 left intersection
	{
		b1._x = b1._x -  _initradius;
	}else if (dj> 0) //j-1 right intersection
	{
		b1._x = b1._x + _initradius;
	}
	_elastic[i].push_back(Bubble(make_pair(b1._x,b1._y),b1._r));
}

bool ElasticBand:: isBubbleIntersect(Bubble b1, Bubble b2)
{
	/* First calculate the distance d between the center of the circles. d = ||P1 - P0||.
	  - If d > r0 + r1 then there are no solutions, the circles are separate.
	  - If d < |r0 - r1| then there are no solutions because one circle is contained within the other.
	  - If d = 0 and r0 = r1 then the circles are coincident and there are an infinite number of solutions.
	  ---http://paulbourke.net/geometry/circlesphere/
	*/
	float d = sqrt(pow((b1._x - b2._x ),2.0)+pow((b1._y - b2._y),2.0));
	if( d > b1._r + b2._r)
	{
	  //no intersect
		return false;
	}
	else /*if(d < abs(b1._r - b2._r))
	{
	 //intersect
	  return true;
	}
	else if ( d ==0 && b1._r == b2._r){
	 //intersect
	  return true;
	}*/
	return true;
}

void ElasticBand::getAllParticles(vector<pair_i> &elasticv,pair_i point1,pair_i point2){

	int x0 = point1.first;
	int y0 = point1.second;
	int x1 = point2.first;
	int y1 = point2.second;

	int dx = x1-x0;
	int dy = y1-y0;
	int D = 2*dy - dx;
    int &y=y0; int c=0;

    if(x0<x1)
    {
		for(; x0< x1; x0++)
		{
			//cout<< "line points:" << x0 <<"," <<y <<";" <<endl;
			elasticv.push_back(make_pair(x0,y));
			if (D > 0){
			   y = y + 1;
			   D = D - 2*dx;
			}
			D = D + 2*dy;
			//cin >> c;
		}
    }
    if(x0>x1)
    {
		for(;  x1< x0; x1++)
		{
			//cout<< "line points:" << x1 <<"," <<y <<";" <<endl;
			elasticv.push_back(make_pair(x1,y));
			if (D < 0){
			   y = y + 1;
			   D = D - 2*dx;
			}
			D = D + 2*dy;
			//cin >> c;
		}
    } //x0 ==x1
    if(x0 == x1){
    	int i=0,j=0;
		if(y0>y1)
		{
			i=y1;
			j=y0;
		}
		else
		{
			i=y0;
			j=y1;
		}
		for(;i<j;i++)
		{
			//cout<< "line points:" << x0 <<"," <<i <<";" <<endl;
			elasticv.push_back(make_pair(x0,i));
		}

    }

}

