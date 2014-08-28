#include "SpliceStandalone.h"
#include "MainWindow.h"

#include <iostream>
#include <sstream>
#include <istream>
#include <ostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <boost/filesystem.hpp>

using namespace FabricSplice;

int main(int argc, char *argv[])
{
	SpliceStandalone app(argc, argv);

	std::string klFilePath;

	po::options_description desc("Splice Standalone");
	desc.add_options()
	 ("help,h", "produce help message")
	 ("klFile,f", po::value<std::string>(&klFilePath), "filePath of the KL file to execute");

	po::positional_options_description p;
	p.add("klFile", 1);

	po::variables_map vm;
	try
	{
		po::store(po::parse_command_line(argc, argv, desc, po::command_line_style::unix_style ^ po::command_line_style::allow_short), vm);
		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
		po::notify(vm);
	}
	catch( std::exception &e )
	{
		std::cout << desc << std::endl;
		return false;
	}
	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return EXIT_SUCCESS;
	}

	if (!klFilePath.empty())
		app.addWrapper(klFilePath);

	app.showMainWindow();
	app.setupFusionLook();

	// Blocks until application decides to quit.
	app.exec();
	
	
	return EXIT_SUCCESS;
}

