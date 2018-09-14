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

	ArchitecturePropertyCollector* objArc;
	int threadsNum = 4; 
	int repeatTime = 5; 
	objArc = new ArchitecturePropertyCollector(threadsNum);
	// for (int temp = 0; temp < 1; temp++)
	// 	objArc->executeTrain(table_, inputs_);
	objArc->repeatExecuteTrain(table_, InputFile, rules_, repeatTime);

	cout << "The training times is  " << objArc->getTrainTimes() << endl;
	cout << "The training length is " << objArc->getTrainingLength() << endl;
	cout << "The number of threads is " << objArc->getNumThreads() << endl;
	cout << "Gama is " << objArc->getGama() << endl;
	cout << "Average Seq time is " << objArc->getAverageSeqTime() << " s" << endl;
	cout << "Showing Alpha " << endl;
	for (int i = 0; i < objArc->getNumThreads(); i++)
		cout << i+1 << " " << objArc->getAlpha(i+1) << endl;
 	cout << endl;


	return 0;
}
	