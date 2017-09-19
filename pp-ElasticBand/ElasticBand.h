#pragma once
#include<vector>
using namespace std;

typedef pair<float,float> pair_f;
typedef pair<float,float> pair_i;

struct Bubble{

	int _x;
	int _y;
	int _r;
	Bubble(pair_i p,int r){
		_x=p.first;
		_y=p.second;
		_r=r;
	}

};
class ElasticBand{

public:
	int                      _nparticles;
	vector<pair_i>           _fullelastic;
	vector<vector<Bubble> >  _elastic;
	vector<Bubble> 			 _obstacle;
	int                      _initradius;
	void initializeElastic(pair_i start,pair_i end,int nparticles, int bubblerd);
	void deformElastic();
private:
	void getAllParticles(vector<pair_i> &elasticv, pair_i point1,pair_i point2);
	bool isBubbleIntersect(Bubble b1, Bubble b2);
	void moveParticle(Bubble b1, Bubble b2,int i);
	void respondToobstacles(vector<Bubble> &currparticle,int i);
	void insertParticles(int s,int e);
	void applyContractionForce(int i);
};



