#include "RF_Toggle_LED_Demo.h"
#include "ICRF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Captures = 0;
int Captures1 = 0;
char skew_buffer[256];


unsigned long get_localtime()
{
//	Local_timevalue = timer_count*32768+TA0R;
	timevalue = Local_timevalue+TA1R;

	if(Local_timevalue < 2147483647)
		{}
	else
		{
		local_overflow++;
		Local_timevalue = 0;
		}
	return timevalue;
}

void get_globaltime()
{

}

void ClockSkewCalculation(float cfo_reg, int IRCF_reading, int IRCF_local)
{
//input: sys_rf, rxbuffer[1],CFO, f_sys_t
//output: frequency skew
	N_pll = 16.69;
	f_nominal = 32768;

		skew = (1/N_pll*((float)cfo_reg*1.59*1000.0) + (((float)IRCF_reading-(float)IRCF_local)*2/NCAPTURES1)*f_nominal)/((float)sys_rf*2/NCAPTURES1);
//		skew = skew *16;
		skew_ratio = skew/f_nominal+1;

}

void SoftwareFreqAdjust()
{
//input: calculate skew, previous CCR0
//output: New CCR0
	last_period = period_assign;
//	skew_new = (skew_ratio + skew_new )/2;
	period_assign = (unsigned int)(16384*skew_ratio);

	TA1CCR0 = period_assign - 1;
//	printf("new period %d %f %d \r\n",(int)TA1CCR0,skew,period_assign);
}

void LRskewAdjust(float skew_calculated)
{
	period_assign = (unsigned int)(16384+skew_calculated);

	TA1CCR0 = period_assign - 1;
//	printf("new period %d %f %d \r\n",(int)TA1CCR0,skew,period_assign);
}


long get_offset(unsigned long authoritative_time, unsigned long local_time_record)
{
	offset = (long) authoritative_time - (long)local_time_record;
	return offset;
}

unsigned long correct_offset(unsigned long offset)
{
	Local_timevalue = Local_timevalue + offset;
//	if (offset >= 0)
//		{
//		Local_timevalue = Local_timevalue + offset;
//		}
//	else
//		{
//		Local_timevalue = Local_timevalue - (unsigned long)(offset&0x7FFFFFFF);
//		}
	return Local_timevalue;
}

float linear_regression(int y[], int topn)
{
	int x1,y1,x1y1,x12;
	int i=0;
	float a0,a1,e;
	for(i=0;i<topn;i++)
	{
		x1+=i;
		y1+=y[i];
		x1y1+=(i*y[i]);
		x12+=(i*i);
	}

	a1=(topn*x1y1-x1*y1)/(topn*x12-x1*x1);
	a0=y1/topn-a1*x1/topn;
	e=y[0]-a0-a1*1;
	a0+=e;

return a1;
}

int get_cfo(void)
{
	unsigned char raw_cfo, converter_cfo;
	int converted_cfo;

	raw_cfo = ReadSingleReg(FREQEST);

	if(raw_cfo>127)
	{
		converter_cfo=(~(raw_cfo-1));
		converted_cfo=-converter_cfo;
	}
	else
	{
		converter_cfo=raw_cfo;
		converted_cfo=converter_cfo;
	}

	return converted_cfo;
}


