#pragma once

class SerialChannel {

	char	buffer[BUF_SIZ+1];
	char	line[BUF_SIZ+1];
	int		bpos;
	string	res;
	bool	hasRes;


	int fd; // file description for the serial port
	void configurePort() {
		BOOST_LOG_TRIVIAL(info)<< "configure port";
		struct termios port_settings; // structure to store the port settings in

		tcgetattr(fd, &port_settings);

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
	SerialChannel(const char*dev) {
bpos=0;
		BOOST_LOG_TRIVIAL(info)<< "opening serial: " << dev;
		fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
		BOOST_LOG_TRIVIAL(info)<< "putting exclusive lock onto: " << dev;

		if(flock(fd,LOCK_EX | LOCK_NB) != 0){
			BOOST_LOG_TRIVIAL(info)<< "error locking: " << dev;
			exit(2);
		}

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

