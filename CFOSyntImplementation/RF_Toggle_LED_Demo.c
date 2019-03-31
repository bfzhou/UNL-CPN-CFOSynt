/******************************************************************************
Author: Baofeng Zhou
Adopted from TI's example code
******************************************************************************/

#include "RF_Toggle_LED_Demo.h"
#include "ICRF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * ----------------------------------------------------------
 * RF definition
 * ----------------------------------------------------------
 */

//#define  PACKET_LEN_MAX		64-1
#define  PACKET_LEN         (0x0A)          // PACKET_LEN <= 61
#define  RSSI_IDX           (PACKET_LEN)    // Index of appended RSSI
#define  CRC_LQI_IDX        (PACKET_LEN+1)  // Index of appended LQI, checksum
#define  CRC_OK             (BIT7)          // CRC_OK bit
#define  PATABLE_VAL        (transmit_power_10dbm)          // 10 dBm output


//#define CALADC12_15V_30C  *((unsigned int *)0x1A1A)// ADC 1.5V reference Temp sensor at 30C is stored at addr 0x01A1Ah
//#define CALADC12_15V_85C  *((unsigned int *)0x1A1C)// ADC 1.5V reference Temp sensor at 85C is stored at addr 0x01A1Ch
//#define CALADC12_20V_30C  *((unsigned int *)0x1A1E)// ADC 2.0V reference Temp sensor at 30C is stored at addr 0x01A1Ah
//#define CALADC12_20V_85C  *((unsigned int *)0x1A20)// ADC 2.0V reference Temp sensor at 85C is stored at addr 0x01A1Ch
//#define ref_factor *((unsigned int *)0x1A28)

extern RF_SETTINGS rfSettings;


unsigned char RxBuffer[PACKET_LEN+2];
unsigned char RxBufferLength = 0;

unsigned char TxBuffer[PACKET_LEN];
//char TxBuffer2[PACKET_LEN_MAX];
//unsigned char Trans_trigger = 0;
unsigned char transmitting = 0;
unsigned char receiving = 0;

unsigned char rf_isr = 0;
unsigned char timer_isr = 0;
unsigned char ircf_isr = 0;

unsigned int i = 0;

char clk_buffer[512];



/*
 * ----------------------------------------------------------
 * ADC definition
 * ----------------------------------------------------------
 */
//ADC readout
//volatile int in_temp, in_volt;
//volatile float IntDegF;
//volatile float IntDegC;
//volatile float temp,volt;
//volatile float degC_per_bit;
////adc references
//unsigned int bits30_15, bits85_15;
//unsigned int bits30_20, bits85_20;
//unsigned int bits30_20, bits85_20;
//unsigned int ref_volt;
//char adc_buffer[128];

//RTC
int year,mon,date,hour,min,sec;
int flag = 1;
int protoc = Protocol_On ;
int SYNC_head= -1;

unsigned int sync_word = 255;

/*
 * ----------------------------------------------------------
 * Function definition
 * ----------------------------------------------------------
 */
//float Print_temp_vcc(void);
void construction(unsigned int t1,  unsigned int t2,  unsigned int t3,  unsigned long t4, unsigned int t5);

syntpacket mysend;
syntreceive myreceive;
stack offset_stack;

int sendinterval;
int sync_interval;
int receiveinterval;

