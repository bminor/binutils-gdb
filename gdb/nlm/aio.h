
/*----------------------------------------------------------------------------*
 *									      *
 *	NetWare 386 Developer's Asynchronous I/O (AIO) Runtime library        *
 *									      *
 *	This include file defines the constants and prototypes required	      *
 *	to use the AIO application interface.				      *
 *									      *
 *	(c) Copyright.	1991 Novell, Inc.   All rights reserved.	      *
 *									      *
 *----------------------------------------------------------------------------*/

typedef unsigned long DWORD;


/*----------------------------------------------------------------------------*
 *									      *
 *    Predefined hardware types for use with the AIOAcquirePort function.     *
 *									      *
 *----------------------------------------------------------------------------*/

#define AIO_COMX_TYPE		  (1)
#define AIO_ARTIC_TYPE		  (2)
#define AIO_WNIM_TYPE		  (3)


/*----------------------------------------------------------------------------*
 *									      *
 *    Function completion code status values.				      *
 *									      *
 *    Note that all error statuses are negative values.			      *
 *									      *
 *----------------------------------------------------------------------------*/

#define AIO_SUCCESS		  (0)
#define AIO_BAD_HANDLE		  (-1)
#define AIO_FAILURE		  (-2)
#define AIO_FUNC_NOT_SUPPORTED	  (-3)
#define AIO_INVALID_PARAMETER	  (-5)
#define AIO_PORT_NOT_AVAILABLE	  (-6)
#define AIO_QUALIFIED_SUCCESS	  (-7)
#define AIO_NO_MORE_PORTS	  (-8)
#define AIO_TYPE_NUMBER_INVALID	  (-10)
#define AIO_BOARD_NUMBER_INVALID  (-11)
#define AIO_PORT_NUMBER_INVALID	  (-12)
#define AIO_RESOURCE_TAG_INVALID  (-13)
#define AIO_DATA_PRESENT	  (-14)
#define AIO_BAD_REQUEST_TYPE	  (-15)
#define AIO_PORT_GONE		  (-20)
#define AIO_RTAG_INVALID	  (-21)


/* This is only for non-CLIB application */

#define ASYNCIOSignature  0x4E595341   /* 'NYSA' */

/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with the AIOSetExternalControl function.	      *
 *									      *
 *----------------------------------------------------------------------------*/

#define AIO_EXTERNAL_CONTROL	  (1)  
#define AIO_EXTCTRL_DTR		    (1)
#define AIO_EXTCTRL_RTS		    (2)


#define AIO_BREAK_CONTROL	  (2)
#define AIO_SET_BREAK_OFF	    (0)
#define AIO_SET_BREAK_ON	    (1)


#define AIO_FLOW_CONTROL	  (3)
#define AIO_SOFTWARE_FLOW_CONTROL_OFF  (0)
#define AIO_SOFTWARE_FLOW_CONTROL_ON   (1)
#define AIO_HARDWARE_FLOW_CONTROL_OFF  (0)
#define AIO_HARDWARE_FLOW_CONTROL_ON   (2)


#define AIO_FLOW_CONTROL_CHARACTERS (4)	 

#define AIO_SET_DEADMAN_TIMER	  (5)  

/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with the AIOGetExternalStatus function.	      *
 *									      *
 *----------------------------------------------------------------------------*/

#define AIO_EXTSTA_RI	      (0x00000001)
#define AIO_EXTSTA_DCD	      (0x00000008)
#define AIO_EXTSTA_DSR	      (0x00000010)
#define AIO_EXTSTA_CTS	      (0x00000020)
#define AIO_EXTSTA_BREAK      (0x00000080)


/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with the AIOFlushBuffers function.		      *
 *									      *
 *----------------------------------------------------------------------------*/

#define AIO_FLUSH_WRITE_BUFFER	  (0x0001)
#define AIO_FLUSH_READ_BUFFER	  (0x0002)


/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with the AIOReadStatus function.		      *
 *									      *
 *----------------------------------------------------------------------------*/

#define AIO_RECEIVE_ACTIVE   (0)
#define AIO_RECEIVE_FULL     (1)

