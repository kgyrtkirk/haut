#pragma once

class STK500Emulator {
	PtyChannel &pty;
	SerialChannel &sp;
	int	error;

public:
	STK500Emulator(PtyChannel &_pty,SerialChannel &_sp) : pty(_pty),sp(_sp){
		error=0;
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
		      BOOST_LOG_TRIVIAL(info)<< "offset added";
		      address=newAddress+16128;
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
		      uint16_t length0=length;

//		      cmd.length=length;
		      uint8_t memtype = getch();

		      // read a page worth of contents
		      bufPtr = buffer;//cmd.buffer;
		      do *bufPtr++ = getch();
		      while (--length);

		      // Read command terminator, start reply
		      verifySpace();

//		      write
		      write_mem(memtype,buffer,address,length0);
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

	void write_mem(uint8_t memtype,uint8_t*buf,uint16_t addrss,uint16_t length){
		BOOST_LOG_TRIVIAL(info)<< "write@" << addrss << ":" << length;
		char s[1110];
		usleep(100);
		sp.resetCap();
		sp.read0();
		int l=sprintf(s,"W%d ",addrss);
		sp.write0(s,l);
		l=0;
		usleep(1000);
		for(int i=0;i<length;i++){
			int n=sprintf(s+l,"%02x",buf[i]);
			l+=n;
//			sp.write0(s,2);
		}
		sp.write0(s,l);
		sp.write0("\r\n",2);
		sp.resetCap();


		int	t=10000;
		while(!sp.read0() && t > 0) {
			usleep(10);
//			t--;
		}
		if(sp.hasResult()){
			string res=sp.getResult();
			BOOST_LOG_TRIVIAL(error)<< "res: " << res;
			BOOST_LOG_TRIVIAL(error)<< "l: " << res.length();;
		}else{
			BOOST_LOG_TRIVIAL(error)<< "nores!" ;
			error=3;
		}
	}
	void read_mem(uint8_t memtype,uint16_t addrss,uint16_t length){
		BOOST_LOG_TRIVIAL(info)<< "read@" << addrss << " :: " << length;
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
			error=3;
		}

	}
};