void main( void )
{
  // Stop watchdog timer to prevent time out reset
	WDTCTL = WDTPW + WDTHOLD;
//	Strat watchdog timer.
//	WDTCTL = WDTPW + WDTCNTCL;

  PortInit();
  // Increase PMMCOREV level to 2 for proper radio operation
  SetVCore(2);
  ResetRadioCore();
  InitRadio();

  InitButtonLeds();

//  DCOInit();
//  RTCInit();
  UartInit();
//  ADCInit();
  TimerInit();

  Local_timevalue = 0;
  IdentityInit();

  local_overflow = 0;
  time_last = 0;

  while (1)
  {

		__bis_SR_register( LPM0_bits + GIE );
	    __no_operation();

	if (transmitting)                      // Process a button press->transmit
	{
      P1IFG = 0;
      P1IE |= BIT4;
      P1IE |= BIT5;
      P1IE |= BIT6;// Re-enable button press
    }
    if(receiving)
    {
      P1IE |= BIT4;
      P1IE |= BIT5;
      P1IE |= BIT6;// Re-enable button press
    }

    if(ircf_isr)
    {
        TA0CCTL2 &= ~CCIE;
        ircf_isr = 0;
    }

    if(timer_isr)
	{
		ReceiveOff();

		if(Node_ID == 1)
		{
			switch (sendinterval){
				case 32:
					TA0CCTL2 &= ~CCIFG;
					TA0CCTL2 &= ~COV;
					TA1CCTL0 &= ~CCIFG;
					TA1CCTL0 &= ~COV;
					TA1CCTL0 &= ~CCIE;
					TA0CCTL2|= CCIE;


					if(sync_interval == 9)
					{
					   Packet_ID++;
					   if(sys_rf<(10000*NCAPTURES1))
						{
							time_to_send= get_localtime();
							construction(Node_ID,Packet_ID,sys_rf,time_to_send,1);
							Transmit( (unsigned char*)TxBuffer, sizeof TxBuffer);
							P3OUT ^= BIT6;
						}
					   else
					   {
						    __no_operation();
					   }
					sendinterval = 0;
					sync_interval = 0;
					}
					else
					{
						if(sys_rf<(10000*NCAPTURES1))
						{
							Packet_ID++;
							time_to_send= get_localtime();
			//	        	printf("%lu yo\r\n",time_to_send);
							construction(Node_ID,Packet_ID,sys_rf,time_to_send,0);
							Transmit( (unsigned char*)TxBuffer, sizeof TxBuffer);
							P3OUT ^= BIT6;
						}
					   else
					   {
							__no_operation();
					   }

						    putstr("packet sent\r\n");
						sync_interval++;
						sendinterval = 0;

					}
				default:
					sendinterval++;
					break;
				}
		}
		else
		{
			ReceiveOn();
		}
    	timer_isr = 0;

	}

	if(rf_isr)
	{
		ReceiveOff();
		TA0CCTL2 &= ~CCIFG;
		TA0CCTL2 &= ~COV;
		TA1CCTL0 &= ~CCIFG;
		TA1CCTL0 &= ~COV;
		TA1CCTL0 &= ~CCIE;
		TA0CCTL2|= CCIE;


        __no_operation();
//		sprintf(receive_ircf,"%d %d",RxBuffer[bit_packetID_1],RxBuffer[bit_packetID_2]);
		packet_int = (unsigned int)  (((unsigned int)RxBuffer[bit_packetID_1]<<8 )+ RxBuffer[bit_packetID_2]);

//		sprintf(receive_ircf,"%d %d",RxBuffer[bit_ircf_1],RxBuffer[bit_ircf_2]);
		icrf_int = (unsigned int) (((unsigned int)RxBuffer[bit_ircf_1]<<8) + RxBuffer[bit_ircf_2]);

//		sprintf(receive_time,"%d %d %d %d",RxBuffer[bit_localtime_1],RxBuffer[bit_localtime_2],RxBuffer[bit_localtime_3],RxBuffer[bit_localtime_4]);
		time_int = (unsigned long) (((unsigned long)RxBuffer[bit_localtime_1]<<24) + ((unsigned long)RxBuffer[bit_localtime_2]<<16) + ((unsigned long)RxBuffer[bit_localtime_3]<<8) +((unsigned long)RxBuffer[bit_localtime_4]));

			if(icrf_int<(10000*NCAPTURES1) && sys_rf<(10000*NCAPTURES1))
			{
				myreceive.nodeid = RxBuffer[0];
				myreceive.synt_seq = packet_int;
				myreceive.IRCF_value = icrf_int;
				myreceive.local_time_value = time_int;
				myreceive.global_time_value = time_int;
				myreceive.CFO = get_cfo();

				if(protoc == Protocol_On)
				{
					if(RxBuffer[bit_SYNC] == 1)
					{
						ClockSkewCalculation(myreceive.CFO, myreceive.IRCF_value,sys_rf);
//						skew = linear_regression(offset_stack.stk,offset_stack.top);
						offset = get_offset(myreceive.global_time_value,myreceive.recv_timestamp);

//						Local_timevalue = (unsigned long)((long)Local_timevalue  - (long)skew*32); //skew compensation
//						Local_timevalue = (unsigned long)((long)Local_timevalue  *skew_ratio); //skew compensation2
						correct_offset(offset);

//    								SoftwareFreqAdjust();
//    								LRskewAdjust(-skew);
//    								lr_result = linear_regression(offset_stack.stk,offset_stack.top);
//    								LRskewAdjust(lr_result);

						//Linear regression
						offset_stack.top = 0;
					}
					else
					{
						offset = get_offset(myreceive.global_time_value,myreceive.recv_timestamp);
//						Local_timevalue = (unsigned long)((long)Local_timevalue  - (long)skew*32); //skew compensation

						//Linear regression
						offset_stack.stk[offset_stack.top]=offset;
						offset_stack.top++;
					}



				}
				else
				{
					if(RxBuffer[bit_SYNC] == 1)
					{
						offset = get_offset(myreceive.global_time_value,myreceive.recv_timestamp);
						correct_offset(offset);

						//Linear regression
						offset_stack.top = 0;
					}
					else
					{
						offset = get_offset(myreceive.global_time_value,myreceive.recv_timestamp);
						correct_offset(offset);
						//Linear regression
						offset_stack.stk[offset_stack.top]=offset;
						offset_stack.top++;
					}
				}

					/*
					* Print local data Print received data print skew
					*/
						sprintf(clk_buffer,"%d %.3f %.5f %u %u %u %lu %u %u %u %lu %d %d %u %ld %ld %ld\r\n",protoc,skew, skew_ratio,Node_ID,print_no,sys_rf, myreceive.recv_timestamp,RxBuffer[bit_Node_ID],packet_int,icrf_int,time_int, get_cfo(),RxBuffer[bit_SYNC],offset_stack.top,offset,get_localtime(),(get_localtime()-time_int));
						//sprintf(clk_buffer,"%u = local_sysrf ", sys_rf);
						putstr(clk_buffer);

						print_no++;
						time_last = time_int;
					}
			rf_isr = 0;
			ReceiveOn();

		}





  }
}
/*
 * ----------------------------------------------------------
 * Identity initizalization
 * ----------------------------------------------------------
 */
