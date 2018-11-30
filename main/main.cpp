/*
 * main implementation: use this 'C++' sample to create your own application
 *
 */
#include <math.h>
#include "PathPlanning.h"
#include "Vehicle.h"
#include "mode_entry.h"
#include "can.h"
#include "pit.h"
#include "uart.h"
#include "derivative.h" /* include peripheral declarations */


#ifdef __cplusplus
extern "C" {
#endif

extern void xcptn_xmpl(void);

#ifdef __cplusplus
}
#endif

void peri_clock_gating(void);

Vehicle m_Vehicle_CA = Vehicle(0.02,1,0,1,3,5);
vuint8_t cnt;
bool TerminalSendFlag = false;

float temp;
__attribute__ ((section(".text")))
int main()
{
    xcptn_xmpl();	/* Configure and Enable Interrupts */
    peri_clock_gating();
    system160mhz();	/* sysclk=160MHz, dividers configured, mode trans*/
    // Flex CAN0
    FlexCAN0_Init();

    // Flex Lin1 Uart
    FlexLin1_Uart_Buffer_Init(80,115200);
//    FlexLin1_Uart_FIFO_Init(80,115200);


    // PIT0
    PIT_0.MCR.B.MDIS = 0; /* Enable PIT module. NOTE: PIT module must be       */
                          /* enabled BEFORE writing to it's registers.         */
                          /* Other cores will write to PIT registers so the    */
                          /* PIT is enabled here before starting other cores.  */
	PIT_0.MCR.B.FRZ = 1;  /* Freeze PIT timers in debug mode */
	PIT0_init(800000);
    /* timeout= 0.8M  PITclks x 4 sysclks/1 PITclk x 1 sec/160Msysck */
    /*        = 0.8M x 4 / 160M = 3.2/160 = 0.02 sec.  */
    PIT_0.MCR.B.FRZ = 0; //Unfreeze timers

    /* Loop forever */
	for(;;)
	{
		if(TerminalSendFlag)
		{
			m_Vehicle_CA.TerminalControlCommandSend();
			m_Vehicle_CA.TerminalControlSpeedSend();
			TerminalSendFlag = false;
		}
	}
}

void peri_clock_gating(void)
{
  MC_ME.RUN_PC[0].R = 0x00000000;  /* gate off clock for all RUN modes */
  MC_ME.RUN_PC[1].R = 0x000000FE;  /* config. peri clock for all RUN modes */

  MC_ME.PCTL79.B.RUN_CFG  = 0b001; //FlexCAN 0: select peripheral config RUN_PC[1]
  MC_ME.PCTL30.B.RUN_CFG  = 0b001; //PCTL30 is PIT0 Peripheral Control Registers for Panther
  MC_ME.PCTL91.B.RUN_CFG  = 0b001; //LINFlexD_1: Select peripheral config RUN_PC[1]. No LINFlex_D_2 on Panther
  MC_ME.PCTL146.B.RUN_CFG = 0b001; // DMAMUX_1:
}

#ifdef __cplusplus
extern "C" {
#endif
void PIT0_isr(void)
{
	cnt = (cnt + 1) % 5;
	m_Vehicle_CA.VehicleContorl();
	if(cnt == 0)
	{
		TerminalSendFlag = true;
	}
	PIT_0.TIMER[0].TFLG.R |= 1;  /* Clear interrupt flag. w1c */
}

void FlexCAN0_Isr(void)
{
	if(CAN_0.IFLAG1.B.BUF31TO8I & 0x000001)
	{

		m_Vehicle_CA.VehicleInformation(CAN_0.MB[8].ID.B.ID_STD,CAN_0.MB[8].DATA.B);
		m_Vehicle_CA.SteeringAngleControlStateMachine();
		m_Vehicle_CA.SteeringAngleControl(0.02);
		/* release the internal lock for all Rx MBs
		 * by reading the TIMER */
		uint32_t temp = CAN_0.TIMER.R;
		CAN_0.IFLAG1.R = 0x00000100;
	}
}

void FlexLin1_Uart_Isr(void)
{
	m_Vehicle_CA.TerminalControlCommandReceive(LINFlexD_1.BDRM.B.DATA4);
	LINFlexD_1.UARTSR.B.DRFRFE = 1;
}
#ifdef __cplusplus
}
#endif