/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with the AIOWriteStatus function.		      *
 *									      *
 *----------------------------------------------------------------------------*/

#define AIO_TRANSMIT_IDLE     (0)
#define AIO_TRANSMIT_ACTIVE   (1)
#define AIO_TRANSMIT_XOFFED   (2)

/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with the AIOAcquirePort function.		      *
 *									      *
 *----------------------------------------------------------------------------*/

#define AIO_HARDWARE_TYPE_WILDCARD  (-1)
#define AIO_BOARD_NUMBER_WILDCARD   (-1)
#define AIO_PORT_NUMBER_WILDCARD    (-1)

/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with the AIOConfigurePort function.		      *
 *									      *
 *----------------------------------------------------------------------------*/

/* BitRate */
			  
#define AIO_BAUD_50	  (0)
#define AIO_BAUD_75	  (1)
#define AIO_BAUD_110	  (2)
#define AIO_BAUD_134p5	  (3)
#define AIO_BAUD_150	  (4)
#define AIO_BAUD_300	  (5)
#define AIO_BAUD_600	  (6)
#define AIO_BAUD_1200	  (7)
#define AIO_BAUD_1800	  (8)
#define AIO_BAUD_2000	  (9)
#define AIO_BAUD_2400	  (10)
#define AIO_BAUD_3600	  (11)
#define AIO_BAUD_4800	  (12)
#define AIO_BAUD_7200	  (13)
#define AIO_BAUD_9600	  (14)
#define AIO_BAUD_19200	  (15)
#define AIO_BAUD_38400	  (16)
#define AIO_BAUD_57600	  (17)
#define AIO_BAUD_115200	  (18)
			  
/* DataBits */

#define AIO_DATA_BITS_5	  (0)
#define AIO_DATA_BITS_6	  (1)
#define AIO_DATA_BITS_7	  (2)
#define AIO_DATA_BITS_8	  (3)

/* StopBits */
			     
#define AIO_STOP_BITS_1	  (0)
#define AIO_STOP_BITS_1p5 (1)
#define AIO_STOP_BITS_2	  (2)
			    
/* Parity */

#define AIO_PARITY_NONE	  (0)
#define AIO_PARITY_ODD	  (1)
#define AIO_PARITY_EVEN	  (2)
#define AIO_PARITY_MARK	  (3)
#define AIO_PARITY_SPACE  (4)

/* FlowControl */
				      
#define AIO_SOFTWARE_FLOWCONTROL_OFF  (0)
#define AIO_SOFTWARE_FLOWCONTROL_ON   (1)
#define AIO_HARDWARE_FLOWCONTROL_OFF  (0)
#define AIO_HARDWARE_FLOWCONTROL_ON   (2)

#define AIO_DROPOUT_VALUE   (0xFF)

/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with AIOPORTCAPABILITIES structure.		      *
 *									      *
 *----------------------------------------------------------------------------*/
			      
typedef struct
  {			      
  WORD	  returnLength;	      /*  byte length of capabilities data */
  BYTE	  majorVersion;
  BYTE	  minorVersion;
  LONG	  notSupportedMask;
  BYTE	  minBitRate;	      /*  minimum bit rate index supported */
  BYTE	  maxBitRate;	      /*  maximum bit rate index supported */
  BYTE	  minDataBits;	      /*  minimum data bits per char index supported */
  BYTE	  maxDataBits;	      /*  maximum data bits per char index supported */
  BYTE	  minStopBits;	      /*  minimum stop bits per char index supported */
  BYTE	  maxStopBits;	      /*  maximum stop bits per char index supported */
  BYTE	  minParityMode;      /*  minimum parity mode index supported */
  BYTE	  maxParityMode;      /*  maximum parity mode index supported */
  BYTE	  minFlowCtrlMode;    /*  minimum flow control mode index supported */
  BYTE	  maxFlowCtrlMode;    /*  maximum flow control mode index supported */
  LONG	  miscCapabilities;   /*  miscellaneous capability flags */
  LONG	  minReadBufferSize;  /*  minimum length of receive buffer */
  LONG	  maxReadBufferSize;  /*  maximum length of receive buffer */
  LONG	  minWriteBufferSize; /*  minimum length of transmit buffer */
  LONG	  maxWriteBufferSize; /*  maximum length of transmit buffer */
  WORD	  minDeadmanTime;     /*  minimum deadman time (seconds) */
  WORD	  maxDeadmanTime;     /*  maximum deadman time (seconds) */
  } AIOPORTCAPABILITIES;

