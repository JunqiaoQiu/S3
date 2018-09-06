#include <iostream>

#include <stdint.h>
#include <stdlib.h>
#include <time.h> 
#include <stdio.h>
#include <pthread.h>
#include <sys/sysinfo.h>

#include "s3framework.hpp"
#include "Resources.hpp"
#include "Predictor.hpp"
#include "MicroSpec.hpp"
#include "Timer.hpp"

using namespace std;
using namespace microspec;

namespace s3
{
	
	const long TrainLength = 20000000;

	class Train_DFA:public DFA
	{
	public:
		Train_DFA() {}
		virtual ~Train_DFA() {}

		virtual void Run(const Table* table, const Input* input)
		{
			int* tableList_ = table->getTable();
			int nstate_ = table->getNumState();
			int nsymbol_ = table->getNumSymbol();
		
			int* Inputs_ = input->getPointer();
			int length_ = input->getLength();

			int state_ = rand() % nstate_;
			long determinetrainlength = (TrainLength > length_)? length_ : TrainLength;
			long randstartpoint_ = rand() % (length_ - determinetrainlength + 1);

			for (long i = randstartpoint_; i < randstartpoint_ + determinetrainlength; i++)
			{
				int temp = tableList_[state_ * nsymbol_ + Inputs_[i]];
				state_ = temp & 0X0FFFFFFF;

				// Action Function (state_)
				//  table->getAction()(...., state_);
			}
			printf("The final state is  %d\n", state_);			
		}

		void DoubleRun(const Table* table, const Input* input)
		{
			int* tableList_ = table->getTable();
			int nstate_ = table->getNumState();
			int nsymbol_ = table->getNumSymbol();
		
			int* Inputs_ = input->getPointer();
			int length_ = input->getLength();

			int state1_ = rand() % nstate_;
			int state2_ = rand() % nstate_;

			long determinetrainlength = (TrainLength > length_)? length_ : TrainLength;
			long randstartpoint_ = rand() % (length_ - determinetrainlength + 1);

			for (long i = randstartpoint_; i < randstartpoint_ + determinetrainlength; i++)
			{
				int temp1 = tableList_[state1_ * nsymbol_ + Inputs_[i]];
				int temp2 = tableList_[state2_ * nsymbol_ + Inputs_[i]];
				state1_ = temp1 & 0X0FFFFFFF;
				state2_ = temp2 & 0X0FFFFFFF;

				if (state1_ == state2_)
					state2_ = 0;

				// Action Function (state_)
				//  table->getAction()(...., state_);
			}
			printf("The final state is  %d and %d \n", state1_, state2_);				
		}
	};

	struct  PassItemOBJ
	{
		Train_DFA* mobj_;
		Table* mtable_;
		Input* minput_;
	};

	ArchitectureEffect::ArchitectureEffect()
	{
		mthreads = 1;
		mtraintime = 0;
		malpha = new double [1];
		malpha[0] = 1.00;
		mgama = 1.00;
	}

	ArchitectureEffect::ArchitectureEffect(int threads_)
	{
		mthreads = threads_;
		mtraintime = 0;
		malpha = new double [threads_];
		mgama = 1.00;
		for (int i =0 ; i < threads_; i++)
			malpha[i] = 1.00;

	}

	ArchitectureEffect::~ArchitectureEffect()
	{
		delete []malpha;
	}

	void ArchitectureEffect::ExecuteTrain(Table* table_, Input* inputs_)
	{
		Train_DFA* obj_train = new Train_DFA();

		Timer T1;
		srand(time(NULL));
		startTime(&T1);
		obj_train->Run(table_, inputs_);		
		stopTime(&T1);
		double basetime_ =  elapsedTime(T1);

		//PTHREAD--------------------------------
		int errorCheck1, errorCheck2;
		long t;
		pthread_t* threads;			
		threads=(pthread_t*)malloc(sizeof(pthread_t)* mthreads);
		cpu_set_t* cpu;		// thread binding variables
		int MAXCPU = (mthreads > get_nprocs() ? get_nprocs():mthreads);
		cpu=(cpu_set_t*)malloc(sizeof(cpu_set_t) * MAXCPU);
		//PTHREAD-------------------------------	

		// thread binding
		for(t=0; t < MAXCPU; t++)
		{
			CPU_ZERO(&cpu[t]);
			CPU_SET(t, &cpu[t]);
		}		

		PassItemOBJ* obj_train_pointer = new PassItemOBJ;
		obj_train_pointer->mobj_ = obj_train;
		obj_train_pointer->mtable_ = table_;
		obj_train_pointer->minput_ = inputs_;

		for (int i = 2; i <= mthreads; i++)
		{
			startTime(&T1);	
			for(t = 0; t < i; t++)
			{
				errorCheck1 = pthread_create(&threads[t], NULL, caller, (void*)obj_train_pointer);
				if (errorCheck1)
				{
					printf("ERROR; return code from pthread_create() is %d\n", errorCheck1);
					exit(-1);
				}
				errorCheck2 = pthread_setaffinity_np(threads[t], sizeof(cpu_set_t), &cpu[t%MAXCPU]);
			}
			for(t = 0; t< i; t++)
		    	pthread_join(threads[t], NULL);	

		    stopTime(&T1);
		    double multime = elapsedTime(T1);
		    cout << i << " " << multime << endl;
		    if (mtraintime == 0)
				malpha[i-1] = multime / basetime_;
		    else
				malpha[i-1] = (multime + basetime_ * malpha[i-1])  / (mtraintime+1.0);		
		}

		startTime(&T1);	
		obj_train->DoubleRun(table_, inputs_);
		stopTime(&T1);
		double doubletime = elapsedTime(T1);
		cout << doubletime << " " << basetime_ << endl;

		if (mtraintime == 0)
			mgama = doubletime / basetime_;
		else
			mgama = (doubletime + basetime_ * mgama)  / (mtraintime+1.0);	

		mtraintime++;
	}

	void* ArchitectureEffect::caller(void* args)
	{
		PassItemOBJ* Arg = (PassItemOBJ*)args;
		(Arg->mobj_)->Run(Arg->mtable_, Arg->minput_);
		pthread_exit((void*)args);
	}

	int ArchitectureEffect::GetTrainTimes() const
	{
		return mtraintime;
	}

	int ArchitectureEffect::GetNumThreads() const
	{
		return mthreads;
	}

	double* ArchitectureEffect::GetAlphaPointer()
	{
		return malpha;
	}

	double ArchitectureEffect::GetAlpha(int threads_) const
	{
		if (threads_ <= mthreads && threads_ > 0)
			return malpha[threads_-1];
		else
			return 1.00;
	}
		
	double ArchitectureEffect::GetGama() const
	{
		return mgama;
	}

}	// end of namespace s3
