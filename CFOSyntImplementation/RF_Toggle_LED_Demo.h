#include "cc430x513x.h"
#include "RF1A.h"
#include "hal_pmm.h"
#include "config.h"
//#include "DCOAdjust.h"



/* wait until the radio core is ready to accept the next instruction */
#define WAIT_UNTIL_READY_FOR_INSTR() while(!(RF1AIFCTL1 & RFINSTRIFG))

/* wait until the radio core is ready to accept additional data */
#define WAIT_UNTIL_READY_FOR_DATA() while(!(RF1AIFCTL1 & RFDINIFG))
/* wait until data has been provided by the radio core */
#define WAIT_UNTIL_DATA_IS_READY() while(!(RF1AIFCTL1 & RFDOUTIFG))
/*******************
 * Function Definition
 */
void Transmit(unsigned char *buffer, unsigned char length);
void ReceiveOn(void);
void ReceiveOff(void);

void InitButtonLeds(void);
void InitRadio(void);
//void Transbutton(void);
//void Receibutton(void);
void RTCInit(void);
void IdentityInit(void);
void PortInit(void);
void UartInit(void);
void ADCInit(void);
unsigned char ClearCCA(void);
unsigned char carriersense(void);

void putchar1( char ch);
void putstr( char*str);
//static inline void write_data_to_register(uint8_t addr, uint8_t *buffer, uint16_t n_bytes);
//static inline uint8_t read_byte_from_register(uint8_t addr);

char receive_time[1024];
char receive_ircf[128];
unsigned int packet_int;
unsigned int icrf_int;
unsigned long time_int;
unsigned long time_last;
unsigned long time_to_send;
long offset;
float lr_result;
int packet_seq;





