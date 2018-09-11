#ifndef S3FRAMEWORK_H
#define S3FRAMEWORK_H

#include <stdlib.h>
#include <stdio.h>

#include "Resources.h"
#include "Predictor.h"
#include "s3verifier.h"

namespace s3
{
	class ArchitecturePropertyCollector
	{
	public:
		ArchitecturePropertyCollector();
		ArchitecturePropertyCollector(int threads);
		~ArchitecturePropertyCollector();

		void executeTrain(const microspec::Table* table, const microspec::Input* inputs);
		int getTrainTimes() const;
		int getNumThreads() const;
		double* getAlphaPointer();
		double getAlpha(int threadsIndex) const;
		double getGama() const;
		double getAverageSeqTime() const;

	protected:
		static void* callDFA_run(void* args);

	private:
		int mThreads;
		int mTrainTime;
		double mAverageBaseTime;
		// resource contention factor
		double* mAlpha;
		// relative speed factor
		double mGama;
	};

	class DFAPropertyCollector
	{
	public:
		DFAPropertyCollector();
		~DFAPropertyCollector();

		void executeProfiling(const microspec::Table* table, const microspec::Input* inputs);

		int printCurrentSamples() const;
		double getAverageConvergenceLength() const;
	private:
		int mSamples;
		double mConvergenceLength;
		long* mConvergenceLengthPool;
	};

	class S3RunTimeController
	{
	public:
		S3RunTimeController();
		~S3RunTimeController();
	private:
		int mThreads;
		ArchitecturePropertyCollector* mOfflineArchiCollector;
		DFAPropertyCollector* mOfflineDFACollector;
		double 
	};

}	// end of namespace s3

#endif // S3FRAMEWORK_H
