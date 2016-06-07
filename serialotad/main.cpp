#include <poll.h>
#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include <pty.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <errno.h>

#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

//#include <boost/iostreams/device/file_descriptor.hpp>
//#include <boost/iostreams/stream.hpp>
//
//namespace io = boost::iostreams;
namespace po = boost::program_options;
using namespace std;

const int BUF_SIZ =1024;

class LocalPty {
	int fd, fds;
	char n[100];
public:
	LocalPty() {
		struct termios tp;
		struct termios *termios_p = &tp;
		termios_p->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR
				| IGNCR | ICRNL | IXON);
//		termios_p->c_iflag |= IGNBRK;
		termios_p->c_oflag &= ~OPOST ;
		termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
//		termios_p->c_lflag=0;
		termios_p->c_cflag &= ~(CSIZE | PARENB);
		termios_p->c_cflag |= CS8;
//		termios_p->c_cc[VMIN]=0;
//		termios_p->c_cc[VTIME]=1;


		BOOST_LOG_TRIVIAL(debug)<< "allocating pty";
		int ret = openpty(&fd, &fds, n, termios_p, 0);
		BOOST_LOG_TRIVIAL(debug)<< "openpty: " <<ret;
		if (ret != 0) {
			throw std::runtime_error("openpty error");
		}
		BOOST_LOG_TRIVIAL(info)<< "pty allocated: "<< n;
//		int flags = fcntl(fd, F_GETFL, 0);
//		fcntl(fd, F_SETFL, O_NONBLOCK);


	}

	void run() {
		printf("%s\n",n);
		while(1) {
			read(fd,n,1);
			printf("%x ",n[0]);
			fflush(stdout);
			//		write(fd,n,1);
			//		write(fd,"\n",1);
			//		read(fd,n,1);
			//		sleep(1000);
		}
	}

	void createLink(const char*linkpath) {
		struct stat buf;
		if(lstat(linkpath,&buf) == 0) {
			if((buf.st_mode & S_IFLNK) != S_IFLNK) {
				BOOST_LOG_TRIVIAL(error)<< "exists, and is not a link: " << linkpath;
				throw std::runtime_error("exists; wont unlink");
			}
			BOOST_LOG_TRIVIAL(error)<< "unlinking: " << linkpath;
			if(0!=unlink(linkpath)) {
				throw std::runtime_error("error; unlink");
			}
		}
		BOOST_LOG_TRIVIAL(info)<< "creating link: " << linkpath;
		if(0!=symlink(n,linkpath)) {
			throw std::runtime_error("error; symlink");
		}
	}
	char	buffer[BUF_SIZ+1];
	int		bpos;

	void fill(){
		fd_set	fdr;
		FD_ZERO(&fdr);
		FD_SET(fd,&fdr);
//		select(fd+1,)
//		usleep(100000000);
//		struct pollfd pofd;
		struct timeval tv;		tv.tv_sec=0;	tv.tv_usec=10000;
//		pofd.fd=fd;
//		pofd.events=1;
//		pofd.revents=0;
//		if(poll(&pofd,1,100)<=0){
//			return;
//		}
		if(select(fd+1,&fdr,NULL,NULL,&tv)<=0){
			return;
		}

		int ret=read(fd,buffer+bpos,BUF_SIZ-bpos);
		if(ret<0 && errno==EAGAIN){
			return;
		}
		if(ret<0){
			BOOST_LOG_TRIVIAL(info)<< "err:  " << ret;
			throw std::runtime_error("error reading");
		}
		bpos+=ret;

	}

	bool hasChar(){
		if(bpos==0){
			fill();
		}
		return bpos>0;
	}
	uint8_t	readChar(){
		if(bpos==0){
			fill();
		}
		if(bpos>0){
			uint8_t ret=buffer[0];
			memmove(buffer,buffer+1,BUF_SIZ);
			bpos--;
			return ret;
		}else{
			return -1;
		}
	}
	void writeChar(uint8_t c){
		write(fd,&c,1);
	}

};


class SerialPort {

	char	buffer[BUF_SIZ+1];
	char	line[BUF_SIZ+1];
	int		bpos;
	string	res;
	bool	hasRes;


