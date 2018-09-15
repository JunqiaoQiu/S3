#include <iostream>

#include <stdint.h>
#include <stdlib.h>
#include <time.h> 
#include <stdio.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <cstring>

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

/* Implementation of class TrainDFA */
	class TrainDFA:public DFA
	{
	public:
		TrainDFA():DFA() 
		{
			mReExecuteAction = doNothingReExecute;
			mTrainLength = DefaultTrainLength;
		}
		TrainDFA(long trainLength):DFA() 
		{
			mReExecuteAction = doNothingReExecute;
			mTrainLength = trainLength;
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
			long determineTrainLength = (mTrainLength > length_)? length_ : mTrainLength;
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

			long determineTrainLength = (mTrainLength > length_)? length_ : mTrainLength;
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
		long mTrainLength; 
	};

	struct  PassTrainDFA
	{
		TrainDFA* mObjDFA;
		const Table* mTablePointer;
		const Input* mInputPointer;

	};

/* Implementation of class ArchitecturePropertyCollector */

	ArchitecturePropertyCollector::ArchitecturePropertyCollector()
	{
		mThreads = 1;
		mTrainingLength = DefaultTrainLength;
		mTrainTime = 0;
		mAlpha = new double [1];
		mAlpha[0] = 1.00;
		mGama = 1.00;
		mAverageBaseTime = 0.00;
	}

	ArchitecturePropertyCollector::ArchitecturePropertyCollector(int threads)
	{
		mThreads = threads;
		mTrainingLength = DefaultTrainLength;
		mTrainTime = 0;
		mAlpha = new double [threads];
		mGama = 1.00;
		for (int i =0 ; i < threads; i++)
			mAlpha[i] = 1.00;
		mAverageBaseTime = 0.00;
	}

	ArchitecturePropertyCollector::ArchitecturePropertyCollector(int threads, long trainingLength)
	{
		mThreads = threads;
		mTrainingLength = trainingLength;
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
		TrainDFA* objTrain = new TrainDFA(mTrainingLength);

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

		// cout << "Base time is " << baseTime << endl;
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

		    // cout << i << " " << mulTime << endl;
		    if (mTrainTime == 0)
				mAlpha[i-1] = mulTime / baseTime;
		    else
				mAlpha[i-1] =  ( (mulTime + (mAverageBaseTime*(mTrainTime+1.0) - baseTime)*mAlpha[i-1] ) / (mTrainTime+1.0) ) / mAverageBaseTime;		
		}

		startTime(&T1);	
		objTrainPointer->mObjDFA->doubleRun(table_, inputs_);
		stopTime(&T1);
		double doubleTime = elapsedTime(T1);
		// cout << doubleTime << " " << baseTime << endl;

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

	void ArchitecturePropertyCollector::executeTrain(const microspec::Table* table, const char* inputsLibFile, 
			const microspec::MappingRule* rule)	
	{
		// collect the testing input files from the Library file
		ifstream inputLib;
		inputLib.open(inputsLibFile);
		vector<string> allInputFiles;
		
		while(inputLib)
		{
			string inputTemp;
			getline(inputLib, inputTemp);
			if(inputLib.fail())
				break;
			allInputFiles.push_back(inputTemp);
		}
		inputLib.close();
		int numInputs = allInputFiles.size();

		for (int i = 0; i < numInputs; i++)
		{
				const Input* inputs = Input::readFromFile(allInputFiles[i].c_str(), rule);
				this->executeTrain(table, inputs);
				delete inputs;
		}
	}

	void ArchitecturePropertyCollector::repeatExecuteTrain(const microspec::Table* table, const microspec::Input* inputs, 
		const int repeatTime)
	{
		for (int i = 0; i < repeatTime; i++)
			this->executeTrain(table, inputs);
	}

	void ArchitecturePropertyCollector::repeatExecuteTrain(const microspec::Table* table, const char* inputsLibFile, 
			const microspec::MappingRule* rule, const int repeatTime)
	{
		// collect the testing input files from the Library file
		ifstream inputLib;
		inputLib.open(inputsLibFile);
		vector<string> allInputFiles;
		
		while(inputLib)
		{
			string inputTemp;
			getline(inputLib, inputTemp);
			if(inputLib.fail())
				break;
			allInputFiles.push_back(inputTemp);
		}
		inputLib.close();
		int numInputs = allInputFiles.size();

		for (int i = 0; i < numInputs; i++)
		{
				const Input* inputs = Input::readFromFile(allInputFiles[i].c_str(), rule);
				this->repeatExecuteTrain(table, inputs, repeatTime);
				delete inputs;
		}
	}	

	int ArchitecturePropertyCollector::getTrainTimes() const
	{
		return mTrainTime;
	}

	long ArchitecturePropertyCollector::getTrainingLength() const
	{
		return mTrainingLength;
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

/* Implementation of class DFAPropertyCollector */	

	DFAPropertyCollector::DFAPropertyCollector()
	{
		mSamples = 0;
		mSamplePoolSize = DFAProfileSample; 
		mSamplesPerInput = DFAProfileSamplePerInput;
		mCorrectPrediction = 0.0;
		mConvergenceLength = 0.0;
		mEffectiveConvergenceLength = 0.0;
		mConvergenceLengthPool = new long [mSamplePoolSize];
	}

	DFAPropertyCollector::DFAPropertyCollector(int samplePoolSize, int samplePerTest)
	{
		mSamples = 0;
		mSamplePoolSize = samplePoolSize; 
		mSamplesPerInput = samplePerTest;
		mCorrectPrediction = 0;
		mConvergenceLength = 0;
		mEffectiveConvergenceLength = 0.0;
		mConvergenceLengthPool = new long [mSamplePoolSize];		
	}

	DFAPropertyCollector::~DFAPropertyCollector()
	{
		delete []mConvergenceLengthPool;
	}

	void DFAPropertyCollector::executeProfilingOnTestLibs(const microspec::Table* table, 
				const char* inputsLibFile, const microspec::MappingRule* rule)
	{
		// collect the testing input files from the Library file
		ifstream inputLib;
		inputLib.open(inputsLibFile);
		vector<string> allInputFiles;
		
		while(inputLib)
		{
			string inputTemp;
			getline(inputLib, inputTemp);
			if(inputLib.fail())
				break;
			allInputFiles.push_back(inputTemp);
		}
		inputLib.close();
		int numInputs = allInputFiles.size();

		for (int i = 0; i < numInputs; i++)
		{
			if (mSamples < mSamplePoolSize)
			{
				const Input* inputs = Input::readFromFile(allInputFiles[i].c_str(), rule);
				// cout << inputs->getLength() << endl;
				this->executeProfilingOnOneInput(table, inputs);
				cout << "Finish profiling on Input " << i+1 << " shown in the Lib" << endl;
				delete inputs;
			}
		}
	}

	void DFAPropertyCollector::executeProfilingOnTestLibs(const microspec::Table* table, 
			const microspec::Input** InputLibs, const int libSize)
	{
		for (int i = 0; i < libSize; i++)
		{
			if (mSamples < mSamplePoolSize)
			{
				this->executeProfilingOnOneInput(table, InputLibs[i]);
			}
		}
	}

	void DFAPropertyCollector::executeProfilingOnOneInput(const microspec::Table* table, const microspec::Input* input)
	{
		srand(time(NULL));
		int* tableList_ = table->getTable();
		int state_ = table->getStartState();
		int nstate_ = table->getNumState();
		int nsymbol_ = table->getNumSymbol();

		int* Inputs_ = input->getPointer();
		int length_ = input->getLength();		

		Predictor* objPredictor = Predictor::constructPredictor(table, input, 2, 2);

		long* samplePoints;
		int* sampleStates;
		long* currentSampleLength;

		samplePoints = new long [mSamplesPerInput];
		sampleStates = new int [mSamplesPerInput];
		currentSampleLength = new long [mSamplesPerInput];

		long i, j, indexRecord;

		// Construct a filter to find out the state pair which are likely to appear
		for (i = 0; i < mSamplesPerInput; i++)
		{
			long temp = rand() % ((long)(length_ * 0.01) + objPredictor->getLookBackLength());
			samplePoints[i] = temp;
		}
		sort(samplePoints, samplePoints+mSamplesPerInput);
		indexRecord = 0;
		for (i = 0; i < mSamplesPerInput; i++)
			sampleStates[i] = objPredictor->seqPredict(samplePoints[i] - objPredictor->getLookBackLength());

		// get the convergence length between real states and profile states
		for (i = 0; i < length_; i ++)
		{
			if (i == samplePoints[indexRecord])
			{
				int statePair1 = state_;
				int statePair2 = sampleStates[indexRecord];
				if (statePair1 == statePair2)
				{
					currentSampleLength[indexRecord] = 0;
					mCorrectPrediction ++;
				}
				else
				{
					for (j = i; j < length_; j++)
					{
						int tempPair1 = tableList_[statePair1 * nsymbol_ + Inputs_[j]];
						int tempPair2 = tableList_[statePair2 * nsymbol_ + Inputs_[j]];
						statePair1 = tempPair1 & 0X0FFFFFFF;
						statePair2 = tempPair2 & 0X0FFFFFFF;
						if (statePair1 == statePair2)
						{
							currentSampleLength[indexRecord] = j-i+1; 
							break;
						}
					}
					if (j == length_)
						currentSampleLength[indexRecord] = length_- i + 1; 
				}
				indexRecord++;
				if (indexRecord == mSamplesPerInput)
					break;
			}

			int temp = tableList_[state_ * nsymbol_ + Inputs_[i]];
			state_ = temp & 0X0FFFFFFF;
		}

		for (i = 0; i < mSamplesPerInput; i++)
		{
			if (mSamples < mSamplePoolSize)
			{
				mConvergenceLengthPool[mSamples] = currentSampleLength[i];
				mSamples++;
			}
			else
			{
				cout << "Sample Pool is Full ... " << endl;
			}
		}
	}

	const int DFAPropertyCollector::getCurrentSamples() const
	{
		return mSamples;
	}

	const int DFAPropertyCollector::getSamplePoolSize() const
	{
		return mSamplePoolSize;
	}

	const int  DFAPropertyCollector::getSamplePerInput() const
	{
		return mSamplesPerInput;
	}

	double DFAPropertyCollector::getPredictionAccuracy()
	{	
		if (mSamples <= 0)
			return 0.0;
		else
			return ((double) mCorrectPrediction * 1.0 / (double) mSamples); 
	}

	double DFAPropertyCollector::getAverageConvergenceLength()
	{
		long total = 0;
		for (int i= 0; i < mSamples; i++)
			total += (mConvergenceLengthPool[i]);
		mConvergenceLength = total * 1.0 /(double(mSamples));

		return mConvergenceLength;
	}

	double DFAPropertyCollector::getEffectiveAverageConvergenceLength()
	{
		long total = 0;
		int effectNum = 0;
		for (int i= 0; i < mSamples; i++)
		{
			if (mConvergenceLengthPool[i] > 0)
			{
				total += (mConvergenceLengthPool[i]);
				effectNum++;
			}
		}
		mEffectiveConvergenceLength = total * 1.0 /(double(effectNum));

		return mEffectiveConvergenceLength;
	}	

	long* DFAPropertyCollector::getConvergencePool() const
	{
		return mConvergenceLengthPool;
	}

	void DFAPropertyCollector::setSamplesPerInput(int sizeSet)
	{
		mSamplesPerInput = sizeSet;
	}

/* Implementation of class S3RunTimeController 	*/

	S3RunTimeController::S3RunTimeController()
	{
		mThreads = 1;
		mTestLength = 0;
		mArchiProfileComplete = false;
		mPropertyProfileComplete = false;

		mPredictSpeedUp = new double [1];
		mOptimalConfiguration = 1;
		mOptimalPerformance = 1.0;
	}

	S3RunTimeController::S3RunTimeController(int threads)
	{
		mThreads = threads;
		mTestLength = 0;
		mArchiProfileComplete = false;
		mPropertyProfileComplete = false;

		mPredictSpeedUp = new double [mThreads];
		mOptimalConfiguration = 1;
		mOptimalPerformance = 1.0; 		
	}	

	S3RunTimeController::S3RunTimeController(int threads, long testLength)
	{
		mThreads = threads;
		mTestLength = testLength;
		mArchiProfileComplete = false;
		mPropertyProfileComplete = false;

		mPredictSpeedUp = new double [mThreads];
		mOptimalConfiguration = 1;
		mOptimalPerformance = 1.0; 		
	}	

	void S3RunTimeController::startOfflineProfile(const microspec::Table* table, 
			const char* inputsLibFile, const microspec::MappingRule* rule)
	{
		this->startOfflineArchiProfile(table, inputsLibFile, rule);
		this->startOfflineDFAProfile(table, inputsLibFile, rule);
	}

	void S3RunTimeController::startOfflineArchiProfile(const microspec::Table* table, 
			const char* inputsLibFile, const microspec::MappingRule* rule)
	{
		if (mArchiProfileComplete == false)	
			mOfflineArchiCollector = new ArchitecturePropertyCollector(mThreads);
		mOfflineArchiCollector->executeTrain(table, inputsLibFile, rule);
		mArchiProfileComplete = true;
	}

	void S3RunTimeController::startOfflineDFAProfile(const microspec::Table* table, 
			const char* inputsLibFile, const microspec::MappingRule* rule)
	{
		if (mPropertyProfileComplete == false)
			mOfflineDFACollector = new DFAPropertyCollector();
		mOfflineDFACollector->executeProfilingOnTestLibs(table, inputsLibFile, rule);
		mPropertyProfileComplete = true;
	}

	void S3RunTimeController::startModelConstruction(char* modelType)
	{
		char* modelTypeLow;
		modelTypeLow = new char [strlen(modelType)];
		for (int i = 0; i < strlen(modelType); ++i)
	    	modelTypeLow[i] = tolower(modelType[i]);
	    this->clearPreviousResults();	

		if (mTestLength <= 0)	 
		{
			cout << endl << "Not Provide The target Test Length ... " << endl;
			return; 
		}

		if (mPropertyProfileComplete == false)
		{
			cout << endl << "Not Provide Offline DFA Property Profiling Yet ... " << endl; 
			return ;
		}   

	    if(std::string(modelTypeLow).find("+") == string::npos && std::string(modelTypeLow).find("lus") == string::npos)
	    {
	    	if (std::string(modelTypeLow).find("M2") != string::npos)
	    		this->ModelM2();
	    	else
	    		this->ModelM1();
	    }
	    else
	    {
	    	if (mArchiProfileComplete == false)
	    	{
				cout << endl << "Not Provide Offline Architecture Profiling Yet ... " << endl; 
				return;	    		
	    	}
	    	else
	    	{
	    		if (std::string(modelTypeLow).find("M2") != string::npos)
	    			this->ModelM2Plus();
	    		else
	    			this->ModelM1Plus(); 
	    	}                                                                                            
	    }
	}	

	void S3RunTimeController::ModelM1()
	{
		int i, j;
		for (i=1; i<mThreads; i++)
		{
			double ChunkLength = mTestLength *1.0 / (i+1);
			double SpecLength = ChunkLength;

			long tempTotal = 0;
			long* pool = mOfflineDFACollector->getConvergencePool();
			int poolSize = mOfflineDFACollector->getCurrentSamples();
			int CLSamples = 0;
			for (j = 0; j< poolSize; j++)
			{
				if (pool[j] > 0)
				{
					if (pool[j] > ChunkLength)
						tempTotal += ChunkLength;
					else
						tempTotal += pool[j];
					CLSamples++;
				}
			}
			if (CLSamples != 0)
				tempTotal = tempTotal * 1.0 / CLSamples;

			tempTotal = tempTotal * i * mOfflineDFACollector->getPredictionAccuracy();

			SpecLength += tempTotal;
			mPredictSpeedUp[i] = mTestLength * 1.0 / SpecLength;
			if (mPredictSpeedUp[i] > mOptimalPerformance)
			{
				mOptimalPerformance = mPredictSpeedUp[i];
				mOptimalConfiguration = i+1;
			}
		}
	}

	void S3RunTimeController::ModelM1Plus()
	{
		int i, j;
		double gama = mOfflineArchiCollector->getGama();
		for (i=1; i<mThreads; i++)
		{
			double ChunkLength = mTestLength *1.0 / (i+1);
			double SpecLength = ChunkLength * mOfflineArchiCollector->getAlpha(i+1);

			long tempTotal = 0;
			long* pool = mOfflineDFACollector->getConvergencePool();
			int poolSize = mOfflineDFACollector->getCurrentSamples();
			int CLSamples = 0;
			for (j = 0; j< poolSize; j++)
			{
				if (pool[j] > 0)
				{
					if (pool[j] > ChunkLength)
						tempTotal += ChunkLength;
					else
						tempTotal += pool[j];
					CLSamples++;
				}
			}
			if (CLSamples != 0)
				tempTotal = tempTotal * 1.0 / CLSamples;

			tempTotal = tempTotal * i * mOfflineDFACollector->getPredictionAccuracy() * gama;

			SpecLength += tempTotal;
			mPredictSpeedUp[i] = mTestLength * 1.0 / SpecLength;
			if (mPredictSpeedUp[i] > mOptimalPerformance)
			{
				mOptimalPerformance = mPredictSpeedUp[i];
				mOptimalConfiguration = i+1;
			}
		}
	}

	void S3RunTimeController::ModelM2()
	{
		int i, j;
		double averageCL = mOfflineDFACollector->getEffectiveAverageConvergenceLength();

		for (i=1; i<mThreads; i++)
		{
			double ChunkLength = mTestLength *1.0 / (i+1);
			double SpecLength = ChunkLength;

			if (averageCL > ChunkLength)
				SpecLength += (ChunkLength * i* mOfflineDFACollector->getPredictionAccuracy() * 1.0);
			else
				SpecLength += (averageCL * i * mOfflineDFACollector->getPredictionAccuracy() * 1.0);

			mPredictSpeedUp[i] = mTestLength * 1.0 / SpecLength;

			if (mPredictSpeedUp[i] > mOptimalPerformance)
			{
				mOptimalPerformance = mPredictSpeedUp[i];
				mOptimalConfiguration = i+1;
			}
		}
	}

	void S3RunTimeController::ModelM2Plus()
	{
		int i, j;
		double averageCL = mOfflineDFACollector->getEffectiveAverageConvergenceLength();
		double gama = mOfflineArchiCollector->getGama();

		for (i=1; i<mThreads; i++)
		{
			double ChunkLength = mTestLength *1.0 / (i+1);
			double SpecLength = ChunkLength * mOfflineArchiCollector->getAlpha(i+1); 

			if (averageCL > ChunkLength)
				SpecLength += (ChunkLength * i* mOfflineDFACollector->getPredictionAccuracy() * 1.0 * gama);
			else
				SpecLength += (averageCL * i * mOfflineDFACollector->getPredictionAccuracy() * 1.0 * gama);

			mPredictSpeedUp[i] = mTestLength * 1.0 / SpecLength;

			if (mPredictSpeedUp[i] > mOptimalPerformance)
			{
				mOptimalPerformance = mPredictSpeedUp[i];
				mOptimalConfiguration = i+1;
			}
		}
	}

	void S3RunTimeController::setTestLength(long length)
	{
		mTestLength = length;
	}

	const long S3RunTimeController::getTestLength() const
	{
		return mTestLength;
	}

	const int S3RunTimeController::getOptConfiguration() const
	{
		return mOptimalConfiguration;
	}

	const double S3RunTimeController::getOptPerformance() const
	{
		return mOptimalPerformance;
	}

	double* S3RunTimeController::getSpeedupArrays() const
	{
		return mPredictSpeedUp;
	}

	void S3RunTimeController::clearPreviousResults()
	{
		mPredictSpeedUp[0] = 1.00;
		for (int i = 1; i < mThreads; i++)
			mPredictSpeedUp[i] = 0.00;
		mOptimalConfiguration = 1;
		mOptimalPerformance = 1.00;
	}

	void S3RunTimeController::printSpeedups() const
	{
		for (int i = 0; i < mThreads; i++)
			cout << i+1 << " " << mPredictSpeedUp[i] << endl;
	}
	
	void S3RunTimeController::printSpeedups(char* outPutFile) const
	{
		ofstream out;
		out.open(outPutFile);
		for (int i = 0; i < mThreads; i++)
			out << i+1 << " " << mPredictSpeedUp[i] << endl;		
		out.close();
	}	

}	// end of namespace s3
