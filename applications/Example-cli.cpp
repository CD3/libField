#include <iostream>
#include <fstream>
#include<boost/program_options.hpp>

#include "Example.hpp"

namespace po = boost::program_options;
using namespace std;

void print_manual()
{
  // we are using a raw string literal here (the R"()" syntax, which is a C++11 feature.
  // it allows for a multi-line string to be written directly, instead of needing to quote
  // each line or insert \n characters.
  std::cout << 
  R"(
Write a short "manual" for the CLI here. This function will be called to when the user gives the --manual option.
You do not need to summarize command line options here, that will be done by the boost library.
    )"
  << std::endl;
}

int main(int argc, const char** argv)
{
    // we usually say that a command line program takes options and arguments.
    // boost calls arguments "positional options", which are just options that can
    // be given without their option name. to create the argument, we define it as
    // an option first, and then tell boost that it is "positional"
    //
    // so to define our command line interface
    // we declare options and arguments that our application will accept.

    // define our regular options. these will be given with a '--' or '-' in front. '--' is used
    // for the long name, '-' is used for the short name.
    po::options_description opt_options("Options");
    opt_options.add_options()
      // these are simple flag options, they do not have an argument.
      ("help,h",    "print help message.")
      ("version", "print library version.")
      ("manual",    "print manual.")
      ("verbose,v", po::value<int>()->implicit_value(0), "verbose level.") // an option that takes an argument, but has a default value.
      ("flag,f",    "A simple flag that takes no argument.")
      ("option-with-arg,o", po::value<std::string>(), "An option that takes an argument.")

      ;

    // now define our arguments.
    po::options_description arg_options("Arguments");
    arg_options.add_options()
      ("argument-1"  , po::value<std::string>(),   "The first argument.") // an option that can be given once
      ("argument-remaining"  , po::value<std::vector<std::string>>()->composing(),   "All remaining arguments."); // an option that can be given multiple times with each argument getting stored in a vector.

    // combine the options and arguments into one option list.
    // this is what we will use to parse the command line, but
    // when we output a description of the options, we will just use
    // opt_options
    po::options_description all_options("Options");
    all_options.add(opt_options).add(arg_options);

    // tell boost how to translate positional options to named options
    po::positional_options_description args;
    args.add("argument-1", 1); // first argument will be mapped to argument-1
    args.add("argument-remaining", -1);  // remaining arguments will be mapped to argument-remaining

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).  options(all_options).positional(args).run(), vm);
    po::notify(vm);


    // boiler plate stuff

    if( vm.count("help") )
    {
      // print out a usage statement and summary of command line options
      std::cout << "Example-cli [options] <file>" << "\n\n";
      std::cout << opt_options << "\n";
      return 0;
    }

    if( vm.count("manual") )
    {
      // print the manual
      print_manual();
      return 0;
    }

    if( vm.count("version") )
    {
      // print the version number for the library
      std::cout << "Example "<<Example_VERSION_FULL << std::endl;
      return 0;
    }


    // start application
    //
    //
    // below is an example of how to use the command line parser to access the options and arguments that
    // where passed.
    
    if( vm.count("flag") )
      std::cout << "'flag' was given." << std::endl;
    else
      std::cout << "'flag' NOT was given." << std::endl;

    if( vm.count("option-with-arg") )
      std::cout << "'option-with-arg' was given. value: " << vm["option-with-arg"].as<std::string>() << std::endl;
    else
      std::cout << "'option-with-arg' NOT was given." << std::endl;

    if( vm.count("argument-1") )
      std::cout << "'argument-1' was given. value: " << vm["argument-1"].as<std::string>() << std::endl;
    else
      std::cout << "'argument-1' NOT was given." << std::endl;

    if( vm.count("argument-remaining") )
    {
      std::cout << "'argument-remaining' was given. " << vm["argument-remaining"].as<std::vector<std::string>>().size() << " values:" << std::endl;
      for(int i = 0; i < vm["argument-remaining"].as<std::vector<std::string>>().size(); i++)
        std::cout<<"\t"<<vm["argument-remaining"].as<std::vector<std::string>>()[i]<<std::endl;
    }
    else
      std::cout << "'argument-remaining' NOT was given." << std::endl;
    





    return 0;
}
