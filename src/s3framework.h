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
		DFAPropertyCollector(int samplePoolSize, int samplePerTest);
		~DFAPropertyCollector();

		// @Brief By reading the inputs from the paths shown on file @inputsLibFile, then 
		// execute convergence length profiling for the given @table 
		void executeProfilingOnTestLibs(const microspec::Table* table, 
			const char* inputsLibFile, const microspec::MappingRule* rule);

		// @Brief Execute the convergence length profiling on the 
		// given @input for the given @table 
		void executeProfiling(const microspec::Table* table, const microspec::Input* input);

		int getCurrentSamples() const;
		int getSamplePoolSize() const;
		int getSamplePerInput() const;
		double getAverageConvergenceLength() const;
		long* getConvergencePool() const;

		void setSamplesPerInput(int sizeSet);

	private:
		int mSamples;
		int mSamplePoolSize;
		int mSamplesPerInput;
		double mConvergenceLength;
		long* mConvergenceLengthPool;
	};

	class S3RunTimeController
	{
	public:
		S3RunTimeController();
		S3RunTimeController(int threads);
		~S3RunTimeController();

		void startOffileArchiProfile();
		void startOfflineDFAProfile();
		void startModelConstruction(char* modelType);

		void setTestLength(long length);
		const long getTestLength() const;
		const int getOptConfiguration() const;
		const double getOptPerformance() const;
		double* getSpeedupArrays() const; 

	protected:

		// @Brief In Model M1 and M2, the arhictecture effects are not considered;
		// while those effects are considered in the plus version; And M1 model checks 
		// each sample in every configuration, while M2 will only use the averave sample.
		void ModelM1();
		void ModelM1Plus();
		void ModelM2();
		void ModelM2Plus();

	private:
		int mThreads;
		bool mArchiProfileComplete;
		bool mPropertyProfileComplete;

		long mTestLength;

		ArchitecturePropertyCollector* mOfflineArchiCollector;
		DFAPropertyCollector* mOfflineDFACollector;
		
		double* mPredictSpeedUp;
		int mOptimalConfiguration;
		double mOptimalPerformance;
	};

}	// end of namespace s3

#endif // S3FRAMEWORK_H