	int fd; // file description for the serial port
	void configurePort() {
		BOOST_LOG_TRIVIAL(info)<< "configure port";
		struct termios port_settings; // structure to store the port settings in

		cfsetispeed(&port_settings, B115200);    // set baud rates
		cfsetospeed(&port_settings, B115200);

		port_settings.c_cflag &= ~PARENB; // set no parity, stop bits, data bits
		port_settings.c_cflag &= ~CSTOPB;
		port_settings.c_cflag &= ~CSIZE;
		port_settings.c_cflag |= CS8;

		struct termios &toptions=port_settings;
		/* no hardware flow control */
		 toptions.c_cflag &= ~CRTSCTS;
		 /* enable receiver, ignore status lines */
		 toptions.c_cflag |= CREAD | CLOCAL;
		 /* disable input/output flow control, disable restart chars */
		 toptions.c_iflag &= ~(IXON | IXOFF | IXANY);
		 /* disable canonical input, disable echo,
		 disable visually erase chars,
		 disable terminal-generated signals */
		 toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
		 /* disable output processing */
		 toptions.c_oflag &= ~OPOST;

		tcsetattr(fd, TCSANOW, &port_settings); // apply the settings to the port
	}
public:
	SerialPort(const char*dev) {
bpos=0;
		BOOST_LOG_TRIVIAL(info)<< "opening serial: " << dev;
		fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);

		if (fd == -1) {
			throw std::runtime_error("can't open serial port");
		} else {
			fcntl(fd, F_SETFL, 0);
		}
		configurePort();

	}

	int	read0(){

		int k=read(fd,buffer+bpos,BUF_SIZ-bpos);
		if(k<0){
			throw std::runtime_error("error reading?");
		}
		bpos+=k;
		buffer[BUF_SIZ]=0;

		char*ptr;
		while((ptr=(char*)memchr(buffer,'\n',bpos))!=0){
			int	l=ptr-buffer;
			memcpy(line,buffer,ptr-buffer);
			line[l]=0;
			if(line[0] == '#'){
				BOOST_LOG_TRIVIAL(debug)<< "on serial: "<<line;
			}else{
				hasRes=true;
				res=line;
				BOOST_LOG_TRIVIAL(debug)<< "serial ret: "<<line;
			}
			memmove(buffer,ptr+1,bpos-l);
			bpos-=l;
			bpos--;

			if(hasRes){
				return 1;
			}
		}
		return 0;
	}
	void write0(char*b,int l){
		int k=write(fd,b,l);
		if(k<0){
			throw std::runtime_error("error writing!");
		}
		if(k!=l){
			throw std::runtime_error("short write!");
		}
	}

	bool resetCap(){
		hasRes=false;
	}

	bool hasResult(){
		return hasRes;
	}
	string getResult(){
		return res;
	}
};

#include "stk500.h"
class STK500Emulator {
	LocalPty &pty;
	SerialPort &sp;
	int	error;

public:
	STK500Emulator(LocalPty &_pty,SerialPort &_sp) : pty(_pty),sp(_sp){

	}
	void verifySpace(){
		if(pty.readChar()!=' '){
			BOOST_LOG_TRIVIAL(info)<< "error...expected space...";
			error=1;
		}
		pty.writeChar(STK_INSYNC);
	}
	  void putch(uint8_t c){
			pty.writeChar(c);
	  }
	  uint8_t getch(){
			return pty.readChar();
	  }
      void getNch(uint8_t count) {
        do getch(); while (--count);
        verifySpace();
      }

