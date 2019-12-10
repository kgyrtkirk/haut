#include "common.h"
#include <vector>
#include <string>
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


int main(int ac, char**av) {

	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()("help", "produce help message");
	desc.add_options()("compression", po::value<int>(),
			"set compression level");
	int debugLevel;
//	std::vector<int>	remotes;

	desc.add_options()("debug,d",po::value<int>(&debugLevel)->default_value(0),"debug level");
	desc.add_options()("target,t",po::value< vector<int> >()->multitoken(),"target dev channels");

	po::variables_map vm;
	po::store(po::parse_command_line(ac, av, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		cout << desc << "\n";
		return 1;
	}
	if (vm.count("target")==0) {
		throw std::runtime_error("no target specified");
	}

	boost::log::core::get()->set_filter (
	    boost::log::trivial::severity >= boost::log::trivial::warning-debugLevel
	    );

	SerialChannel sp("/dev/arduino_mega_75237333536351E00181");
//	PtyChannel pt1;

	vector<PtyChannel>	ptys;
	vector<int> remotes=vm["target"].as<vector<int>>();
	for(auto it=remotes.begin();it!=remotes.end();it++){
		PtyChannel	p(*it);
		string	linkPath="_"+boost::lexical_cast<string>(*it);
		p.createLink(linkPath.c_str());
		ptys.push_back(p);
	}

//	STK500Emulator	stk500(pt1,sp);
//	pt1.createLink("_ota1");


	while(true){

		// let the steam out..
		sp.resetCap();
		sp.read0();

		for(auto it=ptys.begin();it!=ptys.end();it++){
			if(it->hasChar()){
				STK500Emulator	stk500(*it,sp);
				stk500.run();
			}
		}
//		stk500.run();


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
