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
	Input* inputs_ = Input::readFromFile(InputFile, rules_);
	Table* table_ = Table::readFromFile(TableFile, AcceptFile, start, rules_);
	stopTime(&T1);
	double datatime =  elapsedTime(T1);	
	cout << "Data construct time : "<< datatime << " s"<< endl << endl;

	ArchitecturePropertyCollector* arc;
	arc = new ArchitecturePropertyCollector(4);
	for (int temp = 0; temp < 1; temp++)
		arc->executeTrain(table_, inputs_);

	double* alpha;
	alpha = arc->getAlphaPointer();
	cout << endl << "Alpha is " << endl;
	for (int i = 0; i < 4; i++)
		cout << alpha[i] << endl;
	cout << "Gamma is "<< arc->getGama() << endl;

	return 0;
}
	