#define AIO_PORT_NS_MINBITRATE		 (0x80000000)
#define AIO_PORT_NS_MAXBITRATE		 (0x40000000)
#define AIO_PORT_NS_MINDATABITS		 (0x20000000)
#define AIO_PORT_NS_MAXDATABITS		 (0x10000000)
#define AIO_PORT_NS_MINSTOPBITS		 (0x08000000)
#define AIO_PORT_NS_MAXSTOPBITS		 (0x04000000)
#define AIO_PORT_NS_MINPARITYMODE	 (0x02000000)
#define AIO_PORT_NS_MAXPARITYMODE	 (0x01000000)
#define AIO_PORT_NS_MINFLOWCTRLMODE	 (0x00800000)
#define AIO_PORT_NS_MAXFLOWCTRLMODE	 (0x00400000)
#define AIO_PORT_NS_MISCCAPABILITIES	 (0x00200000)
#define AIO_PORT_NS_MINREADBUFFERSIZE	 (0x00100000)
#define AIO_PORT_NS_MAXREADBUFFERSIZE	 (0x00080000)
#define AIO_PORT_NS_MINWRITEBUFFERSIZE	 (0x00040000)
#define AIO_PORT_NS_MAXWRITEBUFFERSIZE	 (0x00020000)
#define AIO_PORT_NS_MINDEADMANTIME	 (0x00010000)
#define AIO_PORT_NS_MAXDEADMANTIME	 (0x00008000) 

#define AIO_PORT_CAPS_NOT_SUPPORTED (0x00007FFF)
#define AIO_PORT_CAPS_MAJOR_VERSION  (1)
#define AIO_PORT_CAPS_MINOR_VERSION  (0)

#define AIO_CAP_OUTPUT_BREAK	(0x00000002)
#define AIO_CAP_FLOWCTRLCHARS	(0x00000004)
#define AIO_CAP_PROGRAMMABLE	(0x00000008)
#define AIO_CAP_INPUT		(0x00000010)
#define AIO_CAP_OUTPUT		(0x00000020)


typedef struct 
  {
  WORD	  returnLength;	      /*  byte length of driver capabilities structure */
  BYTE	  byteData[2];
  } AIODVRCAPABILITIES;

#define AIO_NO_STRUCT_DATA_RETURNED   (2)

/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with the AIOGetPortsRollCall function.	      *
 *									      *
 *----------------------------------------------------------------------------*/

typedef struct
  {
  WORD	  returnLength;	      /*  byte length of port info data */
  BYTE	  majorVersion;
  BYTE	  minorVersion;
  LONG	  notSupportedMask;
  int	  hardwareType;	      /*  value used with AIOAcquirePort   */  
  int	  boardNumber;	      /*    " 				   */
  int	  portNumber;	      /*    "				   */
  WORD	  availability;	      /*  availability of port for acquire */
  LONG	  externalStatus;     /*  current external status value for port */
  LONG	  chgdExternalStatus; /*  changed external status value for port  */
  } AIOPORTINFO;

#define AIO_INFO_NS_HARDWARETYPE	 (0x80000000)
#define AIO_INFO_NS_BOARDNUMBER		 (0x40000000)
#define AIO_INFO_NS_PORTNUMBER		 (0x20000000)
#define AIO_INFO_NS_AVAILABILITY	 (0x10000000)
#define AIO_INFO_NS_EXTERNALSTATUS	 (0x08000000)
#define AIO_INFO_NS_CHGDEXTERNALSTATUS	 (0x04000000)

#define AIO_PORT_INFO_NOT_SUPPORTED (0x03FFFFFF)
#define AIO_PORT_INFO_MAJOR_VERSION   (1)
#define AIO_PORT_INFO_MINOR_VERSION   (0)

