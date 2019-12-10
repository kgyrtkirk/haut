#pragma once

class PtyChannel {
	int fd, fds;
	char n[100];
	int channelId;
public:
	PtyChannel(int _channelId) {
		channelId=_channelId;
		bpos=0;
		struct termios tp;
		struct termios *termios_p = &tp;
		memset(&tp,0,sizeof(tp));
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


	}
	int	getChannelId(){
		return channelId;
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

