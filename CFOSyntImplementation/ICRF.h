#define NCAPTURES 320
#define NCAPTURES1 1

unsigned int starttime0, starttime1, starttimeTA1;
unsigned int Octo_sys, Octo_rf, sys_rf;
unsigned int timer1,timer2;
unsigned int store0[2];
unsigned int store1[2];
int timer_count;

unsigned long timevalue;
unsigned long Local_timevalue;
unsigned int local_overflow;
float skew;
float skew_ratio;
float sw_skew;
unsigned int last_period;
unsigned int period_assign;
float N_pll;
long f_nominal;
unsigned int Packet_ID;
unsigned long Local_time;
unsigned int print_no;

void TimerInit( void);
void ClockSkewCalculation(float cfo_reg, int IRCF_reading, int IRCF_local);
void SoftwareFreqAdjust(void);
long get_offset(unsigned long authoritative_time, unsigned long local_time_record);
unsigned long get_localtime();
void get_globaltime();
unsigned long correct_offset();
float linear_regression(int y[], int topn);
void LRskewAdjust(float skew);

int get_cfo(void);
// synt functions




//float skew=0;
//float skew_ratio=0;
//float skew_new = 0;
//int last_period = 32767;
//int period_assign =32767;
//float N_pll = 16.69;
//unsigned int f_nominal = 32768;
//unsigned int Packet_ID = 0;
//unsigned int Local_time = 0;
//unsigned int print_no = 0;
//float N_pll = 16.69;
//float f_nominal = 32768.0;
