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
		ArchitecturePropertyCollector(int threads, long trainingLength);
		~ArchitecturePropertyCollector();

		// TO DO: can provide function to directly read the alpha and gama from File
		// static ArchitecturePropertyCollector* constructArchiFromFile(char* profileFile);

		// @Brief Provide the @table and @inputs, or @inputsLibFIle, then measure the time
		// spent when using different number of cores on the input chunks with same length
		void executeTrain(const microspec::Table* table, const microspec::Input* inputs);
		void executeTrain(const microspec::Table* table, const char* inputsLibFile, 
			const microspec::MappingRule* rule);

		void repeatExecuteTrain(const microspec::Table* table, const microspec::Input* inputs, 
			const int repeatTime);
		void repeatExecuteTrain(const microspec::Table* table, const char* inputsLibFile, 
			const microspec::MappingRule* rule, const int repeatTime);		
		
		int getTrainTimes() const;
		long getTrainingLength() const;
		int getNumThreads() const;
		double* getAlphaPointer();
		double getAlpha(int threadsIndex) const;
		double getGama() const;
		double getAverageSeqTime() const;

	protected:
		static void* callDFA_run(void* args);

	private:
		// Set up when initialize
		int mThreads;
		long mTrainingLength;

		// the number of training to get the current results
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

		// TO DO: can provide function to directly read the sample pool from File
		// const DFAPropertyCollector* constructPropertyFromFile(char* profileFile);

		// @Brief By reading the inputs from the paths shown on file @inputsLibFile, 
		// or directly from a Input* array, then execute convergence length 
		// profiling for the given @table 
		void executeProfilingOnTestLibs(const microspec::Table* table, 
			const char* inputsLibFile, const microspec::MappingRule* rule);

		void executeProfilingOnTestLibs(const microspec::Table* table, 
			const microspec::Input** InputLibs, const int libSize);

		// @Brief Execute the convergence length profiling on the 
		// given @input for the given @table 
		void executeProfilingOnOneInput(const microspec::Table* table, const microspec::Input* input);

		const int getCurrentSamples() const;
		const int getSamplePoolSize() const;

		const int getSamplePerInput() const;
		void setSamplesPerInput(int sizeSet);
		
		double getPredictionAccuracy();

		// Brief function getAverageConvergenceLength() will return the average of the CL
		// pool, while func getEffectiveAverageConvergenceLength() return the average of the 
		// CL pool without considering the correct prediction samples. 
		double getAverageConvergenceLength();
		double getEffectiveAverageConvergenceLength();
		long* getConvergencePool() const;

	private:
		int mSamples;
		int mSamplePoolSize;
		int mSamplesPerInput;
		int mCorrectPrediction;
		double mConvergenceLength;
		double mEffectiveConvergenceLength;
		long* mConvergenceLengthPool;
	};

	class S3RunTimeController
	{
	public:
		S3RunTimeController();
		S3RunTimeController(int threads);
		S3RunTimeController(int threads, long testLength);
		~S3RunTimeController();

		// @Brief start the offline profiling of arhictecture effects and dfa property.
		void startOfflineProfile(const microspec::Table* table, 
			const char* inputsLibFile, const microspec::MappingRule* rule);

		// @Brief start the offline profiling of architecture effects. 
		void startOfflineArchiProfile(const microspec::Table* table, 
			const char* inputsLibFile, const microspec::MappingRule* rule);


		// @Brief start the offline profiling of dfa property.
		void startOfflineDFAProfile(const microspec::Table* table, 
			const char* inputsLibFile, const microspec::MappingRule* rule);

		// @Brief Provide the name of model, with the help of offline profiling, 
		// the performance analysis will be completed. 
		void startModelConstruction(char* modelType);

		void setTestLength(long length);
		const long getTestLength() const;

		const int getOptConfiguration() const;
		const double getOptPerformance() const;
		double* getSpeedupArrays() const; 

		void printSpeedups() const;
		void printSpeedups(char* outPutFile) const;

	protected:
		void clearPreviousResults();

		// @Brief In Model M1 and M2, the arhictecture effects are not considered;
		// while those effects are considered in the plus version; And M1 model checks 
		// each sample in every configuration, while M2 will only use the averave sample.
		void ModelM1();
		void ModelM1Plus();
		void ModelM2();
		void ModelM2Plus();

	private:
		// Basic Information 
		int mThreads;
		bool mArchiProfileComplete;
		bool mPropertyProfileComplete;
		long mTestLength;

		// Necessary Components
		ArchitecturePropertyCollector* mOfflineArchiCollector;
		DFAPropertyCollector* mOfflineDFACollector;
		
		// Target Results
		double* mPredictSpeedUp;
		int mOptimalConfiguration;
		double mOptimalPerformance;
	};

}	// end of namespace s3

#endif // S3FRAMEWORK_H
