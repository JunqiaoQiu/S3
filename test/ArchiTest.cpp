#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "Resources.hpp"
#include "MicroSpec.hpp"
#include "MicroSpec_Pthread.hpp"
#include "Timer.hpp"
#include "s3framework.hpp"

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

	rules_ = MappingRule::DefineMappingRule();

	cout << endl << "No User Actions!" << endl;
	Timer T1;
	startTime(&T1);
	Input* inputs_ = Input::ReadFromFile(InputFile, rules_);
	Table* table_ = Table::ReadFromFile(TableFile, AcceptFile, start, rules_);
	stopTime(&T1);
	double datatime =  elapsedTime(T1);	
	cout << "Data construct time : "<< datatime << " s"<< endl;

	startTime(&T1);
	DFA* obj_unroll_single = new Spec_DFA_Pthread(100, 4);
	obj_unroll_single->Run(table_, inputs_);
	stopTime(&T1);
	double SpecTime = elapsedTime(T1);

	startTime(&T1);
	DFA* obj_seq = new Seq_DFA();
	obj_seq->Run(table_, inputs_);
	stopTime(&T1);
	double SeqTime =  elapsedTime(T1);

	cout << "SeqDFA Execution "<< SeqTime << " s " << endl;
	cout << "SpecDFA Execution "<< SpecTime << " s " << endl;
	cout << "SpecDFA Speedup " <<  SeqTime / SpecTime << endl;	

	ArchitectureEffect* arc;
	arc = new ArchitectureEffect(4);
	for (int temp = 0; temp < 4; temp++)
		arc->ExecuteTrain(table_, inputs_);

	double* alpha;
	alpha = arc->GetAlphaPointer();
	for (int i = 0; i < 4; i++)
		cout << alpha[i] << endl;
	cout << arc->GetGama() << endl;

	return 0;
}
	