#define AIO_AVAILABLE_FOR_ACQUIRE   (0)
#define AIO_ALREADY_ACQUIRED	    (1)
#define AIO_UNAVAILABLE		    (0xFF)

#define AIO_INITIAL	  (0)
#define AIO_SUCCESSOR	  (1)	 


/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with the AIOGetPortConfiguration function.	      *
 *									      *
 *----------------------------------------------------------------------------*/

typedef struct
  {
  WORD	  returnLength;	      /*  byte length of port configuration data */
  BYTE	  majorVersion;
  BYTE	  minorVersion;
  LONG	  notSupportedMask;
  int	  hardwareType;	      /*  value used with AIOAcquirePort     */
  int	  boardNumber;	      /*    "  */
  int	  portNumber;	      /*    " */
  BYTE	  bitRate;	      /*  Bits per second index */
  BYTE	  dataBits;	      /*  Bits per character index */
  BYTE	  stopBits;	      /*  Stop bits per char index */
  BYTE	  parityMode;	      /*  Generated parity index */
  BYTE	  flowCtrlMode;	      /*  Flow control mode */
  BYTE	  breakMode;	      /*  Break control mode */
  LONG	  readSize;	      /*  Receive buffer size */
  LONG	  writeSize;	      /*  Transmit buffer size */
  BYTE	  transmitXon;	 
  BYTE	  transmitXoff;	 
  BYTE	  receiveXon;	 
  BYTE	  receiveXoff;
  WORD	  externalControl;    /*  set with AIO_EXTERNAL_CONTROL */

  } AIOPORTCONFIG;

#define AIO_CONFIG_NS_HARDWARETYPE	   (0x80000000)
#define AIO_CONFIG_NS_BOARDNUMBER	   (0x40000000)
#define AIO_CONFIG_NS_PORTNUMBER	   (0x20000000)
#define AIO_CONFIG_NS_BITRATE		   (0x10000000)
#define AIO_CONFIG_NS_DATABITS		   (0x08000000)
#define AIO_CONFIG_NS_STOPBITS		   (0x04000000)
#define AIO_CONFIG_NS_PARITYMODE	   (0x02000000)
#define AIO_CONFIG_NS_FLOWCTRLMODE	   (0x01000000)
#define AIO_CONFIG_NS_BREAKMODE		   (0x00800000)
#define AIO_CONFIG_NS_READSIZE		   (0x00400000)
#define AIO_CONFIG_NS_WRITESIZE		   (0x00200000)
#define AIO_CONFIG_NS_TRANSMITXON	   (0x00100000)
#define AIO_CONFIG_NS_TRANSMITXOFF	   (0x00080000)
#define AIO_CONFIG_NS_RECEIVEXON	   (0x00040000)
#define AIO_CONFIG_NS_RECEIVEXOFF	   (0x00020000)
#define AIO_CONFIG_NS_EXTERNALCONTROL	   (0x00010000)

#define AIO_PORT_CONFIG_NOT_SUPPORTED (0x0007FFFF)
#define AIO_PORT_CONFIG_MAJOR_VERSION	(1)
#define AIO_PORT_CONFIG_MINOR_VERSION	(0)

#define AIO_EXTCTRL_DTR_ENABLE		(1)
#define AIO_EXTCTRL_DTR_DISABLE		(0)
#define AIO_EXTCTRL_RTS_ENABLE		(2)
#define AIO_EXTCTRL_RTS_DISABLE		(0)

#define AIO_BREAK_MODE_OFF	     (0)
#define AIO_BREAK_MODE_ON	     (1)

typedef struct 
  {
  WORD	  returnLength;	      /*  byte length of driver config structure */
  BYTE	  byteData[2];
  } AIODVRCONFIG;

/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with the AIOGetStatistics function.		      *
 *									      *
 *----------------------------------------------------------------------------*/

