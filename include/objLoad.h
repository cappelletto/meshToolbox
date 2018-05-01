/**
 * @file options.h
 * @brief Argument parser options based on args.hxx
 * @version 1.0
 * @date 20/01/2018
 * @author Jose Cappelletto
 */

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <iostream>
#include "../3rdparty/args.hxx"

args::ArgumentParser 	argParser("","");
args::HelpFlag 	argHelp(argParser, "help", "Display this help menu", {'h', "help"});
//CompletionFlag completion(cliParser, {"complete"});	//TODO: figure out why is missing in current version of args.hxx
args::Positional<std::string> 	argInput(argParser, "input", "Input mesh file name");
args::Positional<std::string> 	argOutput(argParser, "output", "Input mesh file name. Currently under developement");

#endif