	void run(){
		int address;
		uint8_t	buffer[BUF_SIZ];
		error=0;
//		BOOST_LOG_TRIVIAL(info)<< "entering stk500 emulator";
		  /* Forever loop: exits by causing WDT reset */
		bool first=true;
		  while(error==0) {
			  sp.resetCap();
			  sp.read0();
		    /* get character from UART */
			  if(!pty.hasChar())
				  break;
		    uint8_t ch = pty.readChar();
			  if(first){
					BOOST_LOG_TRIVIAL(info)<< "entered stk500 for first cmd : " << ch;
					first=false;
			  }

		    if(ch == STK_GET_PARAMETER) {
		      uint8_t which = pty.readChar();
		      verifySpace();
		      /*
		       * Send optiboot version as "SW version"
		       * Note that the references to memory are optimized away.
		       */
		      if (which == 0x82) {
		    	  putch(0xaa);
		      } else if (which == 0x81) {
		    	  putch(0xbb);
		      } else {
			/*
			 * GET PARAMETER returns a generic 0x03 reply for
		         * other parameters - enough to keep Avrdude happy
			 */
			putch(0x03);
		      }
		    }
		    else if(ch == STK_SET_DEVICE) {
		      // SET DEVICE is ignored
		      getNch(20);
		    }
		    else if(ch == STK_SET_DEVICE_EXT) {
		      // SET DEVICE EXT is ignored
		      getNch(5);
		    }
		    else if(ch == STK_LOAD_ADDRESS) {
		      // LOAD ADDRESS
		      uint16_t newAddress;
		      newAddress = getch();
		      newAddress = (newAddress & 0xff) | (getch() << 8);
		#ifdef RAMPZ
		      // Transfer top bit to RAMPZ
		      RAMPZ = (newAddress & 0x8000) ? 1 : 0;
		#endif
		      newAddress += newAddress; // Convert from word address to byte address
		      address=newAddress;
//		      cmd.address = newAddress;
		      verifySpace();
		    }
		    else if(ch == STK_UNIVERSAL) {
		      // UNIVERSAL command is ignored
		      getNch(4);
		      putch(0x00);
		    }
		    /* Write memory, length is big endian and is in bytes */
		    else if(ch == STK_PROG_PAGE) {
		      // PROGRAM PAGE - we support flash programming only, not EEPROM
		      uint8_t desttype;
		      uint8_t *bufPtr;
		      uint16_t length;
		      length = getch()<<8; length |= getch();

//		      cmd.length=length;
		      uint8_t memtype = getch();

		      // read a page worth of contents
		      bufPtr = buffer;//cmd.buffer;
		      do *bufPtr++ = getch();
		      while (--length);

		      // Read command terminator, start reply
		      verifySpace();

//		      write
//		      stk500service(cmd.memtype, cmd.buffer, cmd.address, cmd.length);


		    }
		    /* Read memory block mode, length is big endian.  */
		    else if(ch == STK_READ_PAGE) {
		      uint8_t desttype;
		      uint16_t length;
		      length = getch()<<8; length |= getch();

		      uint8_t memtype = getch();

		      verifySpace();

		      read_mem(memtype, address, length);
		    }

		    /* Get device signature bytes  */
		    else if(ch == STK_READ_SIGN) {
		      // READ SIGN - return what Avrdude wants to hear
		      verifySpace();

#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x95
#  define SIGNATURE_2 0x0F

putch(SIGNATURE_0);
		      putch(SIGNATURE_1);
		      putch(SIGNATURE_2);
		    }
		    else if (ch == STK_LEAVE_PROGMODE) { /* 'Q' */
		      // Adaboot no-wait mod
//		      watchdogConfig(WATCHDOG_16MS);
				BOOST_LOG_TRIVIAL(info)<< "leaving program mode - requested";
		      error=1;
		      verifySpace();
		    }
		    else {
		      // This covers the response to commands like STK_ENTER_PROGMODE
		      verifySpace();
		    }
		    putch(STK_OK);
		  }
//		  BOOST_LOG_TRIVIAL(info)<< "leaving stk500 emulator";
	}
	// borrowed from: https://jdale88.wordpress.com/2009/09/24/c-anything-tofrom-a-hex-string/
	// thank you ;)
	// ------------------------------------------------------------------
	/*!
	    Convert a hex string to a block of data
	*/
	void fromHex(
	    const std::string &in,              //!< Input hex string
	    void *const data                    //!< Data store
	    )
	{
	    size_t          length      = in.length();
	    unsigned char   *byteData   = reinterpret_cast<unsigned char*>(data);

	    std::stringstream hexStringStream; hexStringStream >> std::hex;
	    for(size_t strIndex = 0, dataIndex = 0; strIndex < length; ++dataIndex)
	    {
	        // Read out and convert the string two characters at a time
	        const char tmpStr[3] = { in[strIndex++], in[strIndex++], 0 };

	        // Reset and fill the string stream
	        hexStringStream.clear();
	        hexStringStream.str(tmpStr);

	        // Do the conversion
	        int tmpValue = 0;
	        hexStringStream >> tmpValue;
	        byteData[dataIndex] = static_cast<unsigned char>(tmpValue);
	    }
	}

	void read_mem(uint8_t memtype,uint16_t addrss,uint16_t length){
		BOOST_LOG_TRIVIAL(info)<< "read@" << addrss;
		char s[111];
		usleep(100);
		sp.resetCap();
		sp.read0();
		int l=sprintf(s,"R%d\r\n",addrss);
		sp.write0(s,l);
		sp.resetCap();

		int	t=1000;
		while(!sp.read0() && t > 0) {
			usleep(10);
//			t--;
		}
		if(sp.hasResult()){
			string res=sp.getResult();
			BOOST_LOG_TRIVIAL(error)<< "res: " << res;
			BOOST_LOG_TRIVIAL(error)<< "l: " << res.length();;
			int	l=res.length()/2;
			uint8_t	payload[BUF_SIZ];
			fromHex(res,payload);
			l=128;
			for(int i=0;i<l;i++)
				pty.writeChar(payload[i]);

		}else{
			BOOST_LOG_TRIVIAL(error)<< "nores!" ;
		}

	}
};

int main(int ac, char**av) {

	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()("help", "produce help message");
	desc.add_options()("compression", po::value<int>(),
			"set compression level");

	po::variables_map vm;
	po::store(po::parse_command_line(ac, av, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		cout << desc << "\n";
		return 1;
	}

	SerialPort sp("/dev/arduino_mega_75237333536351E00181");
	LocalPty pt1;

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
