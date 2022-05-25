
#include "myexif.h"

int ReadExifInfo(char*,MY_EXIF*);
extern int ByteToInteger(unsigned char,unsigned char);
extern void my_error(int,char*,char*,int,int,int);
static char * exif_get_tagname(int,char*,int,tag_table_type);
static int TypeOfTag(int);
void bincopy(char*,char*,int,int,int);
static unsigned int get32s(void *value, MY_BYTE_ALIGN motorola_intel);
static unsigned int get16s(void *value, MY_BYTE_ALIGN motorola_intel);
static unsigned get32u(void*, MY_BYTE_ALIGN);
static int get16u(void*, MY_BYTE_ALIGN);
static double exif_convert_any_format(void*, int, MY_BYTE_ALIGN);

int ReadExifInfo(char* myFile,MY_EXIF* myses)
{
	FILE *fd;
	int merrno=0,mRet=0;
	char *mstrerror=NULL;
        unsigned char ss[2],lmark[2];
	int mSOI=0,APP_MARK=0;
	int LenExif=0;
	int LenOffset=0;
	int zOff=0;
	int i=0;
	long NoOfDirEntries=0;
	int FID=0;	//Image File Directory Offset
	MY_BYTE_ALIGN ByteAlign;
	long curoff=0,CurrentOffset=0;
	char ExifSignature[6];
	static char EXIF_SIGN[]={0x45,0x78,0x69,0x66,0x00,0x00};
	static char INTEL_ALIGN[]={0x49,0x49};
	static char MOTOROLA_ALIGN[]={0x4D,0x4D};
	int tagno=0;
	int DataFormat=0,LenOfTag=0;
	long SizeMultiplier=0;
	char mdata[2];
	char chartagno[2];
	char mdirdata[4];
	char mdataformat[2];
	char msizemultiplier[4];
	char nxtch[4];
	char realdata[32];
	char stringof[4];
	int StringOffset=0;
	long NextChunk=0;
	char *DirEntryInfo=NULL;
	char *TagName=NULL;
	double res;
	char *mBuff=NULL;
	char bytetest[2];
	    
	mstrerror=(char*)malloc(512);

	fd=fopen(myFile,"r");
	if(!fd)
	{
		merrno=errno;
		mstrerror=strerror(merrno);
		my_error(0,myFile,mstrerror,0,0,0);
		return -1;
	}
	
	if(fread(ss,sizeof(char),2,fd)<=0)
	{
		merrno=errno;
		mstrerror=strerror(merrno);
		my_error(0,myFile,mstrerror,0,0,0);
		mRet=0;
		goto myEND;
	}
	else
	{
		LenOffset+=2;
	}
	mSOI=ByteToInteger(ss[0],ss[1]);
	if(JPEG_SOI!=mSOI)
	{
		my_error(0,myFile," -> Fisierul nu este JPEG",0,0,0);
		mstrerror=NULL;
		return -1;

	}
	
	do
	{
		zOff=fseek(fd,LenOffset,SEEK_SET);
		curoff=ftell(fd);
		if(fread(ss,sizeof(char),2,fd)<=0)
		{
			merrno=errno;
			mstrerror=strerror(merrno);
			if(merrno>0){
				my_error(0,myFile,mstrerror,0,0,0);
			
			}
			else
			{
				my_error(0,myFile,"-> Fisierul nu are APP0",0,0,0);
			}
			mRet=0;
			mstrerror=NULL;
			return -1;
		}
		else
		{
			LenOffset+=2;
			zOff=fseek(fd,LenOffset,SEEK_SET);
			curoff=ftell(fd);
			if(fread(lmark,sizeof(char),2,fd)<=0)
			{
				merrno=errno;
				mstrerror=strerror(merrno);
				if(merrno>0)
				{
					my_error(0,myFile,mstrerror,0,0,0);
				}
				else
				{
					my_error(0,myFile,"-> Fisierul nu are APP1",0,0,0);
				}
				mRet=0;
				mstrerror=NULL;
				return -1;
			}
			
			LenExif=ByteToInteger(lmark[0],lmark[1]);
			LenOffset+=LenExif;
		}
		APP_MARK=ByteToInteger(ss[0],ss[1]);
		
	}while(APP_MARK!=APP1_MARKER);
	LenOffset-=LenExif; 
	LenOffset+=2;	    //adaug si cei 2 octeti care ne spune lungimea totala a EXIF si care sunt inclusi in data	
	
	//Caut EXIF signature
	zOff=fseek(fd,LenOffset,SEEK_SET);
	curoff=ftell(fd);
	if(fread(ExifSignature,sizeof(char),6,fd)<=0)
	{
		merrno=errno;
		mstrerror=strerror(merrno);
		my_error(0,myFile,mstrerror,0,0,0);
		mRet=0;
		goto myEND;
	}
	if(memcmp(ExifSignature,EXIF_SIGN,6)>0)
	{
		//Daca nu are Exif n-are rost sa mai continui
		goto myEND;	
	}
	else
	{
		LenOffset+=6;
	}
	//Acum deja pot sa pun in buffer, absolut tot segmentul de date despre EXIF
	mBuff=(char*)malloc(LenExif+1);
	zOff=fseek(fd,LenOffset,SEEK_SET);
	curoff=ftell(fd);
	if(fread(mBuff,sizeof(char),LenExif,fd)<=0)
	{
		merrno=errno;
		mstrerror=strerror(merrno);
		my_error(0,myFile,mstrerror,0,0,0);
		mRet=0;
		free(mBuff);
		goto myEND;
	}
	
	bincopy(mBuff,bytetest,1,2,LenExif);
	//midcopy(mBuff,bytetest,1,2,LenExif);
	if(memcmp(bytetest,INTEL_ALIGN,2)>0)
	{
		if(memcmp(bytetest,MOTOROLA_ALIGN,2)>0)
		{
			my_error(0,myFile,"EROARE HEADER TIFF,UNKNOWN BYTE ALIGN",0,0,0);
			free(mBuff);
			goto myEND;	//ooopppss....eroare header TIFF
		}
		else
		{
			ByteAlign=MOTOROLA_FORMAT;
		}
	}
	else
	{
		ByteAlign=INTEL_FORMAT;
	}

	bincopy(mBuff,mdirdata,5,4,LenExif);
	FID=get32s(mdirdata,ByteAlign);

	DirEntryInfo=(char*)malloc(64);
	TagName=(char*)malloc(64);
	do{	
		if(CurrentOffset==0)
		{
			bincopy(mBuff,mdata,FID+1,2,LenExif);
			NoOfDirEntries=get16s(mdata,ByteAlign);
			NextChunk=FID;
			
		}
		else
		{
			bincopy(mBuff,mdata,NextChunk+1,2,LenExif);
			NoOfDirEntries=get16s(mdata,ByteAlign);
			//NextChunk+=3;
		}
		
		if(NoOfDirEntries>0)
		{
			for(i=0;i<NoOfDirEntries;i++)
			{

				bincopy(mBuff,DirEntryInfo,NextChunk+3+(i*12),12,LenExif);
				bincopy(DirEntryInfo,chartagno,1,2,12);
				tagno=get16u(chartagno,ByteAlign);
				bincopy(DirEntryInfo,mdataformat,3,2,12);
				DataFormat=get16u(mdataformat,ByteAlign);
				bincopy(DirEntryInfo,msizemultiplier,5,4,12);
				SizeMultiplier=get32u(msizemultiplier,ByteAlign);
				LenOfTag=TypeOfTag(DataFormat)*SizeMultiplier; 
				TagName=exif_get_tagname(tagno,NULL,LenOfTag,tag_table_IFD);
				if(tagno==0 || LenOfTag==0 || SizeMultiplier==0)
				{
					break;
				}
				
					if(DataFormat==2 && LenOfTag>4)	//Daca este ASCII
					{
						
						bincopy(DirEntryInfo,stringof,9,4,12);
						StringOffset=get32u(stringof,ByteAlign);
						bincopy(mBuff,realdata,StringOffset+1,LenOfTag,LenExif);
						if(tagno==0x010F)	//Make 
						{
							realdata[LenOfTag]='\0';
							memcpy(myses->make,realdata,LenOfTag);
						}
						else if(tagno==0x0110)	//Model
						{
							realdata[LenOfTag]='\0';
							memcpy(myses->model,realdata,LenOfTag);
						}
						else if(tagno==0x0132)	//DateTime
						{
							realdata[LenOfTag]='\0';
							memcpy(myses->datetime,realdata,LenOfTag);
						}
						printf("TAG : %s  \n %s\n\n",TagName,realdata);	
						//realdata=NULL;
					}
					else
					{
						
						bincopy(DirEntryInfo,realdata,9,4,12);
						res=exif_convert_any_format(realdata,DataFormat,ByteAlign);	
						
						if(tagno==0xA002)	//XSize
						{
							myses->xsize=(int)res;	
						}
						else if(tagno==0xA003)	//YSize
						{
							myses->ysize=(int)res;
						}
						else if(tagno==0x829A)	//ExposureTime
						{
							myses->exposure=(double)res * pow (10,7);
						}
						else if(tagno==0x9209)	//Flash
						{
							if(res==0 || res==16 || res==24 || res==32)
							{
								strcpy(myses->flash,"No Flash");
							}
							else
							{	
								strcpy(myses->flash,"Unknown");
								switch ((int)res){
									case 1: strcpy(myses->flash,"Flash");
									case 5: strcpy(myses->flash,"Flash strobe return light detected");
									case 7: strcpy(myses->flash,"Flash strob return light detected");
									case 9: strcpy(myses->flash,"Compulsory Flash");
									case 13: strcpy( myses->flash,"Compulsory Flash Return light not detected");
									case 15: strcpy( myses->flash,"Compulsory Flash Return light detected");
									case 25: strcpy( myses->flash,"Flash Auto-mode");
									case 29: strcpy( myses->flash,"Flash Auto-mode Return light not detected");
									case 31: strcpy( myses->flash,"Flash Auto-mode Return light detected");
									case 65: strcpy( myses->flash,"Red Eye");
									case 69: strcpy( myses->flash,"Red Eye Return light not detected");
									case 71: strcpy( myses->flash,"Red Eye Return light detected");
									case 73: strcpy(myses->flash,"Red Eye Compulsory Flash");
									case 77: strcpy(myses->flash,"Red Eye Compulsory Flash Return light not detected");
									case 79: strcpy(myses->flash,"Red Eye Compulsory Flash Return light detected");
									case 89: strcpy( myses->flash,"Red Eye Auto-mode");
									case 93: strcpy( myses->flash,"Red Eye Auto-mode Return light not detected");
									case 95: strcpy( myses->flash,"Red Eye Auto-mode Return light detected");
								}
							}
						}
						else if(tagno==0x829D)	//Aperture care de fapt este FNumber
						{
							
							
							snprintf(myses->aperture,32,"f/ %1.1f",(double)pow(1.4142,(int)(res * pow (10,7))));
							
						}
						else if(tagno==0x8827)	//ISOSpeedRatings..nu amgasit iso_equivalent
						{
							myses->iso_equiv=(int)res;
						}
						else if(tagno==0x9208)	//white_balance care de fapt este LightSource
						{
							strcpy(myses->white_balance,"Unknown");
							switch ((int)res){
								case 0: strcpy(myses->white_balance,"Auto");
								case 1: strcpy(myses->white_balance,"Daylight");
								case 2: strcpy(myses->white_balance,"Fluorescent");
								case 3: strcpy(myses->white_balance,"Tungsten");
								case 10: strcpy(myses->white_balance,"Flash");
								case 17: strcpy(myses->white_balance,"Standard Light A");
								case 18: strcpy(myses->white_balance,"Standard Light B");
								case 19: strcpy(myses->white_balance,"Standard Light C");
								case 20: strcpy(myses->white_balance,"D55");
								case 21: strcpy(myses->white_balance,"D65");
								case 22: strcpy(myses->white_balance,"D75");
								case 255: strcpy(myses->white_balance,"Other");
							}
							
						}
						else if(tagno==0x9207)	//metering
						{
							strcpy(myses->metering,"Unknown");
							switch ((int)res){
								case 0: strcpy(myses->metering,"Unknown"); 
								case 1: strcpy(myses->metering,"Average"); 
								case 2: strcpy(myses->metering,"Center Weighted Average");
								case 3: strcpy(myses->metering,"Spot");
								case 4: strcpy(myses->metering,"Multi-Spot");
								case 5: strcpy(myses->metering,"Multi-Segment");
								case 6: strcpy(myses->metering,"Partial");
								case 255: strcpy(myses->metering,"Other");
							}
							
						}
						else if(tagno==0x8822)	//exposure program
						{
							strcpy(myses->exposure_program,"Unknown");
							switch ((int)res){
								case 1: strcpy(myses->exposure_program,"Manual");
								case 2: strcpy(myses->exposure_program,"Program");
								case 3: strcpy(myses->exposure_program,"Aperture Priority");
								case 4: strcpy(myses->exposure_program,"Shutter Priority");
								case 5: strcpy(myses->exposure_program,"Program Creative");
								case 6: strcpy(myses->exposure_program,"Program Action");
								case 7: strcpy(myses->exposure_program,"Portrat"); 
								case 8: strcpy(myses->exposure_program,"Landscape");
							}
						}
						else if(tagno==0x920A) //FocalLength
						{
							myses->focal_length=(double)res * pow (10,7);
						}
					}
				
				
			}
			
			bincopy(DirEntryInfo,nxtch,9,4,12);
			NextChunk=get32u(nxtch,ByteAlign);
			if(NextChunk>LenExif)
			{
				break;
			}
			else if(NextChunk==0)
			{
				break; //numai sunt Directory Entry
			}
			CurrentOffset=(i*12)+11;
			
		}
		else
		{
			break;
		}
		
	}while(NoOfDirEntries!=0);
	
	//free(TagName);
	TagName=NULL;
	free(DirEntryInfo);
	
	free(mBuff);	
	//mBuff=NULL;
myEND:	
	
	free(mstrerror);
	fclose(fd);
	printf("ReadExif\n");
	return mRet;


}
static char * exif_get_tagname(int tag_num, char *ret, int len, tag_table_type tag_table)
{
	int i, t;
	char tmp[32];

	for (i=0;;i++) {
		if ((t=tag_table[i].Tag) == tag_num || t==TAG_END_OF_LIST) {
			if (t==TAG_END_OF_LIST) {
				break;
			}
			if (ret && len)  {
				strncpy(ret, tag_table[i].Desc, abs(len));
				if (len<0) {
					len = -len;
					ret[len-1]='\0';
					for(i=strlen(ret);i<len;i++)ret[i]=' ';
				}
				ret[len-1]='\0';
				return ret;
			}
			return tag_table[i].Desc;
		}
	}
	if (ret && len) {
		snprintf(tmp, sizeof(tmp), "UndefinedTag:0x%04X", tag_num);
		strncpy(ret, tmp, abs(len));
		if (len<0) {
			len = -len;
			ret[len-1]='\0';
			for(i=strlen(ret);i<len;i++)ret[i]=' ';
		}
		ret[len-1]='\0';
		return ret;
	}
	return "";
}
void bincopy(char* src,char* dest,int start,int stop,int totallen){
	char *srcref=NULL;
	char *stmp=NULL;
	int i=0;
	srcref=(char*)malloc(totallen+1);
	
	stmp=srcref;
	
	memcpy(srcref,src,totallen);
	for(i=0;i<start-1;i++){*srcref++;};
	memcpy(dest,srcref,stop);
	
	srcref=stmp;
	//free(stmp);
	free(srcref);

}
static int unsigned get32s(void *value, MY_BYTE_ALIGN motorola_intel)
{
	if (motorola_intel==MOTOROLA_FORMAT) 
	{
		return  (((char  *)value)[0] << 24)
			  | (((unsigned char *)value)[1] << 16)
			  | (((unsigned char *)value)[2] << 8 )
			  | (((unsigned char *)value)[3]      );
	} 
	else 
	{
		return  (((char  *)value)[3] << 24)
			  | (((unsigned char *)value)[2] << 16)
			  | (((unsigned char *)value)[1] << 8 )
			  | (((unsigned char *)value)[0]      );
	}
}
static unsigned int get16s(void *value, MY_BYTE_ALIGN motorola_intel)
{
	if (motorola_intel==MOTOROLA_FORMAT) 
	{
		return  (((char  *)value)[0] << 8)
			  | (((unsigned char *)value)[1]);
			  
	} 
	else 
	{
		return  (((char  *)value)[1] << 8)
			  | (((unsigned char *)value)[0]);
			  
	}
}
static int TypeOfTag(int m)
{
	switch(m){
		case 1: return 1;
		case 2: return 1;
		case 3: return 2;
		case 4: return 4;
		case 5: return 8;
		case 6: return 1;
		case 7: return 1;
		case 8: return 2;
		case 9: return 4;
		case 10: return 8;
		case 11: return 4;
		case 12: return 8;
	}
	return 0;
}
static unsigned get32u(void *value, MY_BYTE_ALIGN motorola_intel)
{
	return (unsigned)get32s(value, motorola_intel) & 0xffffffff;
}

