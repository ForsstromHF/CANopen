/******************************************************************************
* $Header: can_send.c, 1, 31.07.2006					Mark Weber$
*******************************************************************************
*******************************************************************************
*
* (c) 2006 by BECK IPC GmbH
*
* BECK IPC GmbH
* Grüninger Weg 24
* D-35415 Pohlheim
*
* Phone: (49)-6404-905-0
* Fax:   (49)-6404-905-502
*
*******************************************************************************
*
* Disclaimer: This program is an example and should be used as such.
*             If you wish to use this program or parts of it in your application,
*             you must validate the code yourself. BECK IPC GmbH can not be held
*             responsible for the correct functioning or coding of this example.
*
*******************************************************************************
*
* SC123 Test program
*
*******************************************************************************
*
* $Log:
*  1    IPC@CHIP  1.0         31.07.2006					Mark Weber$
* $
*
******************************************************************************/
/******************************************************************************
* comments:
* this Software was tested with an echoserver. At first you have to run
* can_rcv.exe for preparing the CAN Ports. The usage is self-explanatory.
* If an message is sent back by the echo server or an other CAN device
* it is shown on the screen.
* You can use this software to test your CAN connections on your development
* board. Specific settings can be changed in the sourcecode
******************************************************************************/
/******************************************************************************
* includes
******************************************************************************/

#include <clib.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

/******************************************************************************
* defines
******************************************************************************/

#define BAUD 1000

/******************************************************************************
* globals
******************************************************************************/

CAN_MSG send_msg;
CAN_PORT_INIT CANstruc;
CAN_RX_FILT filter;
int can_port;

/******************************************************************************
* prototypes
******************************************************************************/

void init_can_msg(void);

/******************************************************************************
* main()
******************************************************************************/

void main(int argc, char **argv)
{
   //local variables
	unsigned char data[8];
	int node,
       RTR,
       i,
       result,
       retval;
	char DLC;

	// check arguments, usage
   if ((argc < 4) || (argc > 5))
   {
   	printf("\nUsage:");
   	printf("\ncan_send <port> <nodeID> <RTR> <data>");
   	printf("\n\t<port> = port number (0/1)");
   	printf("\n\t<nodeID> = node ID (0..7ff)");
   	printf("\n\t<RTR> = RTR (0/1)");
	printf("\n\t<data> = data");
	printf("\nHojhoj");
      return;
   }

   //  process data
   can_port = atoi(argv[1]);
   if ((can_port > 1) || (can_port < 0))
   {
   	printf("\nillegal CAN port");
   	return;
   }

   sscanf(argv[2], "%04x", &node);
   if ((node > 0x7ff) || (node < 0))
   {
   	printf("\nillegal nodeID");
   	return;
   }

   RTR = atoi(argv[3]) & 1;
   DLC = argc - 4;

   //copy data to char[8]
   for(i = 0; i <= 7; i++)
   {
   	sscanf(argv[i+4], "%04x", &data[i]);
   }

   BIOS_Set_Focus( FOCUS_APPLICATION );

   // init msg struct
   init_can_msg();

   //check if port is already open
   result = CAN_Open_Port( can_port,&CANstruc );
   if (result == -4) printf("\nport %i is already open",can_port, result);
   if ((result) && (result != -4)) printf("\nerror open port: %d", result);

   send_msg.Id.Normal = node << 5;
   send_msg.Len_Ctrl = DLC & CAN_DLC_FIELD;
   if (RTR) send_msg.Len_Ctrl = CAN_RTR_FLAG;
   for (i = 0; i < DLC; i++) send_msg.Data[i] = data[i];
   printf("\nsending %d bytes from CAN%d to ID: %x with RTR: %d, 29bit: %d"
         , send_msg.Len_Ctrl & CAN_DLC_FIELD
         , can_port
         , node
         , (send_msg.Len_Ctrl & CAN_RTR_FLAG) >> 5
         , send_msg.Len_Ctrl & CAN_IDE_FLAG);
   printf("\ndata:");
   for(i = 0; i < (send_msg.Len_Ctrl & CAN_DLC_FIELD); i++)
   {
      printf(" %02x", send_msg.Data[i]);
   }
   printf("\n");
   printf("\nYtterligare changes");
   retval = CAN_Send ( can_port, 0, &send_msg );
   if (retval) printf("\nerror sending message through port %d: %d", can_port, retval);

   BIOS_Set_Focus( FOCUS_BOTH );


}

/******************************************************************************
* init_can_msg()
******************************************************************************/
void init_can_msg(void)
{
   //-------------------------------------------
	// disable filters (acceptance mask and code)
	filter.Id_Mask.Extended = 0xffff;
	filter.Id_Mask.Normal = 0xffff;
	filter.Data_Mask = 0xffff;
	filter.Id_Value.Extended = 0xffff;
	filter.Id_Value.Normal = 0xffff;
	filter.Data_Value = 0xffff;
	//-------------------------------------------
   //Fill CAN0 struct
  	CANstruc.fDisable_Rx = 0 ;
  	CANstruc.Rx_Q_Size = 10 ;
  	CANstruc.Tx_Q_Size[0] = 10 ;
  	CANstruc.Tx_Q_Size[1] = 10;
  	CANstruc.Tx_Q_Size[2] = 10 ;

  	CANstruc.Config.Baud =  BAUD ;
  	CANstruc.Config.Mode =  0 ;
  	CANstruc.Config.Mask =  0 ;
  	CANstruc.Config.Value = 0 ;
  	CANstruc.Config.Bit_Rate_Div = 0 ;
   //-------------------------------------------

   CAN_Rx_Filters ( can_port, 0, &filter);

}


