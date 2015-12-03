#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <stdlib.h>
#include <limits>

#include "linkmap.h"

#define DEBUG_

bool operator< (const Data& a, const Data& b)
{
	return a.value<b.value;
}

PRF F_measure(const int& retri, const int& corr, const int& ans)
{
	PRF thePRF;
	if (corr==0)
	{
		thePRF.P = double(corr)/retri;
		thePRF.R = double(corr)/ans;
		thePRF.F = 0;
		return thePRF;
	}
	thePRF.P = double(corr)/retri;
	thePRF.R = double(corr)/ans;
	thePRF.F = 2*thePRF.P*thePRF.R/(thePRF.P+thePRF.R);
	return thePRF;
}

int cross_over(const string& link1 , const string& link2)
{
	char * cstr = new char [link1.length()+1];
	strcpy(cstr,link1.c_str());
		
	char* tok = strtok(cstr,"-");
	int l1c1 = atoi(tok);
		
	char* tok2=strtok(NULL,"-");
	int l1c2 = atoi(tok2);

	delete [] cstr;

	char * cstr2 = new char [link2.length()+1];
	strcpy(cstr2,link2.c_str());
		
	tok = strtok(cstr2,"-");
	int l2c1 = atoi(tok);
		
	tok2=strtok(NULL,"-");
	int l2c2 = atoi(tok2);

	delete [] cstr2;

	if( (l1c1>l2c1&&l1c2<l2c2) || (l1c1<l2c1&&l1c2>l2c2) )
		return abs(l1c1-l2c1)+abs(l1c2-l2c2);
	else
		return 0;
}

Linkmap::Linkmap(const char* type ,const vector<int>& randindex)
{
	_type=type;

	//Below for answer
	fstream file_ans;
	string that_line;
	int lineno=0;
	map<string,int> _answers;

	file_ans.open("ans",fstream::in);
	if (!file_ans.is_open())
		cerr<<"Ans file does not exist!!"<<endl;
	
	while (getline(file_ans,that_line))
	{
		//cout<<"  ans "<<that_line<<endl;
		_answers.insert(pair<string,int>(that_line,lineno));
		++lineno;
		//string test="0-0";
		//int cc = cross_over(that_line,test);
		//cout<<"cross is "<<cc<<endl;
	}
	file_ans.close();

	//Below for linking
	vector< Data > _scoresvec;

	fstream file;
	file.open(type,fstream::in);
	if (!file.is_open())
		cerr<<"File name does not exist!!"<<endl;

	while (getline(file,that_line))
	{
		char * cstr = new char [that_line.length()+1];
		strcpy(cstr,that_line.c_str());
		
		char* tok = strtok(cstr,"\t");
		string key(tok);
		
		char* tok2=strtok(NULL,"\t");
		double val=atof(tok2);

		bool is_ans=false;
		if (_answers.find(key)!=_answers.end())
			is_ans=true;

		Data temp;
		temp.value=val;
		temp.key=key;
		temp.is_ans=is_ans;

		_scoresvec.push_back(temp);
		
		delete [] cstr;
	}
	file.close();

	unsigned int half = _scoresvec.size()/2;
	
	int trainans=0;
	for(unsigned int i=0; i < half;++i)
	{
		if(_scoresvec[ randindex[i] ].is_ans==true)
			++trainans;
		_trainvec.push_back(_scoresvec[ randindex[i] ]);
	}
	_trainans=trainans;

	int testans=0;
	for(unsigned int i=half+1; i < _scoresvec.size();++i)
	{
		if(_scoresvec[ randindex[i] ].is_ans==true)
			++testans;
		_testvec.push_back(_scoresvec[ randindex[i] ]);
	}
	_testans=testans;

	//sort(_scoresvec.begin(),_scoresvec.end());
	sort(_trainvec.begin(),_trainvec.end());
	sort(_testvec.begin(),_testvec.end());
	
	


}

double Linkmap::find_thres()
{
	vector< pair<double,double> > all_F;
	double now_score;
	int retri=0;
	int corr=0;
	//vector< Data > copyvec = _scoresvec;
	vector< Data > copyvec = _trainvec;
	while(copyvec.size()!=0)
	{
		++retri;
		now_score = copyvec.back().value;
		if ( copyvec.back().is_ans==true )
			++corr;

		PRF thePRF = F_measure(retri,corr,_trainans);
#ifdef DEBUG		
		//cout<<"When "<<copyvec.back().key<<" F is "<<new_F<<endl;
#endif
		copyvec.pop_back();
	    all_F.push_back(pair<double,double>(thePRF.F,now_score));
	}

	
	sort(all_F.begin(),all_F.end());

	//cout<<"  Training F is "<<all_F[all_F.size()-1].first<<endl;

	return all_F[all_F.size()-1].second;
}

