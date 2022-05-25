#include <stdlib.h>
#include <string.h>



void leftcopy(char* input,char* output,int pos,int BUFF_SIZE)
{
	int index = 0,i;
	
	for(i = 0; i < pos; i++)
	{
		output[index] = input[i];
		index++;
	}
	output[index] = 0;
}

void rightcopy(char* input,char* output,int pos,int BUFF_SIZE)
{
	int index = 0,i;
	int len = BUFF_SIZE;
	for(i = (BUFF_SIZE-pos); i < len; i++)
	{
		output[index] = input[i];
		index++;
	}
	//output[index+1]='\0';
	output[index] = 0;
	
}

void midcopy(char* input,char* output,int start_pos, int stop_pos,int BUFF_SIZE)
{
	int index = 0,i;
		
	for(i = start_pos-1; i < stop_pos; i++)
	{
		output[index] = input[i];
		index++;
	}

	output[index] = 0;
	
}
