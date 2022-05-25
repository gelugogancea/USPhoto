
#ifndef _USPHOTOD_
#define _USPHOTOD_


#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <syslog.h>
#include <math.h>
#include "include/mysql/mysql.h"

#define MYPORT 8080    

#define BACKLOG 1000     
#define STDIN 0
#define STDOUT 1

#define MAIN_ARGV	"no_proc"


#define MAX_BUFF_SIZE	32768
#define MAX_TRY 	10

#define TRUE			1
#define FALSE			0

#define CLI_SET			0xFC
#define CLI_CMD			0xFD
#define SRV_CMD			0xFE
#define ERR_SYS			0xFF

#define CLI_CMD_CONTINUE	0x01
#define CLI_CMD_NEXT_PACK	0x02
#define CLI_CMD_STOP		0x03
#define SRV_CMD_NOT_FINISH	0x04
#define SRV_CMD_IS_FINISH	0x05
#define RECV_FILE_OK		0x06

#define RECV_DATA		0x01		
#define RECV_SET_BUFF_SIZE	0xFFF2
#define RECV_US_PING		0xFFF3
#define RECV_FILE_NAME_RESUME	0xFFF4
#define SEND_DB_FILE_NAME	0xFFF5		
#define SEND_ALBUM_LIST		0xFFF6		
#define RECV_CLOSE_CONN		0xFFF7
#define RECV_END_SESSION	0xFFF8
#define RECV_EOF		0xFFF9
#define RECV_USER		0xFFFA
#define RECV_PASS		0xFFFB
#define RECV_ALBUM_NAME		0xFFFC
#define RECV_CREATE_ALBUM	0xFFFD
#define RECV_FILE_NAME		0xFFFE
#define RECV_SIZE_OF_FILE	0xFFFF

#define MYSQL_ERR_AUTH			0x01
#define MYSQL_ERR_SYS			0x02
#define MYSQL_ERR_ALBUM_NOT_EXIST	0x03
#define MYSQL_ERR_ALBUM_ALREADY_EXIST	0x04
#define MYSQL_ERR_ALBUM_NOT_SELECTED	0x05
#define USER_ERR_NOT_FIND_FOR_RESUME	0x06


/*
#define US_WELCOME  "Welcome to US PHOTO"
#define RECV_AUTH		0x01
#define AUTH_OK			0x02
#define AUTH_FAIL		0x04
#define RECV_FILENAME		0x10	/16	
#define RECV_FILE_SIZE		0x20	/32
#define RECV_HOW_MANY_PACK	0x30	/64
#define RECV_IDX		0x80	/128
#define RECV_CKSUM_OK		0x100	/256
#define RECV_CKSUM_FAIL		0x200	/512
#define RESEND_IDX_PACK		0x400	/1024
#define RECV_ALL_OK 		0x800	/2048
#define NOT_CONTINUE 		0x1000	/4096
#define FINISH_SESION_OK 	0x2000	/8192

#define SEND_ME_FILE 0x01
*/

typedef enum my_send_option{
	MY_CMD=1,
	MY_ASCII_DATA
}SEND_OPTION;


typedef struct my_exif_info{
	int xsize;
	int ysize;
	char datetime[20];
	char make[32];
	char model[32];
	char flash[64];
	double focal_length;
	double exposure;
	char aperture[32];
	int iso_equiv;
	char white_balance[64]; //LightSource
	char metering[32];
	char exposure_program[32];
}MY_EXIF;


typedef struct usphoto_session_info{
	MY_EXIF *usexif;
	int IDAlbum;
	int IDUser;
	long IDFile;
	char fTitle[255];
	char fComments[255];
	char fFileName[255];
}USPHOTO_SESSION;

typedef struct mstrRECV{
	short int isResume;
	short int isData;
	int TYPE_CMD;
	int USER_CMD;
	int mSizeOfFile;
	int BuffSize;
	char mUSER[32];
	char mPASS[32];
	char ALBUM_NAME[32];
	char mFILE_NAME[255];
	char dbFILE_NAME[255];
	char mSYS_PATH[255];
	int PACKIDX;
	int CKSUM;
	char mfData[MAX_BUFF_SIZE];
}STR_RECV;

typedef struct mARGV
{
	int s;
	char mPath[255];
	struct sockaddr_in their_addr;
}MyARGV;

typedef struct thread_table
{
	long tID;
	int  tIDX;
}T_THREAD;

typedef struct{
	unsigned int s1;
	unsigned int s2;
	int seeded;
}LCG_GLOBAL;



int mFile(char*,char*,long);
int send_cmd(int,int,char*,SEND_OPTION);	
long recv_data(int,int,void*,int,struct sockaddr_in,short int);
void PutDataInStruct(char*,int,STR_RECV*);
int ByteToInteger(unsigned char,unsigned char);
void myAlignInt(int,int,unsigned char*);
void IntegerToByte(int,char*);
void MyProcess(MyARGV*);
long init_daemon(int,int);
void StartMyServer(short int);
void my_error(int ErrNumber,char*,char*,int,int,int);
int usLogon(MYSQL*,char*,char*);
char* usListAlbum(MYSQL*,USPHOTO_SESSION*);
long usInsertDataPictures(MYSQL*,USPHOTO_SESSION*);
int usUpdateDataPictures(MYSQL*,USPHOTO_SESSION*);
int usCreateAlbum(MYSQL*,int,char*);
int usSelectAlbum(MYSQL*,USPHOTO_SESSION*,char*);
short int usGiveMeIDForResume(MYSQL* myData,USPHOTO_SESSION* mses);
double combined_lcg(void);
void myuniqid(char* myuqid);
#endif

