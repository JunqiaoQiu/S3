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

	S3Verifier* objVerifier = S3Verifier::constructS3Verifier(numThreads);
	objVerifier->startVerification(table_, InputLibFile, rules_);
	objVerifier->printSpeedup2File("Output.out");

	return 0;
}
	