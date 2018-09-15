#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "Resources.h"
#include "MicroSpec.h"
#include "MicroSpecPthread.h"
#include "Timer.h"
#include "s3framework.h"
#include "s3verifier.h"

using namespace std;
using namespace microspec;
using namespace s3;

int main(int argc, char* argv[])
{
	char* InputLibFile = argv[1];
	char* TableFile = argv[2];
	char* AcceptFile = argv[3];
	int start = atoi(argv[4]);
	int numThreads = atoi(argv[5]);
	MappingRule* rules_;

	if (argc > 6)
		rules_ = MappingRule::defineMappingRule(argv[6]);
	else
		rules_ = MappingRule::defineMappingRule();

	Timer T1;
	startTime(&T1);
	Table* table_ = Table::readFromFile(TableFile, AcceptFile, start, rules_);
	stopTime(&T1);
	double datatime =  elapsedTime(T1);	
	cout << "Data construct time : "<< datatime << " s"<< endl << endl;

	long testLength = 300000000;
	S3RunTimeController* objController = new S3RunTimeController(numThreads, testLength);

	objController->startOfflineProfile(table_, InputLibFile, rules_);
	objController->startModelConstruction("M1+");

	cout << "Optimal Configuration is " << objController->getOptConfiguration() << endl;
	cout << "Optimal Performance is " << objController->getOptPerformance() << endl;
	cout << "Speedups are " << endl;
	for (int i = 0; i < numThreads; i++)
		cout << (objController->getSpeedupArrays())[i] << " ";
	cout << endl;

	objController->printSpeedups("outPutAnalysis.out");

	return 0;
}
	