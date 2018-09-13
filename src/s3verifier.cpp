#include <iostream>

#include <stdint.h>
#include <stdlib.h>
#include <time.h> 
#include <stdio.h>
#include <pthread.h>
#include <sys/sysinfo.h>

#include <fstream>
#include <string>
#include <vector>

#include "s3verifier.h"
#include "Resources.h"
#include "Predictor.h"
#include "Action.h"
#include "MicroSpec.h"
#include "MicroSpecPthread.h"
#include "Timer.h"

using namespace std;
using namespace microspec;

namespace s3
{

/* Implementation of class S3Verifier */

	S3Verifier::S3Verifier()
	{
		mInputs = 0;
		mThreads = 1;
		mSpeedup = new double [1];
		mSpeedup[0] = 1.00; 
		mRepeatTime = DefaultMeasurementRepeat;
	}

	S3Verifier::S3Verifier(int numThreads)
	{
		mInputs = 0;
		mThreads = numThreads;
		mSpeedup = new double [mThreads];
		for (int i = 0; i < mThreads; i++)
			mSpeedup[i] = 1.00;
		mRepeatTime = DefaultMeasurementRepeat;
	}

	S3Verifier::~S3Verifier()
	{
		delete []mSpeedup;
	}

	S3Verifier* S3Verifier::constructS3Verifier(int numThreads)
	{
		S3Verifier* objVerifier;
		objVerifier = new S3Verifier(numThreads);
		return objVerifier;
	}

	void S3Verifier::startVerification(const Table* table, 
			const char* inputsLibFile, const MappingRule* rule)
	{
		int i, j, k;
		
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
		mInputs = allInputFiles.size();

		double** detailTime;
		detailTime = new double* [mInputs];
		for (i = 0; i < mInputs; i++)
			detailTime[i] = new double [mThreads];

		for (i = 0; i < mInputs; i++)
		{
			const Input* inputs = Input::readFromFile(allInputFiles[i].c_str(), rule);

			double** timeOnCurrentInput;
			timeOnCurrentInput = new double* [mRepeatTime];
			for (j = 0; j < mRepeatTime; j++)
			{
				timeOnCurrentInput[j] = new double [mThreads];

				double* returnArray; 
				returnArray = this->executeRunOnOneInput(table, inputs);
				for (k = 0; k < mThreads; k++)
					timeOnCurrentInput[j][k] = returnArray[k];
				delete []returnArray;
			}		
			for (j = 0; j < mThreads; j++)
			{
				detailTime[i][j] = 0.0;
				for (k = 0; k < mRepeatTime; k++)
					detailTime[i][j] += timeOnCurrentInput[k][j];
				detailTime[i][j] = detailTime[i][j] / (mRepeatTime*1.0);
			}
		}

		for (i = 0; i < mThreads; i++)
		{
			mSpeedup[i] = 0.0;
			for (j = 0; j< mInputs; j++)
				mSpeedup[i] += detailTime[j][i];
			mSpeedup[i] = mSpeedup[i] / (mInputs*1.0);
		}

		for (i = 1; i < mThreads; i++)
			mSpeedup[i] = mSpeedup[0] / mSpeedup[i];
		mSpeedup[0] = 1.00;
	}

	void S3Verifier::startVerification(const microspec::Table* table, 
			const microspec::Input** inputLib, int inputSize)
	{
		int i, j, k;
		
		// collect the testing input files from the Library file
		mInputs = inputSize;

		double** detailTime;
		detailTime = new double* [mInputs];
		for (i = 0; i < mInputs; i++)
			detailTime[i] = new double [mThreads];

		for (i = 0; i < mInputs; i++)
		{
			const Input* inputs = inputLib[i];

			double** timeOnCurrentInput;
			timeOnCurrentInput = new double* [mRepeatTime];
			for (j = 0; j < mRepeatTime; j++)
			{
				timeOnCurrentInput[j] = new double [mThreads];

				double* returnArray; 
				returnArray = this->executeRunOnOneInput(table, inputs);
				for (k = 0; k < mThreads; k++)
					timeOnCurrentInput[j][k] = returnArray[k];
				delete []returnArray;
			}		
			for (j = 0; j < mThreads; j++)
			{
				detailTime[i][j] = 0.0;
				for (k = 0; k < mRepeatTime; k++)
					detailTime[i][j] += timeOnCurrentInput[k][j];
				detailTime[i][j] = detailTime[i][j] / (mRepeatTime*1.0);
			}
		}

		for (i = 0; i < mThreads; i++)
		{
			mSpeedup[i] = 0.0;
			for (j = 0; j< mInputs; j++)
				mSpeedup[i] += detailTime[j][i];
			mSpeedup[i] = mSpeedup[i] / (mInputs*1.0);
		}

		for (i = 1; i < mThreads; i++)
			mSpeedup[i] = mSpeedup[0] / mSpeedup[i];
		mSpeedup[0] = 1.00;
	}
	double* S3Verifier::executeRunOnOneInput(const microspec::Table* table, 
			const microspec::Input* inputs)
	{
		double* currentTime;
		currentTime = new double [mThreads];

		Timer T1;
		DFA* objSeq = new SeqDFA();
		startTime(&T1);
		objSeq->run(table, inputs);
		stopTime(&T1);
		currentTime[0] = elapsedTime(T1);

		for (int i = 1; i < mThreads; i++)
		{
			DFA* objSpec = new SpecDFA_Pthread(i+1);
			startTime(&T1);
			objSpec->run(table, inputs);
			stopTime(&T1);
			currentTime[i] = elapsedTime(T1);
		}
		return currentTime;
	}

	int S3Verifier::getNumInputs() const
	{
		return mInputs;
	}

	int S3Verifier::getNumThreads() const
	{
		return mThreads;
	}

	double* S3Verifier::getSpeedups() const
	{
		return mSpeedup;
	}

	void S3Verifier::setRepeatTime(int repeatTime)
	{
		mRepeatTime = repeatTime;
	}

	int S3Verifier::getRepeatTime() const
	{
		return mRepeatTime;
	}

	void S3Verifier::printSpeedup2File(const char* outputFile) const
	{
		ofstream out;
		out.open(outputFile);
		for (int i = 0 ; i < mThreads; i++)
			out << i+1 << " " << mSpeedup[i] << endl;
		out.close();
	}

}	// end of namespace s3
