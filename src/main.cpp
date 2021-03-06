
#include <thread>
#include <string>
#include "cxxopts.hpp"
#include "Resources.h"
#include "s3framework.h"
#include "s3verifier.h"

using namespace std;
using namespace microspec;
using namespace s3;

struct arguments 
{
    std::string table, accept, input, output;
    int benchmark, mode;
    unsigned int cores, threads;
};

static void init_options(cxxopts::Options& options, arguments& args) 
{
    options
        .positional_help("[table accept input output]")
        .show_positional_help();
    options.add_options("")
        ("t,table", "table file ", cxxopts::value<std::string>(args.table))
        ("a,accept", "accept state file ", cxxopts::value<std::string>(args.accept))
        ("i,input", "training input file ", cxxopts::value<std::string>(args.input))
        ("o,output", "output file for the final code", cxxopts::value<std::string>(args.output))
        ("b,benchmark", "Choose the kind of benchmark (token version, global) ", cxxopts::value<int>(args.benchmark))
        ("m,mode", "Choose the mode (which means model version, global)", cxxopts::value<int>(args.mode))
        ("c,cores", "The number of logical processing units (not global)", cxxopts::value<unsigned int>(args.cores))
        ("x,threads", "The number of user-defined threads (not global)", cxxopts::value<unsigned int>(args.threads))
        ("h,help", "Print help")
        ;
    
    options.parse_positional({"table", "accept", "input", "output"});
}

int main(int argc, char **argv)
{
    unsigned int core_num = std::thread::hardware_concurrency();
    if (core_num == 0) core_num = 1;

    int argcount = argc;
    arguments args = {"", "", "", "", 0, 0, core_num, core_num};

    cxxopts::Options options(argv[0], "S3 Analyzer");
    try
    {
        init_options(options, args); 
        auto result = options.parse(argc, argv); // it will change argc
        
        if (result.count("help") || argcount < 4)
        {
            std::cout << options.help({""}) << std::endl;
            return 0;
        }

    } 
    catch (const cxxopts::OptionException& e) 
    {
        std::cout << "Illegal arguments." << std::endl;
        std::cout << "error parsing options: " << e.what() << std::endl;
        std::cout << options.help({""}) << std::endl;
        return 1;
    }

    const char* tableFile = args.table.c_str();
    const char* acceptFile = args.accept.c_str();
    const char* inputFile = args.input.c_str();

    MappingRule* rules_;
    if (args.benchmark == 0)
		rules_ = MappingRule::defineMappingRule();
    Table* table_ = Table::readFromFile(tableFile, acceptFile, 0, rules_);
    S3RunTimeController* objController = new S3RunTimeController(args.cores, 300000000);
    objController->startOfflineProfile(table_, inputFile, rules_);

    if (args.mode == 2)
    	objController->startModelConstruction("M1+");
    else if (args.mode == 3)
    	objController->startModelConstruction("M2");
    else if (args.mode == 4)
    	objController->startModelConstruction("M2+");
    else
    	objController->startModelConstruction("M1");

    return 0;
}


