/*
USPHOTO UPLOAD SERVER
GNET SOFTWARE COPYRIGHT 2004
*/

#include "usprotocol.h"
  
extern void leftcopy(char*,char*,int,int);	
extern void rightcopy(char*,char*,int,int);	
extern void midcopy(char*,char*,int,int,int);
extern void my_error(int,char*,char*,int,int,int);
extern int usLogon(MYSQL*,char*,char*);
extern char* usListAlbum(MYSQL*,USPHOTO_SESSION*);
extern int usCreateAlbum(MYSQL*,int,char*);
extern long usInsertDataPictures(MYSQL*,USPHOTO_SESSION*);
extern int usUpdateDataPictures(MYSQL*,USPHOTO_SESSION*);
extern int usSelectAlbum(MYSQL*,USPHOTO_SESSION*,char*);
extern void myuniqid(char* myuqid);
extern short int usGiveMeIDForResume(MYSQL*,USPHOTO_SESSION*);
extern int ReadExifInfo(char*,MY_EXIF*);
USPHOTO_SESSION* init_struct(void);
void init_myexif(MY_EXIF*);
void release_struct(USPHOTO_SESSION*);
int myGiveMeFileExt(char*,char*); 

int BUFF_SIZE=1024;	//implicit trebuie sa fie 1024 cu tot cu cksum si cmd =>1020+4

int main(int argc, char **argv)
{
    int x;
    
    if (argc>1)
    {
	if (argc==2)
	{
		if((x=strncmp((const char*)argv[1],(const char*)MAIN_ARGV,strlen(MAIN_ARGV)))==0)
		{
			StartMyServer(TRUE);
		}
		else
		{
			my_error(0,"MAIN:ARGV","Nu stiu argument\n",0,0,0);
			goto myEND;
		}

    	}
	else if(argc>2)
	{
		my_error(0,"MAIN:ARGV","Prea multe argumente\n",0,0,0);
		goto myEND;
	}
    }
    else
    {
	long rinit;
	rinit=init_daemon(0,0);     
	StartMyServer(FALSE);		
    }
    
myEND:    
    return(0);
    }