typedef struct 
  {
  WORD	  returnLength;	      /*  byte length of port statistics structure */
  BYTE	  majorVersion;
  BYTE	  minorVersion;
  LONG	  notSupportedMask;
  LONG	  receiveBytes;	      /*  total number of bytes received on port */
  LONG	  transmitBytes;      /*  total number of bytes transmitted from port */
  LONG	  parityErrors;	      /*  number of receive parity errors  */
  LONG	  framingErrors;      /*  number of receive framing errors */
  LONG	  overrunSoftware;    /*  number of software overruns (occurrences) */
  LONG	  overrunHardware;    /*  number of hardware overruns (occurrences) */
  } AIOPORTSTATISTICS;

#define AIO_STATS_NS_RECEIVEBYTES	  (0x80000000)
#define AIO_STATS_NS_TRANSMITBYTES	  (0x40000000)
#define AIO_STATS_NS_PARITYERRORS	  (0x20000000)
#define AIO_STATS_NS_FRAMINGERRORS	  (0x10000000)
#define AIO_STATS_NS_OVERRUNSOFTWARE	  (0x08000000)
#define AIO_STATS_NS_OVERRUNHARDWARE	  (0x04000000)

#define AIO_PORT_STATS_NOT_SUPPORTED  (0x03FFFFFF)
#define AIO_PORT_STATS_MAJOR_VERSION  (1)
#define AIO_PORT_STATS_MINOR_VERSION  (0)

typedef struct 
  {
  WORD	  returnLength;	      /*  byte length of driver statistics structure */
  BYTE	  byteData[2];
  } AIODVRSTATISTICS;

/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with AIOGetDriverList function.		      *
 *									      *
 *----------------------------------------------------------------------------*/

typedef struct
  {
  int	  hardwareType;
  int	  ports;
  char	  name[128];
  } AIODRIVERLISTENTRY;

typedef struct
  {
  WORD		      returnLength;
  AIODRIVERLISTENTRY  driver[1];
  } AIODRIVERLIST;

#define AIO_DRIVER_LIST_GET_FIRST   (-1)


/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with AIOGetBoardList function.		      *
 *									      *
 *----------------------------------------------------------------------------*/

typedef struct
  {
  int	  boardNumber;
  int	  ports;
  char	  name[128];
  } AIOBOARDLISTENTRY;

typedef struct
  {
  WORD		     returnLength;
  AIOBOARDLISTENTRY  board[1];
  } AIOBOARDLIST;

#define AIO_BOARD_LIST_GET_FIRST    (-1)

/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with AIOSetControlData function.		      *
 *									      *
 *----------------------------------------------------------------------------*/

typedef struct 
  {
  WORD	  returnLength;	      /*  byte length of control data structure */
  BYTE	  byteData[2];
  } AIOCONTROLDATA;

/*----------------------------------------------------------------------------*
 *									      *
 *    Definitions for use with AIOGetFirstPortInfo and AIOGetNextPortInfo     *
 *									      *
 *----------------------------------------------------------------------------*/

typedef struct
  {
  int	  typeMask;
  int	  boardMask;
  int	  portMask;
  int	  reserved[6];
  } AIOPORTSEARCH;

	/* reserved index values */

#define SEARCH_TYPE_INDEX     0
#define SEARCH_BOARD_INDEX    1
#define SEARCH_PORT_INDEX     2


/*----------------------------------------------------------------------------*
 *									      *
 *    Definition of AIO functions.					      *
 *									      *
 *----------------------------------------------------------------------------*/

extern	int  AIOAcquirePort	      ( int   *hardwareType,
					int   *boardNumber,
					int   *portNumber,
					int   *portHandle
				      );

extern	int  AIOAcquirePortWithRTag   ( int   *hardwareType,
					int   *boardNumber,	  
					int   *portNumber,	  
					int   *portHandle,	  
					LONG   RTag
				      );

extern	int  AIOConfigurePort	      ( int   portHandle,
					BYTE  bitRate,
					BYTE  dataBits,
					BYTE  stopBits,
					BYTE  parityMode,
					BYTE  flowCtrlMode
				      );
	
extern	int  AIOFlushBuffers	      ( int   portHandle,
					WORD  flushFlag
				      );
	
extern int AIOGetBoardList( int		    hardwareType,
			    int		    boardIndex,
			    AIOBOARDLIST   *pBoardList );

