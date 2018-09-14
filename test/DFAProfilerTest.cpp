#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "Resources.h"
#include "MicroSpec.h"
#include "MicroSpecPthread.h"
#include "Timer.h"
#include "s3framework.h"

using namespace std;
using namespace microspec;
using namespace s3;

int main(int argc, char* argv[])
{
	char* InputFile = argv[1];
	char* TableFile = argv[2];
	char* AcceptFile = argv[3];
	int start = atoi(argv[4]);
	MappingRule* rules_;

	if (argc > 5)
		rules_ = MappingRule::defineMappingRule(argv[5]);
	else
		rules_ = MappingRule::defineMappingRule();

	Timer T1;
	startTime(&T1);
	//Input* inputs_ = Input::readFromFile(InputFile, rules_);
	Table* table_ = Table::readFromFile(TableFile, AcceptFile, start, rules_);
	stopTime(&T1);
	double datatime =  elapsedTime(T1);	
	cout << "Data construct time : "<< datatime << " s"<< endl << endl;

	DFAPropertyCollector* objCollector = new DFAPropertyCollector();

	//objCollector->executeProfilingOnOneInput(table_, inputs_);
	objCollector->executeProfilingOnTestLibs(table_, InputFile, rules_);

	cout << "Detail Pool " << endl;
	for (int i = 0; i < objCollector->getCurrentSamples(); i++)
		cout << (objCollector->getConvergencePool())[i] << " ";
	cout << endl;

	cout << "Current Samples " << objCollector->getCurrentSamples() << endl;
	cout << "Pool Size is " << objCollector->getSamplePoolSize() << endl;
	cout << "Samples on One Input " << objCollector->getSamplePerInput() << endl;
	cout << "Prediction Accuracy " << objCollector->getPredictionAccuracy() << endl;
	cout << "Average CL " << objCollector->getAverageConvergenceLength() << endl;

	return 0;
}
	