int mFile(char* mBuff,char* myPath,long mfSize){
FILE *fd;
    int iw,zOff;
    int merrno=0,mRet=0;
    char *mstrerror=NULL;
            
	mstrerror=(char*)malloc(512);

	fd=fopen(myPath,"a");
	zOff=fseek(fd,0L,SEEK_END);
	iw=fwrite(mBuff,sizeof(char),mfSize,fd);
    	if (iw==0)
	{	merrno=errno;
		mstrerror=strerror(merrno);
		my_error(0,"MFILE",mstrerror,0,0,0);
		mRet=0;
	}
	else
	{
	    mRet=iw;
	}
	free(mstrerror);
	fclose(fd);
	return mRet;
}
long recv_data(int s,int rBytes,void* retBuf,int timeout,struct sockaddr_in cliaddr,short int MyWait)
{

	int recvsize=0,unlock_try=0;
	long realrecvsize=0,movebuff=0;
	char *myptr;
	struct timeval tv;
	fd_set fd_read;
	int val=1,oflag=0,mytry=0;
	int length=0,mi=0;
	int aa=0;
	int myFailCount=0;
	
	if((fcntl(s,F_SETFL,oflag | O_NDELAY))<0)
	{
		my_error(0,"MyPROCESSES:RECV","FCNTL O_NDELAY FAIL\n",0,0,0);
	}
	
	if((ioctl(s,FIONBIO,&val))<0)	 	//1 este lock ...o este unlock
	{					
		my_error(0,"MyPROCESSES:RECV","IOCTL FIONBIO LOCK FAIL\n",0,0,0);
	}		
	
				
			myptr=retBuf;
				
			FD_CLR(s,&fd_read);
			FD_ZERO(&fd_read);
			FD_SET(s,&fd_read);
			tv.tv_sec=timeout;
			tv.tv_usec=10000;
			unlock_try=select(s+1,&fd_read,NULL,NULL,&tv);
				
			if(unlock_try<=0)
			{
				
			    usleep(100);	
			    goto myEND;
				
			}
				
			
			for(;;)
			{
				
				
				
myAGAIN:			if(rBytes!=0xFFFFFFFF)
				{
					do 
					{	
					
						usleep(10);
						mi=ioctl(s,FIONREAD,&length,sizeof(length));	
					
						
						recvsize=read(s,myptr+=(movebuff),length);
						if(recvsize>0)
						{
							aa+=recvsize;
							movebuff=recvsize;
							myFailCount=0;
						}
						else
						{
							myFailCount++;
							if(myFailCount>100)
							{
								goto myEND;
								realrecvsize=-1;
							}
							FD_CLR(s,&fd_read);
							FD_ZERO(&fd_read);
							FD_SET(s,&fd_read);
							tv.tv_sec=10;
							tv.tv_usec=0;
							select(s+1,&fd_read,NULL,NULL,&tv);
						}
						
						if(aa>rBytes)
						{
							realrecvsize=-1;
							goto myEND;
						}
												
						printf("IOCTL : %d     %d    %d\n",aa,length,rBytes);
						length=0;
						recvsize=0;
					}while(aa!=rBytes);
				}
				else
				{
					usleep(10);
					mi=ioctl(s,FIONREAD,&length,sizeof(length));	
					recvsize=read(s,myptr+=(movebuff),length);
				}
					
				if(length==0)
				{
				
					usleep(50);
					
				}
				else if(mi<0)	
				{
					break;
				}	
								

				printf("TRY :%d   %d   %d    %d\n",recvsize,length,realrecvsize,rBytes);							
				
				if(rBytes!=0xFFFFFFFF)
				{
					
				
					if(aa>0)
					{	
				    
				    		if(aa==BUFF_SIZE)
						{
							realrecvsize=aa;
							break;
						}
						else if(recvsize==rBytes+4)
						{

							realrecvsize=aa;
							break;
						}
						else
						{
							realrecvsize+=aa;
						}
					
						
						if(aa==rBytes)
						{
							MyWait=0;
							break;
						}
						else
						{
							MyWait=1;
							continue;
						}
						
					
				    
					}
					else if(recvsize==0 && length==0)
					{
						
						break;
					}
					else if(recvsize<=0 && realrecvsize<rBytes)
					{
						mytry++;
						
						if(mytry>1000)
						{
							realrecvsize=0;
							break;
							
						}
						else
						{
							continue;
						}
					}			    
			    		else if(recvsize<=0 && realrecvsize==rBytes)
					{
					    break;
					}
					else if(recvsize==EAGAIN)
					{
					    perror("RECV:EAGAIN ERR : ");
					    usleep(50);
					    goto myAGAIN;
					}
			    
				}
				else
				{
					if(recvsize>0)	
					{	
						realrecvsize+=recvsize;
					}
					else if(recvsize==0)
			    		{
			   	    		break;
				
			    		}
			    		else if(recvsize<0)
					{
					
					
						if(recvsize==EINTR)
						{
							my_error(0,"MyPROCESSES:RECV","EINTR err\n",0,0,0);
							recvsize=1;
							goto myAGAIN;
						}
						else if(recvsize==EAGAIN)
						{	
							recvsize=1;
							my_error(0,"MyPROCESSES:RECV","EAGAIN err\n",0,0,0);
						}
						else if(recvsize==EIO)
						{
							recvsize=1;
							my_error(0,"MyPROCESSES:RECV","EIO err\n",0,0,0);
						}
						else
						{
							goto myEND;
						}
					}
					
				}
			
				
			}
myEND:			
			
			printf("REALRECV : %d\n",realrecvsize);
			val=0;
			if((ioctl(s,FIONBIO,&val))<0)
			{
				my_error(0,"MyPROCESSES:RECV","IOCTL FIONBIO UNLOCK FAIL\n",0,0,0);
			}	
			return(realrecvsize);
					

	
}
int send_cmd(int s,int timeout,char* mcmd,SEND_OPTION myOption)
{
	fd_set fd_write;
	struct timeval tv;
	
	int res,sent,val=1,myret=0;
	
	
	FD_ZERO(&fd_write);
	FD_CLR(s,&fd_write);
	FD_SET(s,&fd_write);
	tv.tv_sec=timeout;
	tv.tv_usec=10000;
	
	if((ioctl(s,FIONBIO,&val))<0)	 	//1 este lock ...o este unlock
	{					
		my_error(0,"MyPROCESSES:SEND","IOCTL FIONBIO LOCK FAIL\n",0,0,0);
	}	
	res=select(s+1,NULL,&fd_write,NULL,&tv);
	if(!res)
	{
	    
	    myret=-1;
	    
	}
	else if(res==0)
	{
	    
	    myret=0;
	    
	}
	else
	{
		if(FD_ISSET(s,&fd_write))
		{
			
			if(myOption==MY_CMD)
			{	
				sent=send(s,(void*)mcmd,4,0);
			}
			else
			{
				sent=send(s,(void*)mcmd,strlen(mcmd),0);
				
			}
			myret=sent;
			
		}
		else
		{
		    
		    
		    myret=0;
		    
		}
		
		myret=0;
		
	}
	val=0;
	if((ioctl(s,FIONBIO,&val))<0)
	{	 	
		my_error(0,"MyPROCESSES:SEND","IOCTL FIONBIO END LOCK FAIL\n",0,0,0);
	}
	return(myret);
}

