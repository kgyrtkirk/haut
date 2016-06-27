#include <poll.h>
#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include <pty.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <errno.h>

#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>

//#include <boost/iostreams/device/file_descriptor.hpp>
//#include <boost/iostreams/stream.hpp>
//
//namespace io = boost::iostreams;
namespace po = boost::program_options;
using namespace std;

const int BUF_SIZ =1024;

#include "PtyChannel.h"
#include "SerialChannel.h"


#include "stk500.h"
#include "Stk500Emulator.h"

#include <boost/log/expressions.hpp>

int main(int ac, char**av) {

	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()("help", "produce help message");
	desc.add_options()("compression", po::value<int>(),
			"set compression level");
	int debugLevel;
	desc.add_options()("debug,d",po::value<int>(&debugLevel)->default_value(0),"debug level");


	po::variables_map vm;
	po::store(po::parse_command_line(ac, av, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		cout << desc << "\n";
		return 1;
	}

	boost::log::core::get()->set_filter (
	    boost::log::trivial::severity >= boost::log::trivial::warning-debugLevel
	    );

	SerialChannel sp("/dev/arduino_mega_75237333536351E00181");
	PtyChannel pt1;

	STK500Emulator	stk500(pt1,sp);
	pt1.createLink("_ota1");


	while(true){

		// let the steam out..
		sp.resetCap();
		sp.read0();

		stk500.run();


//		rea
//		fflush(stdout);

	}

//	if (vm.count("compression")) {
//		cout << "Compression level was set to " << vm["compression"].as<int>()
//				<< ".\n";
//	} else {
//		cout << "Compression level was not set.\n";
//	}

	return 0;
}
