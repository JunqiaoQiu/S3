#ifndef S3VERIFIER_H
#define S3VERIFIER_H

#include <stdlib.h>
#include <stdio.h>

#include "Resources.h"
#include "Predictor.h"

namespace s3
{
	// Some default settings
	const long DefaultTrainLength = 10000000;
	const int DFAProfileSample = 120;
	const int DFAProfileSamplePerInput = 12;
	const int DefaultMeasurementRepeat = 5;

	// @Brief Provide the verification about a given FSM average performance
	// when using different number of cores on a plaltform 
	class S3Verifier
	{
	public:
		S3Verifier();
		S3Verifier(int numThreads);
		S3Verifier(int numThreads, int numRepeat);
		~S3Verifier();

		void setAction(char* actionType);
		
		// Return a S3Verifier pointer with proding the max # cores could be used
		static S3Verifier* constructS3Verifier(int numThreads);

		// @Brief Start FSM execution on the test inputs, and thus getting the 
		// speedup array. @table represents the given DFA information, while 
		// @inputsLibFile includes the name of files where the test inputs locate. 
		// @rule denote the mapping rule used. 
		void startVerification(const microspec::Table* table, 
			const char* inputsLibFile, const microspec::MappingRule* rule);

		// @Brief Start FSM execution on the test inputs, and thus getting the 
		// speedup array. @table represents the given DFA information, while 
		// @inputLib points to the Input object array, and @inputSize denotes the lib size. 
		// @rule denote the mapping rule used. 
		void startVerification(const microspec::Table* table, 
			const microspec::Input** inputLib, int inputSize);		

		// @Brief Provide a execution measurement for a given DFA on a 
		// given input, and return the corresponding execution time.
		double* executeRunOnOneInput(const microspec::Table* table, 
			const microspec::Input* inputs);

		int getNumInputs() const;
		int getNumThreads() const;
		double* getSpeedups() const;
		void setRepeatTime(int repeatTime);
		int getRepeatTime() const;
		double	getSequentialTime() const;
		void printSpeedup2File(const char* outputFile) const;

	private:
		int mInputs;
		int mThreads;
		int mRepeatTime;
		double* mSpeedup;
		double mSequentialTime;
		char* mActionType;
	};

}	// end of namespace s3

#endif // S3VERIFIER_H
