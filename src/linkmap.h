#ifndef LINKMAP_H
#define LINKMAP_H
#include <map>
#include <vector>

using namespace std;

struct Data{
	double value;
	string key;
	bool is_ans;
};

struct PRF{
	double P;
	double R;
	double F;
	struct PRF& operator+=(const PRF& rhs) {P+=rhs.P; R+=rhs.R; F+=rhs.F; return *this;}

};

class Linkmap{
public:
	Linkmap(const char*,const vector<int>&);
	double find_thres();
	void linking(const double&);
	PRF global(const double& ,const float&,const float&);
	PRF evaluating();
	void printall();
private:
	const char * _type;
	//map<double,string> _linkscores;
	int _trainans;
	int _testans;
	vector< Data > _trainvec;
	vector< Data > _testvec;
	vector< Data > _evalvec;
};
#endif