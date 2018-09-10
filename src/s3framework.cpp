#include <iostream>

#include <stdint.h>
#include <stdlib.h>
#include <time.h> 
#include <stdio.h>
#include <pthread.h>
#include <sys/sysinfo.h>

#include "s3framework.h"
#include "Resources.h"
#include "Predictor.h"
#include "Action.h"
#include "MicroSpec.h"
#include "Timer.h"

using namespace std;
using namespace microspec;

namespace s3
{
	class TrainDFA:public DFA
	{
	public:
		TrainDFA():DFA() 
		{
			mReExecuteAction = doNothingReExecute;
		}
		virtual ~TrainDFA() {}

		virtual void run(const Table* table, const Input* input)
		{
			int* tableList_ = table->getTable();
			int nstate_ = table->getNumState();
			int nsymbol_ = table->getNumSymbol();
		
			int* Inputs_ = input->getPointer();
			int length_ = input->getLength();

			int state_ = rand() % nstate_;
			long determineTrainLength = (TrainLength > length_)? length_ : TrainLength;
			long randstartpoint_ = rand() % (length_ - determineTrainLength + 1);

			for (long i = randstartpoint_; i < randstartpoint_ + determineTrainLength; i++)
			{
				int temp = tableList_[state_ * nsymbol_ + Inputs_[i]];
				state_ = temp & 0X0FFFFFFF;

				// Action Function (state_)
				mAction(temp, mEndingResults);
			}
			//printf("The final state is  %d\n", state_);			
		}

		void doubleRun(const Table* table, const Input* input)
		{
			int* tableList_ = table->getTable();
			int nstate_ = table->getNumState();
			int nsymbol_ = table->getNumSymbol();
		
			int* Inputs_ = input->getPointer();
			int length_ = input->getLength();

			int state1_ = rand() % nstate_;
			int state2_ = rand() % nstate_;

			long determineTrainLength = (TrainLength > length_)? length_ : TrainLength;
			long randStartPoint = rand() % (length_ - determineTrainLength + 1);

			for (long i = randStartPoint; i < randStartPoint + determineTrainLength; i++)
			{
				int temp1 = tableList_[state1_ * nsymbol_ + Inputs_[i]];
				int temp2 = tableList_[state2_ * nsymbol_ + Inputs_[i]];
				state1_ = temp1 & 0X0FFFFFFF;
				state2_ = temp2 & 0X0FFFFFFF;

				if (state1_ == state2_)
					state2_ = 0;

				// Action Function (state_)
				mReExecuteAction(temp1, temp2, mEndingResults);
			}
			//printf("The final state is  %d and %d \n", state1_, state2_);				
		}

	private:
		actionReExecute mReExecuteAction;
	};

	struct  PassTrainDFA
	{
		TrainDFA* mObjDFA;
		const Table* mTablePointer;
		const Input* mInputPointer;
	};

	ArchitecturePropertyCollector::ArchitecturePropertyCollector()
	{
		mThreads = 1;
		mTrainTime = 0;
		mAlpha = new double [1];
		mAlpha[0] = 1.00;
		mGama = 1.00;
		mAverageBaseTime = 0.00;
	}

	ArchitecturePropertyCollector::ArchitecturePropertyCollector(int threads)
	{
		mThreads = threads;
		mTrainTime = 0;
		mAlpha = new double [threads];
		mGama = 1.00;
		for (int i =0 ; i < threads; i++)
			mAlpha[i] = 1.00;
		mAverageBaseTime = 0.00;
	}

	ArchitecturePropertyCollector::~ArchitecturePropertyCollector()
	{
		delete []mAlpha;
	}

