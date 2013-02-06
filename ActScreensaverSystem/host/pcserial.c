/*
------------------------------------------------------------------------------ 
pcserial.c  
Read data from Arduino through serial port
by Steven Kim
------------------------------------------------------------------------------ 
*/
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/signal.h> 
#include <sys/ioctl.h> 
#include <sys/poll.h>  
#include <termios.h> 

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>


#define LOGON 1
#define LOGOFF 0

int status = LOGON;

void press_button()
{
        Display *d;
        d = XOpenDisplay(NULL);
        printf("press_button\n");
        if(d == NULL)
        {
                printf("Error open display\n");
        }
        if (status) {
                system("gnome-screensaver-command -a");
                status = LOGOFF;
        }
        else {
                system("gnome-screensaver-command -d");
                status = LOGON;
        }
            
        XFlush(d);
        XCloseDisplay(d);
}


/*
------------------------------------------------------------------------------ 
open serial port
Warnning: 
no control RTS/CTS
No store ex-status after opening serial port
-----------------------------------------------------------------------------
*/
int open_serial(char *dev_name, int baud, int vtime, int vmin) 
{ 
	int fd; 
	struct termios  newtio; 

	//open serial port
	fd = open(dev_name, O_RDWR | O_NOCTTY); 
	if (fd < 0)  
	{ 
		printf("Device OPEN FAIL %s\n", dev_name); 
		return  -1; 
	} 
	//init and set up serial port env
	memset(&newtio, 0, sizeof(newtio)); 
	newtio.c_iflag = IGNPAR;  // non-parity 
	newtio.c_oflag = 0; 
	newtio.c_cflag = CS8 | CLOCAL | CREAD; // NO-rts/cts
	switch(baud) 
	{ 
		case 115200 : newtio.c_cflag |= B115200; break; 
		case 57600  : newtio.c_cflag |= B57600;  break; 
		case 38400  : newtio.c_cflag |= B38400;  break; 
		case 19200  : newtio.c_cflag |= B19200;  break; 
		case 9600   : newtio.c_cflag |= B9600;   break; 
		case 4800   : newtio.c_cflag |= B4800;   break; 
		case 2400   : newtio.c_cflag |= B2400;   break; 
		default     : newtio.c_cflag |= B115200; break; 
	} 
	//set input mode (non-canonical, no echo,.....) 
    newtio.c_lflag = 0; 
    newtio.c_cc[VTIME] = vtime;  // timeout 0.1sec matric
    newtio.c_cc[VMIN]  = vmin;   // Hold until getting at least n characters
	tcflush  (fd, TCIFLUSH); 
	tcsetattr(fd, TCSANOW, &newtio); 
		 
	return fd; 
} 
/*
------------------------------------------------------------------------------ 
Close serial port
------------------------------------------------------------------------------
*/
void close_serial(int fd) 
{ 
	printf("Close Serial port\n");
    close(fd); 
} 


/*
------------------------------------------------------------------------------ 
Read data from Arduino through serial port
------------------------------------------------------------------------------ 
*/
int     main( int argc, char **argv ) 
{ 
	int fd;              // File descryptor for serial port
	int baud;            // Baudrate, tranmission speed
	char  dev_name[128]; // Serial port dev name
    unsigned char  buf;  // Data buffer
	int   ret, skipnum = 0;

	if (argc != 3) 
	{ 
		printf(" pcserial [device] [baudrate] (device: /dev/ttySAC0, baud rate: 2400...115200)\n"); 
		return -1;  
	} 
	printf(" Start to get data through serial port...  (%s)\n", __DATE__);

	//Get parameter 
	strcpy(dev_name, argv[1]);              // device file name
	baud    = strtoul(argv[2], NULL, 10);   // serial baud rate
	//Open serial port
	//set up 1sec delay or wake up if getting data
	fd = open_serial(dev_name, baud, 0, 1); 
	if (fd < 0) return -2; 
	while(1)
    {
                ret = read(fd, &buf, sizeof(buf));
                if(ret > 0)
                {
                    //At first execution pcserial program, 
                    //2 bytes '0' will be excluded to detect the movement.
                    if(skipnum > 10) {
                        // Arduino sends '0' value when the movement is detected.
                        // Otherwise, sent '1'.
                        if(buf == '0') 
                            press_button();
                    }
                    skipnum++;
                }
    }
   
	//Close serial port
	close_serial(fd); 

	printf(" Serial test end\n"); 

	return  0;         
}