void IdentityInit(void)
{
	/*only sender can send peroidically*/
	  if(Node_ID == 1)
	  {
		  transmitting = 1;
	      ReceiveOff();
	      receiving = 0;
		  __no_operation();
	  }
	  else
	  {
		  transmitting = 0;
		  receiving = 1;
		  ReceiveOn();
		  __no_operation();
	  }

}

/*
 * ----------------------------------------------------------
 * UART
 * ----------------------------------------------------------
 */
void UartInit( void)
{

    PMAPPWD = 0x02D52;                        // Get write-access to port mapping regs
    P2MAP0 = PM_UCA0RXD;                      // Map UCA0RXD output to P1.5
    P2MAP1 = PM_UCA0TXD;                      // Map UCA0TXD output to P1.6
    PMAPPWD = 0;                              // Lock port mapping registers

    P2DIR |= BIT1;                            // Set P1.6 as TX output
    P2SEL |= BIT0 + BIT1;                     // Select P1.5 & P1.6 to UART function
	/*
	 * 10MHz cpu
	 */
//    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
//    UCA0CTL1 |= UCSSEL__SMCLK;                     // SMCLK=12MHz
//    UCA0BR0 = 6;                              // 1MHz 115200 (see User's Guide)
//    UCA0BR1 = 0;                              // 1MHz 115200
//    UCA0MCTL |= UCBRS_0 + UCBRF_8 +UCOS16;            // Modulation UCBRSx=1, UCBRFx=0
//    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
//    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

    /*
     * 32k ACLK
     */
    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL__SMCLK;                     // SMCLK
    UCA0BR0 = 9;                              // 1MHz 115200 (see User's Guide)
    UCA0BR1 = 0;                              // 1MHz 115200
    UCA0MCTL |= UCBRS_1 + UCBRF_0;            // Modulation UCBRSx=1, UCBRFx=0
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

void putchar1( char ch)
{
  while (!(UCA0IFG&UCTXIFG));                // USCI_A0 TX buffer ready?
  UCA0TXBUF = ch;
}
void putstr( char*str)
{
  for(;*str;str++) putchar1(*str);
}

/*
 * ----------------------------------------------------------
 * timer
 * ----------------------------------------------------------
 */
void TimerInit( void)
{
	  UCSCTL6 &= ~XT2OFF;                       // Enable XT2
	  UCSCTL3 |= SELREF_2;                      // FLLref = REFO
  	  UCSCTL4 = SELA__DCOCLKDIV + SELS__DCOCLKDIV + SELM__DCOCLKDIV;

	  // Loop until XT1,XT2 & DCO stabilizes
	  do
	  {
	    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
	                                            // Clear XT2,DCO fault flags
	    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
	  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

	  UCSCTL4 |= SELA__DCOCLKDIV + SELS__DCOCLKDIV + SELM__XT2CLK;   // SMCLK=MCLK=XT2

    UCSCTL5 |= DIVA__32;

//--------timer and clock configuration----------------------------------------------------
  //Timer:TA0CLK 10M/8=1250kHz
  //_|-|_|-|_
  //CCI:ACLK:32K/2=16kHz
  //_|-----|_


//    WriteSingleReg(IOCFG0,0x35);
//    TA0CTL|=TACLR+TASSEL__TACLK+ID_2+MC__CONTINUOUS; //TACLK=OCTOclk/8=1250kHz
//    TA0CCTL2|=CCIS_1+CAP+SCS+CM_1+CCIE;     //CCIxB:ACLK=32kHz/2=16.384kHz
//    TA0CCTL3|=CCIS_1+CAP+SCS+CM_1+CCIE;     //CCIxB:GDO1=135kHz

//    WriteSingleReg(IOCFG0,0x3F);
//    UCSCTL5|=DIVA__32; //+DIVS__8;
//    TA0CCTL2|=CCIS_1+CAP+SCS+CM_1+CCIE;     //CCIxB:ACLK=32kHz
//    TA0CTL|=TACLR+TASSEL__INCLK+ID_0+MC__CONTINUOUS; //SMCLK=2, TASSEL_3=INCLK=RF/192 +TAIE

    TA0CTL|=TACLR+TASSEL__INCLK+ID_1+MC__CONTINUOUS; //INCLK = 135kHz
    TA0CCTL2|=CCIS_1+CAP+SCS+CM_1;     //CCIxB:ACLK=32.768kHz/2=16.384kHz

    /*software clock*/
    TA1CTL = TASSEL_2 + ID_0 + MC_1 + TACLR;         // ACLK, upmode, clear TAR
    TA1CCTL0 = OUTMOD_4 + CCIE;                      // CCR0 toggle mode
    period_assign = 32768;
    TA1CCR0 = period_assign-1;

}
/*
 * ----------------------------------------------------------
 * RTC
 * ----------------------------------------------------------
 */
void RTCInit(void)
{
	 RTCCTL01 |= RTCRDYIE+RTCTEVIE + RTCRDY + RTCMODE +RTCSSEL_0;

	 RTCCTL01 |= RTCHOLD;

     RTCSEC =  0x2D;
     RTCMIN =  0x3B;
     RTCHOUR = 0x17;
     RTCDOW  =  0x05;
     RTCDAY =  0x19;
     RTCMON =  0x01;
     RTCYEAR = 0x07E1;
     RTCCTL01 &= ~RTCHOLD;
}

/*
 * ----------------------------------------------------------
 * ADC
 * ----------------------------------------------------------
 */
void ADCInit(void)
{
  /* Initialize the shared reference module */
//  REFCTL0 |= REFMSTR + REFVSEL_0 + REFON;    // Enable internal 1.5V reference
  REFCTL0 |= REFMSTR + REFVSEL_1 + REFON;    // Enable internal 2V reference

  /* Initialize ADC12_A */
  ADC12CTL0 = ADC12SHT0_8 + ADC12ON + ADC12MSC;		// Set sample time ADC12REFON
  ADC12CTL1 = ADC12SHP + ADC12CONSEQ_1;                     // Enable sample timer
  ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10;  // ADC input ch A10 => temp sense
  ADC12MCTL1 = ADC12SREF_1 + ADC12INCH_11  +ADC12EOS;  // ADC input ch A10 => temp sense
  ADC12IE = 0x001;                          // ADC_IFG upon conv result-ADCMEMO
//  ADC12CTL2 = ADC12RES_2 + ADC12REFBURST;

  __delay_cycles(75);                       // 35us delay to allow Ref to settle
                                            // based on default DCO frequency.
                                            // See Datasheet for typical settle
                                            // time.
  ADC12CTL0 |= ADC12ENC;
}

//float Print_temp_vcc(void)
//{
//	ADC12CTL0 &= ~ADC12SC;  	// clear the start bit
//	ADC12CTL0 |= ADC12SC;    // Sampling and conversion start
//    bits30_20 = CALADC12_20V_30C;
//    bits85_20 = CALADC12_20V_85C;
//    ref_volt = ref_factor;
//    // Use calibration data stored in info memory
//    degC_per_bit = (float)((float)(85-30)/(float)(bits85_20-bits30_20));
//    temp = ((float)in_temp - bits30_20) * degC_per_bit + 30.0;
//    volt = (float)ref_volt/(float)32768*(float)in_volt/(float)4095*(float)2*(float)2;
//    return temp,volt;
//}

/*
 * ----------------------------------------------------------
 * General Porting/ Button and LED port
 * ----------------------------------------------------------
 */

void PortInit( void)
{
        //--------port selection----------------------------------------------------
                     P2SEL |= BIT3+BIT6+BIT5+BIT7;                       // P2.4 (TA1 out), P2.6 (GDO0 input)
                     P2DIR |= BIT3+BIT6+BIT5+BIT7;                                         // P2.4 TA1CCR1 P2.7 output

                     P2DIR &= ~BIT2;                           // P2.2/TA0.1 Input Capture
                     P2SEL |= BIT2;                            // TA0.1 option select
                     //LED port
                     P1DIR|=BIT7;
                     P3DIR|=BIT6+BIT7;
        //--------port selection end----------------------------------------------------

        //--------port mapping----------------------------------------------------
                     PMAPPWD = 0x02D52;                        // Get write-access to port mapping regs
                     P2MAP3 = PM_RFGDO0;                       // Map GDO0 as an input on P2.5
                     P2MAP6 = PM_MCLK;                                      //MCLK
//                     P2MAP6 = PM_SMCLK;                                      //SMCLK
                     P2MAP5 = PM_ACLK;                                       //ACLK
                     P2MAP2= PM_TA0CLK;
//                     P2MAP4= PM_ACLK;
                     PMAPCTL |= PMAPRECFG;                             // Allow for future port map configurations.
                     PMAPPWD = 0x00;                           // Lock Port mapping
        //--------port mapping end----------------------------------------------------
}
void InitButtonLeds(void)
{
  // Set up the button as interruptible
  P1DIR &= ~BIT6;
  P1REN |= BIT6;
  P1IES &= BIT6;
  P1IFG = 0;
  P1OUT |= BIT6;
  P1IE  |= BIT6;

  // Set up the button as interruptible
  P1DIR &= ~BIT4;
  P1REN |= BIT4;
  P1IES &= BIT4;
  P1IFG = 0;
  P1OUT |= BIT4;
  P1IE  |= BIT4;

  // Set up the button as interruptible
  P1DIR &= ~BIT5;
  P1REN |= BIT5;
  P1IES &= BIT5;
  P1IFG = 0;
  P1OUT |= BIT5;
  P1IE  |= BIT5;

  // Initialize Port J
  PJOUT = 0x00;
  PJDIR = 0xFF;

  // Set up LEDs
  P1OUT &= ~BIT7;
  P1DIR |= BIT7;
  P3OUT &= ~BIT6;
  P3DIR |= BIT6;
  P3OUT &= ~BIT7;
  P3DIR |= BIT7;

}

/*
 * ----------------------------------------------------------
 * Radio operations
 * ----------------------------------------------------------
 */
void InitRadio(void)
{
  // Set the High-Power Mode Request Enable bit so LPM3 can be entered
  // with active radio enabled
  PMMCTL0_H = 0xA5;
  PMMCTL0_L |= PMMHPMRE_L;
  PMMCTL0_H = 0x00;

  WriteRfSettings(&rfSettings);

  WriteSinglePATable(PATABLE_VAL);
}


void Transmit(unsigned char *buffer, unsigned char length)
{
  RF1AIES |= BIT9;
  RF1AIFG &= ~BIT9;                         // Clear pending interrupts
  RF1AIE |= BIT9;                           // Enable TX end-of-packet interrupt

  WriteBurstReg(RF_TXFIFOWR, buffer, length);

  Strobe( RF_STX );                         // Strobe STX
}

void ReceiveOn(void)
{
  RF1AIES |= BIT9;                          // Falling edge of RFIFG9
  RF1AIFG &= ~BIT9;                         // Clear a pending interrupt
  RF1AIE  |= BIT9;                          // Enable the interrupt

  // Radio is in IDLE following a TX, so strobe SRX to enter Receive Mode
  Strobe( RF_SRX );
}

void ReceiveOff(void)
{
  RF1AIE &= ~BIT9;                          // Disable RX interrupts
  RF1AIFG &= ~BIT9;                         // Clear pending IFG

  // It is possible that ReceiveOff is called while radio is receiving a packet.
  // Therefore, it is necessary to flush the RX FIFO after issuing IDLE strobe
  // such that the RXFIFO is empty prior to receiving a packet.
  Strobe( RF_SIDLE );
  Strobe( RF_SFRX  );
}

unsigned char carriersense(void) {
     unsigned char cs_Status;
     ReceiveOn();
     cs_Status =  ReadSingleReg(PKTSTATUS);
     return (cs_Status& 0x40) >> 6;
}

unsigned char ClearCCA(void) {
     unsigned char p_Status;
     ReceiveOn();
     p_Status =  ReadSingleReg(PKTSTATUS);
     return (p_Status& 0x10) >> 4;
}

void construction(unsigned int t1,  unsigned int t2,  unsigned int t3,  unsigned long t4, unsigned int t5)
{
//	memcpy(TxBuffer, t1, sizeof(t1));
//	memcpy(&TxBuffer[sizeof(t1)], t2, sizeof(t2));
//	memcpy(&TxBuffer[sizeof(t1)+sizeof(t2)], t3, sizeof(t3));
//	memcpy(&TxBuffer[sizeof(t1)+sizeof(t2)+sizeof(t3)], t4, sizeof(t4));
//	memcpy(&TxBuffer[sizeof(t1)+sizeof(t2)+sizeof(t4)], t5, sizeof(t5));

        TxBuffer[0] = (unsigned char) t1 & 0xFF;

        TxBuffer[1] = (unsigned char) (t2 >> 8) & 0xFF;
        TxBuffer[2] = (unsigned char) t2 & 0xFF;

        TxBuffer[3] = (unsigned char) (t3 >> 8) & 0xFF;
        TxBuffer[4] = (unsigned char) t3 & 0xFF;

        TxBuffer[5] = (unsigned char) (t4 >> 24) & 0xFF;
        TxBuffer[6] = (unsigned char) (t4 >> 16) & 0xFF;
        TxBuffer[7] = (unsigned char) (t4 >> 8) & 0xFF;
        TxBuffer[8] = (unsigned char) t4 & 0xFF;
        TxBuffer[9] = (unsigned char) t5 & 0xFF;

//        printf("%d %d %d\r\n",TxBuffer[0],TxBuffer[1],TxBuffer[2]);
//		unsigned char* b1=(unsigned char*) &t1;
//		unsigned char* b2=(unsigned char*) &t2;
//		unsigned char* b3=(unsigned char*) &t3;
//		unsigned char* b4=(unsigned char*) &t4;
//		unsigned char* b5=(unsigned char*) &t5;
//        sprintf(TxBuffer2,"%u %u %u %lu %u",t1,t2,t3,t4,t5);
//        printf("%c %c %c %c %\r\n",b1,b2,b3,b4,b5);
//        printf("2 %d\r\n",sizeof(t1)+sizeof(t2)+sizeof(t4)+sizeof(t5));
//        printf("2 %d\r\n",sizeof(TxBuffer));
//        printf("tx %s\r\n",TxBuffer);
//        memset(TxBuffer,PACKET_LEN,buffer);
//        sprintf(TxBuffer,"%s",buffer);
//        printf("Txbuffer %x\r\n",TxBuffer);

//		  putstr(buffer);
//        return 0;
}

//void build_packet(void *payload, uint8_t payload_len)
//{
//
//	 (uint8_t *)payload, payload_len
//}

/*
 * ----------------------------------------------------------
 * Interrupt services
 * ----------------------------------------------------------
 */
#pragma vector=CC1101_VECTOR
__interrupt void CC1101_ISR(void)
{
  switch(__even_in_range(RF1AIV,32))        // Prioritizing Radio Core Interrupt
  {
    case  0: break;                         // No RF core interrupt pending
    case  2: break;                         // RFIFG0
    case  4: break;                         // RFIFG1
    case  6: break;                         // RFIFG2
    case  8: break;                         // RFIFG3
    case 10: break;                         // RFIFG4
    case 12: break;                         // RFIFG5
    case 14: break;                         // RFIFG6
    case 16: break;                         // RFIFG7
    case 18: break;                         // RFIFG8
    case 20:
    	// RFIFG9
      myreceive.recv_timestamp = get_localtime();
      if(receiving)             // RX end of packet
      {

        // Read the length byte from the FIFO
        RxBufferLength = ReadSingleReg( RXBYTES );
//        RxAddr =  ReadSingleReg(RXFIFO);
        ReadBurstReg(RF_RXFIFORD , RxBuffer, RxBufferLength);

        // Stop here to see contents of RxBuffer
        __no_operation();
//        printf("%d",RxBuffer);
        // Check the CRC results
        if(RxBuffer[CRC_LQI_IDX] & CRC_OK)
        {
        	rf_isr = 1;

        }
      }
      else if(transmitting)         // TX end of packet
      {
         RF1AIE &= ~BIT9;                    // Disable TX end-of-packet interrupt

      }
      else while(1);                // trap
      break;
    case 22: break;                         // RFIFG10
    case 24: break;                         // RFIFG11
    case 26: break;                         // RFIFG12
    case 28: break;                         // RFIFG13
    case 30: break;                         // RFIFG14
    case 32: break;                         // RFIFG15
  }
  __bic_SR_register_on_exit(LPM0_bits);
}



//#pragma vector=RTC_VECTOR
//__interrupt void RTC_ISR(void)
//{
//
//  switch(__even_in_range(RTCIV,16))
//  {
//    case RTC_NONE:                          // No interrupts
//      break;
//
//    case RTC_RTCRDYIFG:                     // RTCRDYIFG
////        TA0CCTL2 &= ~COV;
////        TA0CCTL2 &= ~CCIFG;
////        TA0CCTL2 |= CCIE;
////
//////		sprintf(clk_buffer,"%lu\r\n",get_localtime());
//////		putstr(clk_buffer);
////if(Node_ID == 1)
////{
////	switch (sendinterval){
////        case Packet_transmission_period:
////        	ReceiveOff();
////        	receiving = 0;
////        	transmitting = 1;
////
////
////        	if(sys_rf<1300)
////        	{
//////        		while(carriersense()!=1);
//////        		__delay_cycles(15000);
////        		construction(Node_ID,Packet_ID,sys_rf,get_localtime());
////        		Transmit( (unsigned char*)TxBuffer, sizeof TxBuffer);
////        	}
////            Packet_ID++;
////        	sendinterval = 0;
////        	break;
////        default:
////			sendinterval++;
//////			if(SYNC_head == 1)
//////			{
////
//////			switch(sync_interval){
//////				case SYNC_Period:
//////		        	ReceiveOff();
//////		        	receiving = 0;
//////		        	transmitting = 1;
////////					construction(sync_word,Packet_ID,sys_rf,Local_time);
//////					Transmit( (unsigned char*)TxBuffer, sizeof TxBuffer);
//////					P1OUT |= BIT7;
//////					sync_interval=0;
//////					break;
//////				default:
//////					sync_interval++;
//////					break;
//////			}
//////			}
////        }
////}
//    case RTC_RTCTEVIFG:                     // RTCEVIFG
//      __no_operation();                     // Interrupts every minute
//      break;
//    case RTC_RTCAIFG:                       // RTCAIFG
//      break;
//    case RTC_RT0PSIFG:                      // RT0PSIFG
//      break;
//    case RTC_RT1PSIFG:                      // RT1PSIFG
//      break;
//    case 12: break;                         // Reserved
//    case 14: break;                         // Reserved
//    case 16: break;                         // Reserved
//    default: break;
//  }
//  __bic_SR_register_on_exit(LPM0_bits);
//}

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
  switch(__even_in_range(P1IV, 16))
  {
    case  0: break;
    case  2: break;                         // P1.0 IFG
    case  4: break;                         // P1.1 IFG
    case  6: break;                         // P1.2 IFG
    case  8: break;                         // P1.3 IFG
    case 10:
    	P1IE = 0;                             // Debounce by disabling buttons
    	protoc = Protocol_Off;
        P3DIR &=~ BIT6+BIT7;
//			TA1CCTL0 = OUTMOD_4 + CCIE;                      // CCR0 toggle mode
//			TA1CCR0 = 32768-1;
//			TA1CTL = TASSEL_1 + ID_0 + MC_1 + TACLR;
    	break;                         // P1.4 IFG
    case 12:
    	P1IE = 0;                             // Debounce by disabling buttons
//    	SYNC_head = -SYNC_head;
//    	if(SYNC_head==1)
//    	{
//    		P3OUT |= BIT6;
//    	}
//    	else
//    	{
//    		P3OUT &= ~BIT6;
//    	}
    	break;                         // P1.5 IFG
    case 14:
    	//leave for future use
        P1IE = 0;                             // Debounce by disabling buttons
        protoc = Protocol_On;
        P3DIR|=BIT6+BIT7;
        P3OUT |= BIT7;
    	break;                         // P1.6 IFG
    case 16: break;                               // P1.7 IFG
  }
  __bic_SR_register_on_exit(LPM0_bits); // Exit active
}

//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//#pragma vector=ADC12_VECTOR
//__interrupt void ADC12ISR (void)
//#elif defined(__GNUC__)
//void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR (void)
//#else
//#error Compiler not supported!
//#endif
//{
//  switch(__even_in_range(ADC12IV,34))
//  {
//  case  0: break;                           // Vector  0:  No interrupt
//  case  2: break;                           // Vector  2:  ADC overflow
//  case  4: break;                           // Vector  4:  ADC timing overflow
//  case  6:                                  // Vector  6:  ADC12IFG0
//    in_temp = ADC12MEM0;                       // Move results, IFG is cleared
//    in_volt = ADC12MEM1;
//    __bic_SR_register_on_exit(LPM0_bits);   // Exit active CPU
//    break;
//  case  8: break;                           // Vector  8:  ADC12IFG1
//  case 10: break;                           // Vector 10:  ADC12IFG2
//  case 12: break;                           // Vector 12:  ADC12IFG3
//  case 14: break;                           // Vector 14:  ADC12IFG4
//  case 16: break;                           // Vector 16:  ADC12IFG5
//  case 18: break;                           // Vector 18:  ADC12IFG6
//  case 20: break;                           // Vector 20:  ADC12IFG7
//  case 22: break;                           // Vector 22:  ADC12IFG8
//  case 24: break;                           // Vector 24:  ADC12IFG9
//  case 26: break;                           // Vector 26:  ADC12IFG10
//  case 28: break;                           // Vector 28:  ADC12IFG11
//  case 30: break;                           // Vector 30:  ADC12IFG12
//  case 32: break;                           // Vector 32:  ADC12IFG13
//  case 34: break;                           // Vector 34:  ADC12IFG14
//  default: break;
//  }
//  __bic_SR_register_on_exit(LPM0_bits);   // Exit active CPU
//}


#pragma vector=TIMER0_A1_VECTOR
__interrupt void TimerA0(void)
{
	static unsigned int starttimeTA0;
	static int CapturesTA0 = 0;

   switch(__even_in_range(TA0IV,15))
   {
      case 4:
             switch(CapturesTA0){
             case 0:
                   starttimeTA0 = TA0CCR2;
                   CapturesTA0 = NCAPTURES1;
                   break;
             case 1:
                   sys_rf = TA0CCR2 - starttimeTA0;
                   CapturesTA0= 0;
                   ircf_isr = 1;
                   TA0CCTL2 &= ~CCIE;
                   TA1CCTL0|= CCIE;
//                   __bic_SR_register_on_exit(LPM0_bits); // Exit active
                   break;
             default:
                   CapturesTA0--;
                   break;
             }

      default:break;
   }

}

// Timer1 A0 interrupt service routine

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A0 (void)
#else
#error Compiler not supported!
#endif
{
	timer_count++;
//	Local_timevalue = Local_timevalue + period_assign;

	if(Node_ID == 1)
	{
		Local_timevalue = Local_timevalue + period_assign;
	}
	else
	{
		if(protoc == Protocol_On)
		{
//			Local_timevalue = Local_timevalue + period_assign;
			Local_timevalue = (unsigned long)((long)Local_timevalue  - (long)skew); //skew compensation
			Local_timevalue = (unsigned long)((long)Local_timevalue+ (unsigned long)period_assign); //skew compensation
		}
		else
		{
			Local_timevalue = Local_timevalue + period_assign;
		}
	}

//	printf("%ld\r\n",Local_timevalue);
	P1OUT ^= BIT7;
	timer_isr = 1;


	__bic_SR_register_on_exit(LPM0_bits); // Exit active
}