void PutDataInStruct(char* mBuff,int BUFF_ALLOC,STR_RECV* mRet){

	
	char *mData=NULL;
	char* myIDX=NULL;
	char* myCKSUM=NULL;
	char* b1=NULL;
	char* b2=NULL;
	char* b3=NULL;
	int a=0,b=0;
	
	b1=(char*)malloc(BUFF_ALLOC+1);
	b2=(char*)malloc(BUFF_ALLOC+1);
	b3=(char*)malloc(BUFF_ALLOC+1);
	mData=(char*)malloc(BUFF_ALLOC+1);
	myIDX=(char*)malloc(4);		
	myCKSUM=(char*)malloc(4);
	
	memcpy(b1,mBuff,BUFF_ALLOC);
	memcpy(b2,mBuff,BUFF_ALLOC);
	memcpy(b3,mBuff,BUFF_ALLOC);
		
	leftcopy(b1,myIDX,2,BUFF_ALLOC);
	a=ByteToInteger(myIDX[0],myIDX[1]);
	
	rightcopy(b2,myCKSUM,2,BUFF_ALLOC);
	b=ByteToInteger(myCKSUM[0],myCKSUM[1]);
	
	
	if(a==RECV_DATA)
	{
		mRet->isData=TRUE;	
	}
	else if(a==RECV_USER)
	{
		
		memset(mRet->mUSER,0x0,32);
		midcopy(b3,mData,3,BUFF_ALLOC-2,BUFF_ALLOC);// buffer - 2 octeti rezervati pentru CKSUM
		memcpy(mRet->mUSER,mData,(BUFF_ALLOC)-4);
		mRet->CKSUM=b;
		
	}
	else if(a==RECV_PASS)
	{
		memset(mRet->mPASS,0x0,32);
		midcopy(b3,mData,3,BUFF_ALLOC-2,BUFF_ALLOC);// buffer - 2 octeti rezervati pentru CKSUM
		memcpy(mRet->mPASS,mData,(BUFF_ALLOC)-4);
		mRet->CKSUM=b;
		
	}
	else if (a==RECV_ALBUM_NAME || a==RECV_CREATE_ALBUM)
	{
		memset(mRet->ALBUM_NAME,0x0,32);
		midcopy(b3,mData,3,BUFF_ALLOC-2,BUFF_ALLOC);// buffer - 2 octeti rezervati pentru CKSUM
		memcpy(mRet->ALBUM_NAME,mData,(BUFF_ALLOC)-4);
		mRet->CKSUM=b;
		
	}
	else if(a==RECV_FILE_NAME || a==RECV_FILE_NAME_RESUME)
	{
		memset(mRet->mFILE_NAME,0x0,255);
		midcopy(b3,mData,3,BUFF_ALLOC-2,BUFF_ALLOC);// buffer - 2 octeti rezervati pentru CKSUM
		memcpy(mRet->mFILE_NAME,mData,(BUFF_ALLOC)-4);
		mRet->CKSUM=b;
		
	}
	else if(a==RECV_SIZE_OF_FILE)
	{
		midcopy(b3,mData,3,BUFF_ALLOC-2,BUFF_ALLOC);// buffer - 2 octeti rezervati pentru CKSUM
		mRet->mSizeOfFile=atoi(mData);
		mRet->CKSUM=b;
		
	}
	else if(a==RECV_SET_BUFF_SIZE)
	{
		midcopy(b3,mData,3,BUFF_ALLOC-2,BUFF_ALLOC);// buffer - 2 octeti rezervati pentru CKSUM
		mRet->BuffSize=atoi(mData);
		mRet->CKSUM=b;
	}
	
	
	if(mRet->isData==TRUE)
	{
		
		memset(mRet->mfData,0x0,BUFF_ALLOC);
		mRet->PACKIDX=a;
		midcopy(b3,mData,3,BUFF_ALLOC-2,BUFF_ALLOC);// buffer - 2 octeti rezervati pentru CKSUM
		memcpy(mRet->mfData,mData,(BUFF_ALLOC)-4);
		mRet->CKSUM=b;
		
	}
	else
	{
		mRet->TYPE_CMD=a;
		mRet->USER_CMD=b;
		mRet->isData=FALSE;
	}
		
	free(b3);
	free(b2);
	free(b1);
	free(mData);
	
	free(myCKSUM);
	free(myIDX);
	
}
int ByteToInteger(unsigned char h,unsigned char l){
	
	int Hi=0,Lo=0;
	int hh=0,ll=0;
	hh=h;
	ll=l;
	
	if(hh<0)
	{
		Hi=hh+256;
	}
	else
	{
		Hi=h;
	}
	if(ll<0)
	{
		Lo=ll+256;
	}
	else
	{
		Lo=l;
	}
	return Hi*256+Lo;
}
void myAlignInt(int a,int b,unsigned char* s){
	
	int i;
	unsigned char* st,s1[1],s2[1];
	st=s;
	if(a<0x100)
	{
		st[0]=0x0;
		st[1]=a;

	}
	else
	{
		IntegerToByte(a,s1);
		memcpy(st,s1,2);

	}
	if(b<0x100)
	{
		st[2]=0x0;
		st[3]=b;

	}
	else
	{
		*st++;
		*st++;
		IntegerToByte(b,s2);
		memcpy(st,s2,2);
		*st--;
		*st--;

	}
	for(i=0;i<4;i++)
	{
		*s++=st[i];
	}
	*st=0;
	
}
void IntegerToByte(int mNumber,char* sp){
	sp[0]=mNumber/0x100;
	sp[1]=mNumber%0x100;
	
}
void MyProcess(MyARGV* MArg){	
	int maxtry=0,mTime=10;
	int lenrecv=0,lenwrite=0,mCount=1;
	int MyWait=0;
	int myrescreate=0;
	char* mBuff=NULL;
	char* realPath=NULL;
	unsigned char *mcmd;
	fd_set fd_read;
	struct timeval tv;
	int msize=BUFF_SIZE,t;
	int BytesRemain=0;
	int BytesToRead=BUFF_SIZE;
	int TotalData=0;
	int MySpecCmd=0;
	int nval=0;
	int mypid=0;
	int sflag=0;
	int retext=0;
	short int myauth=0;
	int resselect=0,muid=0;
	short int retid=0;
	SEND_OPTION mOption=MY_CMD;
	char* mList;
	char ExtFile[5];
	char mPath[512];
	STR_RECV *ss;
	USPHOTO_SESSION *mySession=NULL;
	MYSQL *myDATA=NULL;
	
	muid=getuid();
	if(muid==0)
	{
		my_error(0,"MyPROCESSES:START_CHILD_PROCESS:SECURITY PROBLEM","Cineva incearca sa pornesc acest process avand drepturi de root\n",0,0,0);
	}	
		
	myDATA=mysql_init((MYSQL*)0);

	if(!mysql_real_connect(myDATA,"10.40.3.102","root","","usphoto",(int)NULL,NULL,0))
    	{
    		
		my_error(0,"MyPROCESSES:MYSQL_CONNECT","Nu pot sa ma conectez la MYSQL\n",0,0,0);
		exit(0);
	}
	
	t=setsockopt(MArg->s,SOL_SOCKET,SO_SNDBUF && SO_RCVBUF,&msize,sizeof(msize));
	mBuff=(char*)malloc(MAX_BUFF_SIZE);
	mcmd=(unsigned char*)malloc(sizeof(unsigned char*));
		
	mySession=init_struct();
	ss=(STR_RECV*)malloc(sizeof(STR_RECV)+1);		
	
		    strcpy(ss->mSYS_PATH,MArg->mPath);
		    mypid=getpid();
		    
		    nval=fcntl(MArg->s,F_SETOWN,mypid);
		    if(nval<0)
		    {
			if(nval==EINVAL)
			{
				my_error(0,"MyPROCESSES:MY_PROCESS","IOCTL EINVAL\n",0,0,0);
			}
			else if(nval==ENOTTY)
			{
				my_error(0,"MyPROCESSES:MY_PROCESS","IOCTL ENOTTY\n",0,0,0);
			}
			else
			{	
				my_error(0,"MyPROCESSES:MY_PROCESS","IOCTL SETOWN FAIL\n",0,0,0);
		    	}
		    }
		    	if((ioctl(MArg->s,FIOASYNC,&sflag))<0)
			{		
				my_error(0,"MyPROCESSES:MYPROCESS","IOCTL FIOASYNC FAIL\n",0,0,0);
			}
		    
		    mTime=180;//astept 60 de secunde pt. conectarea initiala	
		    for(;;)
		    {
			maxtry++;
			if(maxtry==MAX_TRY)
			{
			    exit(0);
			}
			
			    	lenrecv=recv_data(MArg->s,BytesToRead,mBuff,mTime,MArg->their_addr,MyWait);
				
				if(lenrecv>0)
				{
	
					PutDataInStruct(mBuff,lenrecv,ss);
					if(ss->isData==TRUE)
			   		{
						
						mCount++;
						
						lenwrite=mFile(ss->mfData,realPath,lenrecv-4);//fara cei 4 octeti
						if(lenwrite>0)                       //rezervati IDX & CKSUM 
						{
				    			TotalData+=lenwrite;			
							
							
							BytesRemain-=lenwrite;
							printf("TD / BR : %d    %d\n",TotalData,BytesRemain);

							if(BytesRemain>=BUFF_SIZE)
							{
								BytesToRead=BUFF_SIZE;
								
							}
							else
							{
								if(BytesRemain>0)
								{	
									BytesToRead=BytesRemain+4;
								}
								else
								{
									BytesToRead=0;
								}
								
							}
							
							if(BytesRemain>0)
							{ 
								ss->TYPE_CMD=CLI_CMD_NEXT_PACK;
								ss->USER_CMD=mCount;
							}
							else
							{
									
								memset(mPath,0x0,512);
								
								sprintf(mPath,"%s/%s",ss->mSYS_PATH,mySession->fFileName);
								ReadExifInfo(realPath,mySession->usexif);
								usUpdateDataPictures(myDATA,mySession);
								
								ss->TYPE_CMD=SRV_CMD;
								ss->USER_CMD=RECV_FILE_OK;
								ss->isData=FALSE;
								ss->mSizeOfFile=0;
								ss->PACKIDX=0;
								ss->isResume=FALSE;
								memset(ss->mFILE_NAME,0x0,255);
								memset(ss->dbFILE_NAME,0x0,255);
								memset(mySession->fTitle,0x0,255);
								memset(mySession->fComments,0x0,255);
								memset(mySession->fFileName,0x0,255);
								memset(mPath,0x0,512);

								mySession->IDFile=0;

								init_myexif(mySession->usexif);
								BytesToRead=0xFFFFFFFF;//reinitializez BytesToRead
								mCount=1;
									
									
							
							}
							myAlignInt(ss->TYPE_CMD,ss->USER_CMD,mcmd);
							send_cmd(MArg->s,mTime,mcmd,MY_CMD);	
							ss->TYPE_CMD=0;
							ss->USER_CMD=0;
						}
				
	
    		    	    		}
			    		else
			    		{
					
						
						if(ss->TYPE_CMD==RECV_USER)
						{
							MySpecCmd=SRV_CMD;
							mOption=MY_CMD;
							
							
						}
						else if(ss->TYPE_CMD==RECV_PASS)
						{
							
							mOption=MY_CMD;
							mySession->IDUser=usLogon(myDATA,ss->mUSER,ss->mPASS);
    							if(mySession->IDUser>0)
							{

								MySpecCmd=SRV_CMD;
							}
							else if(mySession->IDUser==0)
							{

								myauth=TRUE;
								MySpecCmd=ERR_SYS;
								ss->TYPE_CMD=MYSQL_ERR_AUTH;
								
							}
							else if(mySession->IDUser<0)
							{
								myauth=TRUE;
								MySpecCmd=ERR_SYS;
								ss->TYPE_CMD=MYSQL_ERR_SYS;
							}
												
    							
						}
						else if(ss->TYPE_CMD==RECV_ALBUM_NAME)
						{

														
							mOption=MY_CMD;
							
							if((usSelectAlbum(myDATA,mySession,ss->ALBUM_NAME))>0)
							{
								MySpecCmd=SRV_CMD;	
							}
							else
							{
								MySpecCmd=ERR_SYS;
								ss->TYPE_CMD=MYSQL_ERR_ALBUM_NOT_EXIST;
								
							}
							
							
						}
			    			else if(ss->TYPE_CMD==RECV_CREATE_ALBUM)
						{
							mOption=MY_CMD;
							
							myrescreate=usCreateAlbum(myDATA,mySession->IDUser,ss->ALBUM_NAME);
							if(myrescreate>0)
							{
								MySpecCmd=SRV_CMD;
							}	
							else if(myrescreate==0)
							{
								MySpecCmd=ERR_SYS;
								ss->TYPE_CMD=MYSQL_ERR_ALBUM_ALREADY_EXIST;
							}
							else
							{
								MySpecCmd=ERR_SYS;
								ss->TYPE_CMD=MYSQL_ERR_SYS;
							}
							
						}
						else if(ss->TYPE_CMD==RECV_FILE_NAME)
						{
							
							myuniqid(mySession->fFileName);
							retext=myGiveMeFileExt(ExtFile,ss->mFILE_NAME);
							strncat(mySession->fFileName,ExtFile,retext);
							strncpy(mySession->fTitle,ss->mFILE_NAME,strlen(ss->mFILE_NAME));
							
							MySpecCmd=SRV_CMD;
							mOption=MY_CMD;	
						
						}
						else if(ss->TYPE_CMD==RECV_SIZE_OF_FILE)
						{
							realPath=(char*)malloc(512);
		    					
							if(mySession->IDAlbum>0)
							{
								
								BytesRemain=ss->mSizeOfFile;
								if(BytesRemain<BUFF_SIZE)
								{
									BytesToRead=BytesRemain;
								}
								else
								{
									BytesToRead=BUFF_SIZE;	
								}
								
								MySpecCmd=SRV_CMD;
								mOption=MY_CMD;
								

								sprintf(realPath,"%s/%s",ss->mSYS_PATH,mySession->fFileName);
								
								if(ss->isResume==FALSE)
								{
									if((usInsertDataPictures(myDATA,mySession))<=0)
									{

										MySpecCmd=ERR_SYS;
										goto myEND;
									}
								}
							}
							else
							{
								MySpecCmd=ERR_SYS;
								mOption=MY_CMD;
								ss->TYPE_CMD=MYSQL_ERR_ALBUM_NOT_SELECTED;
								
							}
						}
						else if(ss->TYPE_CMD==SEND_ALBUM_LIST)
						{

							MySpecCmd=SRV_CMD;
							mOption=MY_ASCII_DATA;
							
							mList=usListAlbum(myDATA,mySession);
							
							mcmd=mList;
							
						}
						else if(ss->TYPE_CMD==SEND_DB_FILE_NAME)
						{
							MySpecCmd=SRV_CMD;
							mOption=MY_ASCII_DATA;
							strncpy(mcmd,mySession->fFileName,strlen(mySession->fFileName));
							mcmd[strlen(mySession->fTitle)]=0xA;

						}
						else if(ss->TYPE_CMD==RECV_FILE_NAME_RESUME)
						{
							
							mOption=MY_CMD;
							ss->isResume=TRUE;
							strncpy(mySession->fFileName,ss->mFILE_NAME,strlen(ss->mFILE_NAME));
							retid=usGiveMeIDForResume(myDATA,mySession);
							
							if(retid>0)
							{
								MySpecCmd=SRV_CMD;
									
							}
							else if(retid<0)
							{
								MySpecCmd=ERR_SYS;
							}
							else 
							{
								MySpecCmd=USER_ERR_NOT_FIND_FOR_RESUME;
							}
							
						}
						else if (ss->TYPE_CMD==RECV_US_PING)
						{
							MySpecCmd=SRV_CMD;
							BytesToRead=0xFFFFFFFF;	
						}
						else if(ss->TYPE_CMD==RECV_SET_BUFF_SIZE)
						{
							MySpecCmd=SRV_CMD;
							BUFF_SIZE=ss->BuffSize;
							
						}
						
						if(mOption==MY_CMD)
						{
							myAlignInt(MySpecCmd,ss->TYPE_CMD,mcmd);
						}
						send_cmd(MArg->s,mTime,mcmd,mOption);
						
						if(myauth==TRUE)
						{
							goto myEND;
						}
					}
															
					if(mOption==MY_CMD)
					{
						memset(mcmd,0x0,4);
					}
					else
					{
						memset(mcmd,0x0,strlen(mcmd));
					}
					*mBuff=0;
					*ss->mfData=0;
					MySpecCmd=0;
					memset(mBuff,0x0,BUFF_SIZE);

					ss->TYPE_CMD=0;	
					ss->USER_CMD=0;
					memset(ss->mfData,0x0,lenrecv);
					mTime=10;
					lenwrite=0;
					lenrecv=0;
				    	maxtry=0;
					
				}
			   	else
			   	{

					FD_CLR(MArg->s,&fd_read);
					FD_ZERO(&fd_read);
					FD_SET(MArg->s,&fd_read);
					
					tv.tv_sec=60;
					tv.tv_usec=0;
					resselect=select(MArg->s+1,&fd_read,NULL,NULL,&tv);
				}
			   	
			   	
			
		    }    
		
            
	free(realPath);
myEND:	    
	        
	mysql_close(myDATA);
	release_struct(mySession);
	free(mcmd);
	free(mBuff);
	    
	close(MArg->s);  
	free(MArg);
	exit(0);
	    
}
long init_daemon(nochdir,noclose)
{
	
switch (fork())
    {
        case 0:  break;
        case -1: return -1;
        default: _exit(0);          
    }

    if (setsid() < 0)              
      return -1;

   
    signal(SIGHUP,SIG_IGN);
    switch (fork())
    {
        case 0:  break;
        case -1: return -1;
        default: _exit(0);
    }

    if (!nochdir)
      chdir("/");
      umask(0);

    if (!noclose)
    {
        
        int i;
	for(i=1;i<64;i++)
	{
		close(i);
	}
	open("/dev/null",O_RDWR);
        dup(0); dup(0);
    }

    return 0;
}
void StartMyServer(short int mbool)
{
      
	struct sockaddr_in my_addr;    
        struct sockaddr_in their_addr; 
        int child,i=0;
	int sockfd;
	int sin_size,yes=1,new_fd=0;
	char* mPath=NULL;
	MyARGV* marg;
	
	i=getuid();
	if(i==0)
	{
		perror("SECURITY PROBLEM\nCineva incearca sa ruleze acest process avand drepturi de root \nInchid aplicatia\n");

	}	
	mPath=(char*)malloc(sizeof(char*));
			    
	mPath=getenv((const char*)"USPHOTO_DOWNLOAD");	
	if(!mPath)
	{
		perror("Nu este setat USPHOTO_DOWNLOAD\n");
		exit(0);
	}	
	
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(1);
        }
	
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1)
	{
	    perror("SET_SOCK_OPT : \n");
	}
	
        my_addr.sin_family = AF_INET;         
        my_addr.sin_port = htons(MYPORT);     
        my_addr.sin_addr.s_addr = INADDR_ANY; 
        bzero(&(my_addr.sin_zero), 8);        

        if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))== -1) {
            perror("bind");
            exit(1);
        }

        if (listen(sockfd, 1) == -1) {
            perror("listen");
            exit(1);
        }
	
	
        while(1) {  
            sin_size = sizeof(struct sockaddr_in);
            if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,&sin_size)) == -1) {
                perror("accept");
                continue;
            }
            
            
	    marg=(MyARGV*)malloc(sizeof(MyARGV));
	    marg->s=new_fd;
	    strncpy(marg->mPath,mPath,strlen(mPath));
	    marg->their_addr=their_addr;
	    
	    if(mbool==FALSE)
	    {	    
	    	signal(SIGCHLD,SIG_IGN);	
		if((child=fork())==0)
	    	{
	    		close(sockfd);
			MyProcess(marg);
			exit(0);
	    
	    	}
	    	close(new_fd);
	    }
	    else
	    {
	    	MyProcess(marg);	
	    }
	    free(marg);	
            
        }
}
USPHOTO_SESSION* init_struct(void)
{
	USPHOTO_SESSION *m=NULL;

	m=(USPHOTO_SESSION*)malloc(sizeof(USPHOTO_SESSION)+1);
	m->usexif=(MY_EXIF*)malloc(sizeof(MY_EXIF));
	m->IDAlbum=0;
	m->IDUser=0;
	m->IDFile=0;
	memset(m->fTitle,0x0,255);
	memset(m->fComments,0x0,255);
	memset(m->fFileName,0x0,255);
	init_myexif(m->usexif);

	return m;
}
void init_myexif(MY_EXIF* usexif)
{
	usexif->xsize=0;
	usexif->ysize=0;
	memset(usexif->datetime,0x0,20);
	memset(usexif->make,0x0,32);
	memset(usexif->model,0x0,32);
	memset(usexif->flash,0x0,64);
	usexif->focal_length=0;
	usexif->exposure=0;
	memset(usexif->aperture,0x0,32);
	usexif->iso_equiv=0;
	memset(usexif->white_balance,0x0,64);
	memset(usexif->metering,0x0,32);
	memset(usexif->exposure_program,0x0,32);
}
void release_struct(USPHOTO_SESSION* m)
{
	free(m);
}
int myGiveMeFileExt(char* Mext,char* mBuff)
{
	
	
	char* ref;

	char* cmp=".";
	char* defext=".jpg";
	int i=0,v=0,x=0,z=0;
	
	ref=mBuff;
	
	do
	{
	    i++;
	    *mBuff++;
	    v=strncmp(cmp,mBuff,1); 	
	    if(i>255){break;}	
	}while(v!=0);
	if(i<255)
	{

		for(x=i;x<(i+4);x++)
		{
			
			*Mext=*mBuff;	
			*Mext++;
			*mBuff++;
			z++;
		}
		Mext[z+1]='\0';
	}
	else
	{
		strcpy(Mext,defext);
	}
	i=0;
	mBuff=ref;
	Mext-=z;
	return z;
	
	
}
