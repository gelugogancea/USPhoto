
#include "usprotocol.h"

int ck_album_name(MYSQL*,char*,int);

int ck_album_name(MYSQL* myData,char* mname,int userid)
{
	MYSQL_RES *mRes;
	int nrRows=0;
	char mSQLExec[1024];
	char *MYSQLerr=NULL;
	
	memset(mSQLExec,0x0,1024);
	sprintf(mSQLExec,"select id_roll from rolls where id_user='%d' and name_roll='%s';",userid,mname);
	if((mysql_real_query(myData,mSQLExec,strlen(mSQLExec)))<0)
	{
		
		MYSQLerr=(char*)mysql_error(myData);
		my_error(0,"MyPROCESS:ck_album_name",MYSQLerr,0,0,0);
		return(-1);
	}

	mRes=mysql_store_result(myData);
	nrRows=mysql_num_rows(mRes);
	
	mysql_free_result(mRes);
	return (nrRows);
	
}

int usLogon(MYSQL* myData,char* mUser,char* mPass)
{
	MYSQL_RES *mRes;
	MYSQL_FIELD *mFields;
	MYSQL_ROW mRow;
	int nrRows=0,nrFields=0,i=0,x=0;
	char mSQLExec[1024];
	char *MYSQLerr=NULL;
	int USER_ID=0;
	memset(mSQLExec,0x0,1024);
	
	sprintf(mSQLExec,"select id_user from users where username='%s' and password=MD5('%s');",mUser,mPass);
	if((mysql_real_query(myData,mSQLExec,strlen(mSQLExec)))<0)
	{
		MYSQLerr=(char*)mysql_error(myData);
		my_error(0,"MyPROCESS:USLOGON",MYSQLerr,0,0,0);
		return(-1);
	}

	mRes=mysql_store_result(myData);

	if(!mRes)
	{
		my_error(0,"MyPROCESS:USLOGON","DB ERROR mysql_store_result\n",0,0,0);
		USER_ID=-1;	
		goto myEND;
	}
	nrRows=mysql_num_rows(mRes);
	if(nrRows==0)
	{
		goto myEND;
	}
	nrFields=mysql_num_fields(mRes);
	mFields=mysql_fetch_fields(mRes);
	while ((mRow = mysql_fetch_row(mRes)))
	{
		if(!mRow)
		{
			my_error(0,"MyPROCESS:USLOGON","DB ERROR FETCH ROW\n",0,0,0);
		}
			x++;
			
			for(i=0;i<nrFields;i++)
			{
				USER_ID=atoi(mRow[i]);
			}
	
	}


myEND:
	mysql_free_result(mRes);
	return(USER_ID);
}
char* usListAlbum(MYSQL* myData,USPHOTO_SESSION* mses)
{
	MYSQL_RES *mRes;
	MYSQL_FIELD *mFields;
	MYSQL_ROW mRow;
	int nrRows=0,nrFields=0,i=0,x=0;
	char mSQLExec[1024],*mySQLErr=NULL;
	char* mList=NULL;
	
	memset(mSQLExec,0x0,1024);
	mList=(char*)malloc(16384);
	memset(mList,0x0,16384);
	sprintf(mSQLExec,"select name_roll from rolls where id_user='%d';",mses->IDUser);
	if((mysql_real_query(myData,mSQLExec,strlen(mSQLExec)))<0)
	{
		mySQLErr=(char*)mysql_error(myData);
		my_error(0,"MyPROCESS:usListAlbum",mySQLErr,0,0,0);
		free(mList);
		return(char*)(-1);
	}

	mRes=mysql_store_result(myData);

	if(!mRes)
	{
		my_error(0,"MyPROCESS:usListAlbum","DB ERROR mysql_store_result\n",0,0,0);
		mList=(char*)-1;	
		goto myEND;
	}
	nrRows=mysql_num_rows(mRes);
	if(nrRows==0)
	{
		goto myEND;
	}
	nrFields=mysql_num_fields(mRes);
	mFields=mysql_fetch_fields(mRes);
	
	while ((mRow = mysql_fetch_row(mRes)))
	{
		if(!mRow)
		{
			my_error(0,"MyPROCESS:usListAlbum","DB ERROR FETCH ROW\n",0,0,0);
		}
			x++;
			
			for(i=0;i<nrFields;i++)
			{
				strcat(mList,mRow[i] ? mRow[i] : "NULL");
				strcat(mList,"\n");
				
			}
	
	}
	

myEND:
	mList[strlen(mList)]='\r';
	mysql_free_result(mRes);
	
	return(mList);
}
long usInsertDataPictures(MYSQL* myData,USPHOTO_SESSION* mSession)
{
	char mSQLExec[1024],*mySQLErr=NULL;
	long myid=0;
	
	memset(mSQLExec,0x0,1024);
	sprintf(mSQLExec,"insert into pictures SET id_roll=%d,id_user=%d,title='%s',filename='%s';" \
			,mSession->IDAlbum,mSession->IDUser,mSession->fTitle,mSession->fFileName);
	
	if((mysql_real_query(myData,mSQLExec,strlen(mSQLExec)))<0)
	{
		mySQLErr=(char*)mysql_error(myData);
		my_error(0,"MyPROCESS:usInsertDataPictures",mySQLErr,0,0,0);
		return(-1);
	}
	
	myid=mysql_insert_id(myData);
	if(myid<=0)
	{
		my_error(0,"MyPROCESS:usInsertDataPictures","DB ERROR NU AM FILE ID\n",0,0,0);
		return(0);
	}
	else
	{
		mSession->IDFile=myid;
	}

	return(1);
}
int usCreateAlbum(MYSQL* myData,int USER_ID,char* AlbumName)
{
	char mSQLExec[1024],*mySQLErr=NULL;
	int myid=0,myres=0;
	myres=ck_album_name(myData,AlbumName,USER_ID);
	if(myres==0)
	{
		memset(mSQLExec,0x0,1024);
		sprintf(mSQLExec,"insert into rolls set id_user=%d,name_roll='%s';",USER_ID,AlbumName);
	
		if((mysql_real_query(myData,mSQLExec,strlen(mSQLExec)))<0)
		{
			mySQLErr=(char*)mysql_error(myData);
			my_error(0,"MyPROCESS:usCreateAlbum",mySQLErr,0,0,0);
			return(-1);
		}
	
		myid=mysql_insert_id(myData);
		if(myid<=0)
		{
			my_error(0,"MyPROCESS:usCreateAlbum","DB insert 0 ROWS",0,0,0);
			return(0);
		}
		else
	
		return(myid);
	}
	else if(myres<0)
	{
		return (-1);
	}
	else
	{
		return(0);
	}
}
int usUpdateDataPictures(MYSQL* myData,USPHOTO_SESSION* mSession)
{
	char mSQLExec[1024],*mySQLErr=NULL;
	int myid=0;
	
	memset(mSQLExec,0x0,1024);
	snprintf(mSQLExec,1024,"update pictures SET resume=1,xsize='%d',ysize='%d',exif_datetime='%s',exif_make='%s',exif_model='%s',exif_flash='%s', \
		exif_focal_length='%2.2f',exif_exposure='%2.2f',exif_aperture='%s',exif_iso_equiv='%d',exif_white_balance='%s', \
		exif_metering='%s',exif_exposure_program='%s' where id_poza=%d;",(int)mSession->usexif->xsize,(int)mSession->usexif->ysize, \
		mSession->usexif->datetime,mSession->usexif->make,mSession->usexif->model,mSession->usexif->flash, \
		mSession->usexif->focal_length,mSession->usexif->exposure,mSession->usexif->aperture,mSession->usexif->iso_equiv, \
		mSession->usexif->white_balance,mSession->usexif->metering,mSession->usexif->exposure_program,(int)mSession->IDFile);

	printf("Query :\n %s\n",mSQLExec);	
	if((mysql_real_query(myData,mSQLExec,strlen(mSQLExec)))<0)
	{
		mySQLErr=(char*)mysql_error(myData);
		my_error(0,"MyPROCESS:usUpdateDataPictures",mySQLErr,0,0,0);
		return(-1);
	}
	
	myid=mysql_affected_rows(myData);
	if(myid<=0)
	{
		my_error(0,"MyPROCESS:usUpdateDataPictures","DB UPDATE o ROWS",0,0,0);
		return(0);
	}
	else
	
	return(myid);
	
}
int usSelectAlbum(MYSQL* myData,USPHOTO_SESSION* mses,char* AlbumName)
{
	MYSQL_RES *mRes;
	MYSQL_FIELD *mFields;
	MYSQL_ROW mRow;
	int nrRows=0,nrFields=0,i=0;
	char mSQLExec[1024];
	int mErr=0;
	
	memset(mSQLExec,0x0,1024);
	sprintf(mSQLExec,"select id_roll from rolls where id_user=%d and name_roll='%s';",mses->IDUser,AlbumName);
	if((mysql_real_query(myData,mSQLExec,strlen(mSQLExec)))<0)
	{
		my_error(0,"MyPROCESS:usSelectAlbum","DB ERROR NU POT SA EXECUT QUERY\n",0,0,0);
		return(-1);
	}

	mRes=mysql_store_result(myData);

	if(!mRes)
	{
		my_error(0,"MyPROCESS:usSelectAlbum","DB ERROR mysql_store_result\n",0,0,0);
		mErr=-1;	
		goto myEND;
	}
	nrRows=mysql_num_rows(mRes);
	if(nrRows==0)
	{
		goto myEND;
	}
	else
	{
		mErr=nrRows;
	}
	nrFields=mysql_num_fields(mRes);
	mFields=mysql_fetch_fields(mRes);
	while ((mRow = mysql_fetch_row(mRes)))
	{
		if(!mRow)
		{
			my_error(0,"MyPROCESS:usSelectAlbum","DB ERROR FETCH ROW\n",0,0,0);
		}
			for(i=0;i<nrFields;i++)
			{
				mses->IDAlbum=atoi(mRow[i]);
				
			}
	
	}
	

myEND:
	mysql_free_result(mRes);
	return(mErr);	
}
short int usGiveMeIDForResume(MYSQL* myData,USPHOTO_SESSION* mses)
{
	MYSQL_RES *mRes;
	MYSQL_FIELD *mFields;
	MYSQL_ROW mRow;
	int nrRows=0,nrFields=0,i=0;
	short int x=0;
	char mSQLExec[1024],*mySQLErr=NULL;
		
	memset(mSQLExec,0x0,1024);
		
	sprintf(mSQLExec,"select id_poza from pictures where filename='%s';",mses->fFileName);
	printf("QUERY : %s\n",mSQLExec);
	if((mysql_real_query(myData,mSQLExec,strlen(mSQLExec)))<0)
	{
		mySQLErr=(char*)mysql_error(myData);
		my_error(0,"MyPROCESS:usGiveMeIDForResume",mySQLErr,0,0,0);
		return(-1);
	}

	mRes=mysql_store_result(myData);
	printf("Ajung mRes \n");
	if(!mRes)
	{
		my_error(0,"MyPROCESS:usGiveMeIDForResume","DB ERROR mysql_store_result\n",0,0,0);
		x=-1;	
		goto myEND;
	}
	printf("Ajung dupa !mRes \n");
	nrRows=mysql_num_rows(mRes);
	printf("nrRows : %d \n",nrRows);
	if(nrRows==0)
	{
		goto myEND;
	}
	nrFields=mysql_num_fields(mRes);
	mFields=mysql_fetch_fields(mRes);
	
	while ((mRow = mysql_fetch_row(mRes)))
	{
		if(!mRow)
		{
			my_error(0,"MyPROCESS:usGiveMeIDForResume","DB ERROR FETCH ROW\n",0,0,0);
		}
						
			for(i=0;i<nrFields;i++)
			{
				mses->IDFile=atol(mRow[i] ? mRow[i] : 0);
				x++;
				
				
			}
	
	}
	

myEND:
	
	mysql_free_result(mRes);
	
	return(x);
}