extern	int  AIOGetDriverList	      ( int   lastHardwareType,
					AIODRIVERLIST *pDriverList
				      );
       
extern	int  AIOGetExternalStatus     ( int   portHandle,
					LONG  *extStatus,
					LONG  *chgdExtStatus
				      );

extern int   AIOGetFirstPortInfo      ( int		    hardwareType,
					int		    boardNumber,    
					int		    portNumber,	   
					AIOPORTSEARCH	    *portSearchP,    
					AIOPORTINFO	    *portInfoP,	   
					AIOPORTCAPABILITIES *capabilitiesP,   
					AIODVRCAPABILITIES  *dvrCapabilitiesP,
					char		    *NLMModuleNameP );

extern int   AIOGetNextPortInfo	      ( AIOPORTSEARCH	    *portSearchP,	
					AIOPORTINFO	    *portInfoP,	      
					AIOPORTCAPABILITIES *capabilitiesP,   
					AIODVRCAPABILITIES  *dvrCapabilitiesP,
					char		    *NLMModuleNameP );
       
extern	int  AIOGetPortCapability     ( int		    portHandle,
					AIOPORTCAPABILITIES *pCapabilities,
					AIODVRCAPABILITIES  *pDvrCapabilities
				      );

extern	int  AIOGetPortConfiguration  ( int		    portHandle,
					AIOPORTCONFIG	    *pPortConfig,
					AIODVRCONFIG	    *pDvrConfig
				      );

extern	int  AIOGetPortStatus	      ( int   portHandle,
					LONG  *writeCount,
					WORD  *writeState,
					LONG  *readCount,
					WORD  *readState,
					LONG  *extStatus,
					LONG  *chgdExtStatus
				      );
	
extern	int  AIOGetReadBufferSize     ( int   portHandle,
					LONG  *readSize
				      );
	
extern	int  AIOGetPortStatistics     ( int   portHandle,
					AIOPORTSTATISTICS   *pPortStatistics,
					AIODVRSTATISTICS    *pDvrStatistics
				      );

extern int   AIOGetFirstPortStatistics( int		    hardwareType,
					int		    boardNumber,    
					int		    portNumber,	   
					AIOPORTSEARCH	    *portSearchP,    
					AIOPORTSTATISTICS   *pPortStatistics,
					AIODVRSTATISTICS    *pDvrStatistics
				      );

extern int   AIOGetNextPortStatistics ( AIOPORTSEARCH	    *portSearchP,	
					AIOPORTSTATISTICS   *pPortStatistics,
					AIODVRSTATISTICS    *pDvrStatistics
				      );

extern	int  AIOGetWriteBufferSize    ( int   portHandle,
					LONG  *writeSize
				      );
	
extern	int  AIOReadData	      ( int   portHandle,
					char  *buffer,
					LONG  length,
					LONG  *numberBytesRead
				      );
	
extern	int  AIOReadStatus	      ( int   portHandle, 
					LONG  *count,
					WORD  *state
				      );
	
extern	int  AIOReleasePort	      ( int   portHandle );
	
extern	int  AIOSetControlData	      ( int   portHandle,
					int   requestType,
					AIOCONTROLDATA *requestStructValue
				      );

extern	int  AIOSetExternalControl    ( int   portHandle,
					int   requestType,
					int   requestValue
				      );
	
extern	int  AIOSetFlowControl	      ( int   portHandle,
					int   flowCtrlMode
				      );
	
extern	int  AIOSetFlowControlCharacters( int	portHandle,
					BYTE  transmitXon,
					BYTE  transmitXoff,
					BYTE  receiveXon,
					BYTE  receiveXoff
				      );
	
extern	int  AIOSetReadBufferSize     ( int    portHandle,
					LONG   bufferSize
				      );
      
extern	int  AIOSetWriteBufferSize    ( int    portHandle,
					LONG   bufferSize
				      );

extern	int  AIOWriteData	      ( int   portHandle,
					char  *buffer,
					LONG  length,
					LONG  *numberBytesWritten
				      );
	
extern	int  AIOWriteStatus	      ( int   portHandle,
					LONG  *count,
					WORD  *state
				      );
 
