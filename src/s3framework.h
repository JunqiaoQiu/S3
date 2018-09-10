#ifndef S3FRAMEWORK_H
#define S3FRAMEWORK_H

#include <stdlib.h>
#include <stdio.h>

#include "Resources.h"
#include "Predictor.h"

namespace s3
{
	class ArchitecturePropertyCollector
	{
	public:
		ArchitecturePropertyCollector();
		ArchitecturePropertyCollector(int threads_);
		~ArchitecturePropertyCollector();

		void ExecuteTrain(const microspec::Table* table_, const microspec::Input* inputs_);
		int GetTrainTimes() const;
		int GetNumThreads() const;
		double* GetAlphaPointer();
		double GetAlpha(int threads_) const;
		double GetGama() const;

	protected:
		static void* caller(void* args);

	private:
		int mthreads;
		int mtraintime;

		// resource contention factor
		double* malpha;
		// relative speed factor
		double mgama;

	};



}	// end of namespace s3

#endif // S3FRAMEWORK_H
