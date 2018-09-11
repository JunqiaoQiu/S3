#ifndef S3VERIFIER_H
#define S3VERIFIER_H

#include <stdlib.h>
#include <stdio.h>

#include "Resources.h"
#include "Predictor.h"

namespace s3
{
	const long TrainLength = 10000000;
	const int DFAProfileSample = 120;
	const int DFAProfileSamplePerInput = 12;
	const int DefaultLookBackLength = 100;
	const int DefaultMeasurementRepeat = 5;

	class S3Verifier
	{
	public:
		S3Verifier();
		S3Verifier(int numThreads, int numInputs);
		~S3Verifier();
		
		static S3Verifier* constructS3Verifier(char* testFilesLibs, const microspec::Table* table,
			const microspec::MappingRule* rule, int configurations);

		void startVerification();
		double* executeRunOnOneInput(const microspec::Table* table, 
			const microspec::Input* inputs);

		int getNumInputs() const;
		int getNumThreads() const;
		double* getSpeedups() const;
		void setRepeatTime(int repeatTime);
		int getRepeatTime() const;

	private:
		int mInputs;
		int mThreads;
		int mRepeatTime;
		double* mSpeedup;
	}

}	// end of namespace s3

#endif // S3VERIFIER_H
