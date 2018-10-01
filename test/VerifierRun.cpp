/*
	* This program is used to get the corresponding Speculative FSM 
	* executionm time with different configurations on the target 
	* platforms.
*/
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
	char* InputLibFile = argv[1]; // The files with the path to all test inputs
	char* TableFile = argv[2];
	char* AcceptFile = argv[3];
	int start = atoi(argv[4]);
	int numThreads = atoi(argv[5]);
	int repeatCounts = atoi(argv[6]);
	MappingRule* rules_;

	// If not setting the mapping rule, use the default ones
	if (argc > 7)
		rules_ = MappingRule::defineMappingRule(argv[7]);
	else
		rules_ = MappingRule::defineMappingRule();

	Timer T1;
	startTime(&T1);
	Table* table_ = Table::readFromFile(TableFile, AcceptFile, start, rules_);
	stopTime(&T1);
	double datatime =  elapsedTime(T1);	
	cout << "Data construct time : "<< datatime << " s"<< endl << endl;

	S3Verifier* objVerifier = new S3Verifier(numThreads, repeatCounts); //S3Verifier::constructS3Verifier(numThreads);
	objVerifier->setAction("accumulate");
	objVerifier->startVerification(table_, InputLibFile, rules_);
	objVerifier->printSpeedup2File("Output.out");
	cout << "Average Sequential Execution Time is " << objVerifier->getSequentialTime() << " s" << endl;

	return 0;
}
	