static int get16u(void *value, MY_BYTE_ALIGN motorola_intel)
{
	if (motorola_intel==MOTOROLA_FORMAT) 
	{
		return (((unsigned char *)value)[0] << 8) | ((unsigned char *)value)[1];
	} else {
		return (((unsigned char *)value)[1] << 8) | ((unsigned char *)value)[0];
	}
}


static double exif_convert_any_format(void *value, int format, MY_BYTE_ALIGN motorola_intel)
{
	int 		s_den;
	unsigned 	u_den;

	switch(format) {
		case TAG_FMT_SBYTE:     return *(signed char *)value;
		case TAG_FMT_BYTE:      return *(unsigned char *)value;

		case TAG_FMT_USHORT:    return get16u(value, motorola_intel);
		case TAG_FMT_ULONG:     return get32u(value, motorola_intel);

		case TAG_FMT_URATIONAL:
			u_den = get32u(4+(char *)value, motorola_intel);
			if (u_den == 0) {
				return 0;
			} else {
				return (double)get32u(value, motorola_intel) / u_den;
			}

		case TAG_FMT_SRATIONAL:
			s_den = get32s(4+(char *)value, motorola_intel);
			if (s_den == 0) {
				return 0;
			} else {
				return (double)get32s(value, motorola_intel) / s_den;
			}

		case TAG_FMT_SSHORT:    return (signed short)get16u(value, motorola_intel);
		case TAG_FMT_SLONG:     return get32s(value, motorola_intel);

		
		case TAG_FMT_SINGLE:
			return (double)*(float *)value;
		case TAG_FMT_DOUBLE:
			return *(double *)value;
	}
	return 0;
}