void Linkmap::linking(const double& thres)
{
	ofstream myout;
	string fuck("myout");
	myout.open((fuck+string(_type)).c_str());

	for(size_t i=_testvec.size()-1;i!=0;--i)
	{
		if(_testvec[i].value < thres)
			break;
		else
		{
			myout<<_testvec[i].key<<endl;
			_evalvec.push_back(_testvec[i]);
		}
	}
	
}

PRF Linkmap::global(const double& thres,const float& lambda1,const float& lambda2 )
{
	vector< Data > linkvec;
	for(size_t i=_testvec.size()-1;i!=0;--i)
	{
		if(_testvec[i].value < thres)
			break;
		else
		{
			//myout<<_tvec[i].key<<endl;
			linkvec.push_back(_testvec[i]);
		}
	}
	// F(L) = sigma(sim) - lambda1 * sig(cross) - lambda2 * (# of links)
	//int alllinks=0;
	int allcross=0;
	double allsim=0;
	double allscore=0;
	//double lambda1=0.01;
	//double lambda2=0.01;

	//init
	vector <Data> newLinkvec;
	newLinkvec.push_back(linkvec.back());
	//++alllinks;
	allsim = allsim + linkvec.back().value;
	allscore = allsim -lambda2*newLinkvec.size();
	linkvec.pop_back();

	while(linkvec.size()!=0)
	{
		int next_to_link=-1;
		double bestnewscore = - numeric_limits<double>::infinity();
		double bestnewscross = allcross;
		double bestnewsim = allsim;
		//Calculate F(L)
		for(size_t i=0;i!=linkvec.size();++i)
		{
			int newcross = allcross;
			//Calculate all cross
			for(size_t j=0;j!=newLinkvec.size();++j)
				newcross += cross_over(newLinkvec[j].key,linkvec[i].key);
			double newsim = allsim + linkvec[i].value;
			double newscore = newsim - lambda1*newcross - lambda2*(newLinkvec.size()+1);
			if (newscore>bestnewscore)
			{
				next_to_link=i;
				bestnewscore=newscore;
				bestnewscross=newcross;
				bestnewsim=newsim;
			}
		}
		if( bestnewscore > allscore)
		{
			allcross = bestnewscross;
			allscore = bestnewscore;
			allsim = bestnewsim;
			newLinkvec.push_back(linkvec[next_to_link]);
		}
		linkvec.erase(linkvec.begin()+next_to_link);
	}
	//cout<<"All sim is "<<allsim<<endl;
	//cout<<"All cross is "<<allcross<<endl;
	//cout<<"All link is "<<newLinkvec.size()<<endl;

	vector< Data > copyvec = newLinkvec;
	int retri=0;
	int corr=0;
	while(copyvec.size()!=0)
	{
		++retri;
		//now_score = copyvec.back().first;
		if (copyvec.back().is_ans==true)
			++corr;
		copyvec.pop_back();
	}

	ofstream myout;
	string fuck("myglobalout");
	myout.open((fuck+string(_type)).c_str());

	for(size_t i=0;i<newLinkvec.size();++i)
		myout<<newLinkvec[i].key<<endl;

	return F_measure(retri,corr,_testans);
	
	
}

PRF Linkmap::evaluating()
{
	vector< Data > copyvec = _evalvec;
	int retri=0;
	int corr=0;
	while(copyvec.size()!=0)
	{
		++retri;
		//now_score = copyvec.back().first;
		if (copyvec.back().is_ans==true)
			++corr;
		copyvec.pop_back();
	}

	return F_measure(retri,corr,_testans);
}

void Linkmap::printall()
{
	for(size_t i=_trainvec.size()-1;i!=0;--i)
	{
		//cout<<_trainvec[i].key<<"\t"<<_trainvec[i].value<<endl;
	}
	for(size_t i=_testvec.size()-1;i!=0;--i)
	{
		//cout<<_testvec[i].key<<"\t"<<_testvec[i].value<<endl;
	}
}