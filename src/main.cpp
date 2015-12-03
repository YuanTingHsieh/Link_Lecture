#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <vector>
#include "linkmap.h"

#define DEBUG_
#define LAMB1_SIZE 5
#define LAMB2_SIZE 5

using namespace std;


void printAverage(const PRF& pp, int total_sim)
{
	cout<<"  Average P is "<<pp.P/total_sim<<endl;
	cout<<"  Average R is "<<pp.R/total_sim<<endl;
	cout<<"  Average F is "<<pp.F/total_sim<<endl;
}

int main(int argc,char* argv[])
{
	if (argc!=1)
	{
		cerr<<"Usage: ./LinkCourse"<<endl;
		return -1;
	}

	int total_sim = 30;

	PRF allFscore1;
	PRF allFscore2;
	PRF allFscore3;
	PRF allFscore4;
	PRF allFscore5;
	PRF allFscore6;
	PRF* allFscore7[LAMB1_SIZE];
	for(int i=0;i<LAMB1_SIZE;++i)
		allFscore7[i] = new PRF[LAMB2_SIZE];

	const char* m1="tfidf.norm";
	const char* m2="tfidf.keyterm.norm";
	const char* m3="tfidf.title.norm";
	const char* m4="tfidf.title.grammer.norm";
	const char* m5="topic128.norm";
	const char* m6="combine";

	//float lamb1=atof(argv[1]);
	//float lamb2=atof(argv[2]);

	float lamb1[LAMB1_SIZE]={0.001,0.0015,0.002,0.0025,0.003};
	float lamb2[LAMB2_SIZE]={0,0.04,0.08,0.16,0.32};

	for(int sim_times=0;sim_times < total_sim;sim_times++)
	{
		vector<int > randindex;
		for(int i=0;i<12221;++i)
			randindex.push_back(i);

		srand(time(NULL));
		random_shuffle(randindex.begin(),randindex.end());

		//cout<<"Linking by "<<m1<<endl;
		Linkmap lm(m1,randindex);
		double thres = lm.find_thres();
		//cout<<"  The threshold is "<<thres<<endl;
		lm.linking(thres);
		////cout<<"  Evaluating F"<<endl;
		PRF nowscore = lm.evaluating();
		//cout<<"  The F is "<<Fscore<<endl;
		allFscore1+=nowscore;

		//cout<<"Linking by "<<m2<<endl;
		Linkmap lm2(m2,randindex);
		thres = lm2.find_thres();
		//cout<<"  The threshold is "<<thres<<endl;
		lm2.linking(thres);
		////cout<<"  Evaluating F"<<endl;
		nowscore = lm2.evaluating();
		//cout<<"  The F is "<<Fscore<<endl;
		allFscore2+=nowscore;

		//cout<<"Linking by "<<m3<<endl;
		Linkmap lm3(m3,randindex);
		thres = lm3.find_thres();
		//cout<<"  The threshold is "<<thres<<endl;
		lm3.linking(thres);
		////cout<<"  Evaluating F"<<endl;
		nowscore = lm3.evaluating();
		//cout<<"  The F is "<<Fscore<<endl;
		allFscore3+=nowscore;

		//cout<<"Linking by "<<m4<<endl;
		Linkmap lm4(m4,randindex);
		thres = lm4.find_thres();
		//cout<<"  The threshold is "<<thres<<endl;
		lm4.linking(thres);
		////cout<<"  Evaluating F"<<endl;
		nowscore = lm4.evaluating();
		//cout<<"  The F is "<<Fscore<<endl;
		allFscore4+=nowscore;

		//cout<<"Linking by "<<m5<<endl;
		Linkmap lm5(m5,randindex);
		thres = lm5.find_thres();
		//cout<<"  The threshold is "<<thres<<endl;
		lm5.linking(thres);
		////cout<<"  Evaluating F"<<endl;
		nowscore = lm5.evaluating();
		//cout<<"  The F is "<<Fscore<<endl;
		allFscore5+=nowscore;

		//cout<<"Linking by "<<m6<<endl;
		Linkmap lm6(m6,randindex);
		thres = lm6.find_thres();
		//cout<<"  The threshold is "<<thres<<endl;
		lm6.linking(thres);
		////cout<<"  Evaluating F"<<endl;
		nowscore = lm6.evaluating();
		//cout<<"  The F is "<<Fscore<<endl;
		allFscore6+=nowscore;

		//cout<<"Global linking by "<<m6<<endl;
		Linkmap lm7(m6,randindex);
		thres = lm7.find_thres();
		//cout<<"  The threshold is "<<thres<<endl;
		//lm7.linking(thres);
		for(int i=0;i<LAMB1_SIZE;++i)
		{
			for(int j=0;j<LAMB2_SIZE;++j)
			{
				nowscore = lm7.global(thres,lamb1[i],lamb2[j]);
				allFscore7[i][j]+=nowscore;
			}
		}
	}

	cout<<"Linking by "<<m1<<endl;
	printAverage(allFscore1,total_sim);
	cout<<"Linking by "<<m2<<endl;
	printAverage(allFscore2,total_sim);
	cout<<"Linking by "<<m5<<endl;
	printAverage(allFscore5,total_sim);
	cout<<"Linking by "<<m3<<endl;
	printAverage(allFscore3,total_sim);
	cout<<"Linking by "<<m4<<endl;
	printAverage(allFscore4,total_sim);
	cout<<"Linking by "<<m6<<endl;
	printAverage(allFscore6,total_sim);
	for(int i=0;i<LAMB1_SIZE;++i)
		{
			for(int j=0;j<LAMB2_SIZE;++j)
			{
				cout<<"Global Linking by "<<m6<<endl;
				cout<<"with lamb1 "<<lamb1[i]<<" and lamb2 "<<lamb2[j]<<endl;
				printAverage(allFscore7[i][j],total_sim);
			}
		}
}