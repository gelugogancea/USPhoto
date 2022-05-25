#include <time.h>
#include "usprotocol.h"

void my_error(int ErrNumber,char* Description,char* mItem,int SES_ID,int USER_ID,int FILE_ID)
{
	FILE *f;
	char *envhome=NULL,*mypath=NULL,*merrmsg,*s_time;
	char *a,b;
	int y,nbytes;
	time_t now;
	struct tm *l_time;

	a=&b;
	envhome=(char*)malloc(256);
	mypath=(char*)malloc(256);
	merrmsg=(char*)malloc(2048);
	s_time=(char*)malloc(32);

	now=time((time_t*)NULL);
	l_time=localtime(&now);
	strftime(s_time,25,"%F %T",l_time);
	envhome=getenv((const char*)"USPHOTO_DOWNLOAD");


	if (envhome==NULL)
	{
		perror("USPHOTO - GNET SOFTWARE COMPANY\ne-mail:develop@gonetsoftware.com\nhttp://www.gonetsoftware.com\n\nVariabila de mediu USPHOTO_DOWNLOAD nu este setata\n");
		exit(0);
	}
	else
	{
		sprintf(mypath,"%s/usphotod.log",envhome);
	}

	f=fopen((const char*)mypath,"a+");
	
	if(ErrNumber>0)	
	{	
		sprintf(merrmsg,"%s : %s - %s : USER_ID : %d :FILE_ID : %d\n",s_time,Description,mItem,USER_ID,FILE_ID);
	}
	else
	{
		sprintf(merrmsg,"%s : %d -> %s - %s : USER_ID : %d :FILE_ID : %d\n",s_time,ErrNumber,Description,mItem,USER_ID,FILE_ID);
	}
	
	
	if (f==NULL)
	{
		perror("USPHOTO - GNET SOFTWARE COMPANY\ne-mail:develop@gonetsoftware.com\nhttp://www.gonetsoftware.com\n\nNu pot sa deschid usphotod.log");
		exit(0);
	}

	y=fseek(f,0L,SEEK_END);
	nbytes=fwrite((const void*)merrmsg,sizeof(char),strlen(merrmsg),f);
	if (nbytes<0)
	{
		perror("USPHOTO - GENT SOFTWARE COMPANY\ne-mail:develop@gonetsoftware.com\nhttp://www.gonetsoftware.com\n\nNu pot sa scriu in usphotod.log");
	}
	fclose(f);

	envhome=NULL;
	free(mypath);
	free(merrmsg);
	free(s_time);
}