	void ArchitecturePropertyCollector::executeTrain(const Table* table_, const Input* inputs_)
	{
		TrainDFA* objTrain = new TrainDFA();

		Timer T1;
		srand(time(NULL));
		startTime(&T1);
		objTrain->run(table_, inputs_);		
		stopTime(&T1);
		double baseTime =  elapsedTime(T1);
		if (mTrainTime == 0)
			mAverageBaseTime = baseTime;
		else
			mAverageBaseTime = (mAverageBaseTime * mTrainTime + baseTime) / (mTrainTime+1.0) ;

		cout << "Base time is " << baseTime << endl;
		//PTHREAD--------------------------------
		int errorCode1, errorCode2;
		long t;
		pthread_t* threads;			
		threads=(pthread_t*)malloc(sizeof(pthread_t) * mThreads);
		cpu_set_t* cpu;		// thread binding variables
		int MAXCPU = (mThreads > get_nprocs() ? get_nprocs():mThreads);
		cpu=(cpu_set_t*)malloc(sizeof(cpu_set_t) * MAXCPU);
		//PTHREAD-------------------------------	

		// thread binding
		for(t=0; t < MAXCPU; t++)
		{
			CPU_ZERO(&cpu[t]);
			CPU_SET(t, &cpu[t]);
		}		

		PassTrainDFA* objTrainPointer = new PassTrainDFA;
		objTrainPointer->mObjDFA = objTrain;
		objTrainPointer->mTablePointer = table_;
		objTrainPointer->mInputPointer = inputs_;

		for (int i = 2; i <= mThreads; i++)
		{
			startTime(&T1);	
			for(t = 0; t < i; t++)
			{
				errorCode1 = pthread_create(&threads[t], NULL, callDFA_run, (void*)objTrainPointer);
				if (errorCode1)
				{
					printf("ERROR; return code from pthread_create() is %d\n", errorCode1);
					exit(-1);
				}
				errorCode2 = pthread_setaffinity_np(threads[t], sizeof(cpu_set_t), &cpu[t%MAXCPU]);
			}
			for(t = 0; t< i; t++)
		    	pthread_join(threads[t], NULL);	
		    stopTime(&T1);
		    double mulTime = elapsedTime(T1);

		    cout << i << " " << mulTime << endl;
		    if (mTrainTime == 0)
				mAlpha[i-1] = mulTime / baseTime;
		    else
				mAlpha[i-1] =  ( (mulTime + (mAverageBaseTime*(mTrainTime+1.0) - baseTime)*mAlpha[i-1] ) / (mTrainTime+1.0) ) / mAverageBaseTime;		
		}

		startTime(&T1);	
		objTrainPointer->mObjDFA->doubleRun(table_, inputs_);
		stopTime(&T1);
		double doubleTime = elapsedTime(T1);
		cout << doubleTime << " " << baseTime << endl;

		if (mTrainTime == 0)
			mGama = doubleTime / baseTime;
		else
			mGama = ( (doubleTime + (mAverageBaseTime*(mTrainTime+1.0) - baseTime)*mGama) / (mTrainTime+1.0) ) / mAverageBaseTime;	

		mTrainTime++;
	}

	void* ArchitecturePropertyCollector::callDFA_run(void* args)
	{
		PassTrainDFA* Arg = (PassTrainDFA*)args;
		(Arg->mObjDFA)->run(Arg->mTablePointer, Arg->mInputPointer);
		pthread_exit((void*)args);
	}

	int ArchitecturePropertyCollector::getTrainTimes() const
	{
		return mTrainTime;
	}

	int ArchitecturePropertyCollector::getNumThreads() const
	{
		return mThreads;
	}

	double* ArchitecturePropertyCollector::getAlphaPointer()
	{
		return mAlpha;
	}

	double ArchitecturePropertyCollector::getAlpha(int threadsIndex) const
	{
		if (threadsIndex <= mThreads && threadsIndex > 0)
			return mAlpha[threadsIndex-1];
		else
			return 1.00;
	}
		
	double ArchitecturePropertyCollector::getGama() const
	{
		return mGama;
	}

	double ArchitecturePropertyCollector::getAverageSeqTime() const
	{
		return mAverageBaseTime;
	}

}	// end of namespace s3
