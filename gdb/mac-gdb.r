/* OBSOLETE /* Resource file for MacGDB. */ */
/* OBSOLETE  */
/* OBSOLETE #include "SysTypes.r" */
/* OBSOLETE #include "Types.r" */
/* OBSOLETE  */
/* OBSOLETE #include "mac-defs.h" */
/* OBSOLETE  */
/* OBSOLETE /* Version resources. */ */
/* OBSOLETE  */
/* OBSOLETE resource 'vers' (1) { */
/* OBSOLETE 	0, */
/* OBSOLETE 	0, */
/* OBSOLETE 	0, */
/* OBSOLETE 	0, */
/* OBSOLETE 	verUS, */
/* OBSOLETE 	VERSION_STRING, */
/* OBSOLETE 	VERSION_STRING  " (C) 1986-95 FSF, Inc." */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE resource 'vers' (2, purgeable)  { */
/* OBSOLETE 	0, */
/* OBSOLETE 	0, */
/* OBSOLETE 	0, */
/* OBSOLETE 	0, */
/* OBSOLETE 	verUs, */
/* OBSOLETE 	VERSION_STRING, */
/* OBSOLETE 	"GDB " VERSION_STRING " for MPW" */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE #ifdef WANT_CFRG */
/* OBSOLETE  */
/* OBSOLETE #include "CodeFragmentTypes.r" */
/* OBSOLETE  */
/* OBSOLETE resource 'cfrg' (0) { */
/* OBSOLETE 	{ */
/* OBSOLETE 		kPowerPC, */
/* OBSOLETE 		kFullLib, */
/* OBSOLETE 		kNoVersionNum, kNoVersionNum, */
/* OBSOLETE 		0, 0, */
/* OBSOLETE 		kIsApp, kOnDiskFlat, kZeroOffset, kWholeFork, */
/* OBSOLETE 		PROG_NAME */
/* OBSOLETE 	} */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE #endif /* WANT_CFRG */ */
/* OBSOLETE  */
/* OBSOLETE #ifdef Macgdb /* Exclude the following from SIOWgdb which uses SIOW.r.  */ */
/* OBSOLETE  */
/* OBSOLETE resource 'MBAR' (128) { */
/* OBSOLETE 	{ mApple, mFile, mEdit, mDebug }; */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE resource 'MENU' (mApple, preload) { */
/* OBSOLETE 	mApple, */
/* OBSOLETE 	textMenuProc, */
/* OBSOLETE 	0x7FFFFFFD, */
/* OBSOLETE 	enabled, */
/* OBSOLETE 	apple, */
/* OBSOLETE 	{ */
/* OBSOLETE 		"About...", noIcon, noKey, noMark, plain, */
/* OBSOLETE 		"-", noIcon, noKey, noMark, plain */
/* OBSOLETE 	} */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE resource 'MENU' (mFile, preload) { */
/* OBSOLETE 	mFile, */
/* OBSOLETE 	textMenuProc, */
/* OBSOLETE 	allEnabled, */
/* OBSOLETE 	enabled, */
/* OBSOLETE 	"File", */
/* OBSOLETE 	{ */
/* OBSOLETE 		"New", noIcon, noKey, noMark, plain, */
/* OBSOLETE 		"Open...", noIcon, "O", noMark, plain, */
/* OBSOLETE 		"-", noIcon, noKey, noMark, plain, */
/* OBSOLETE 		"Quit", noIcon, "Q", noMark, plain */
/* OBSOLETE 	} */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE resource 'MENU' (mEdit, preload) { */
/* OBSOLETE 	mEdit, */
/* OBSOLETE 	textMenuProc, */
/* OBSOLETE 	allEnabled, */
/* OBSOLETE 	enabled, */
/* OBSOLETE 	"Edit", */
/* OBSOLETE 	{ */
/* OBSOLETE 		"Undo", noIcon, "Z", noMark, plain, */
/* OBSOLETE 		"-", noIcon, noKey, noMark, plain, */
/* OBSOLETE 		"Cut", noIcon, "X", noMark, plain, */
/* OBSOLETE 		"Copy", noIcon, "C", noMark, plain, */
/* OBSOLETE 		"Paste", noIcon, "V", noMark, plain, */
/* OBSOLETE 		"Clear", noIcon, noKey, noMark, plain */
/* OBSOLETE 	} */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE resource 'MENU' (mDebug, preload) { */
/* OBSOLETE 	mDebug, */
/* OBSOLETE 	textMenuProc, */
/* OBSOLETE 	allEnabled, */
/* OBSOLETE 	enabled, */
/* OBSOLETE 	"Debug", */
/* OBSOLETE 	{ */
/* OBSOLETE 		"Target", noIcon, "T", noMark, plain, */
/* OBSOLETE 		"-", noIcon, noKey, noMark, plain, */
/* OBSOLETE 		"Run", noIcon, "R", noMark, plain, */
/* OBSOLETE 		"Continue", noIcon, "K", noMark, plain, */
/* OBSOLETE 		"Step", noIcon, "S", noMark, plain, */
/* OBSOLETE 		"Next", noIcon, "N", noMark, plain */
/* OBSOLETE 	} */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE resource 'ALRT' (128) { */
/* OBSOLETE 	{40, 40, 180, 420}, */
/* OBSOLETE 	128, */
/* OBSOLETE 	{	/* array: 4 elements */ */
/* OBSOLETE 		/* [1] */ */
/* OBSOLETE 		OK, visible, sound1, */
/* OBSOLETE 		/* [2] */ */
/* OBSOLETE 		OK, visible, sound1, */
/* OBSOLETE 		/* [3] */ */
/* OBSOLETE 		OK, visible, sound1, */
/* OBSOLETE 		/* [4] */ */
/* OBSOLETE 		OK, visible, sound1 */
/* OBSOLETE 	} */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE resource 'DITL' (128) { */
/* OBSOLETE 	{	/* array DITLarray: 2 elements */ */
/* OBSOLETE 		/* [1] */ */
/* OBSOLETE 		{110, 150, 128, 200}, */
/* OBSOLETE 		Button { */
/* OBSOLETE 			enabled, */
/* OBSOLETE 			"OK" */
/* OBSOLETE 		}, */
/* OBSOLETE 		/* [2] */ */
/* OBSOLETE 		{10, 10, 100, 370}, */
/* OBSOLETE 		StaticText { */
/* OBSOLETE 			disabled, */
/* OBSOLETE 			"GDB 4.12.3\n" */
/* OBSOLETE 			"Copyright ©  1994 Free Software Foundation, Inc.\n" */
/* OBSOLETE 		}, */
/* OBSOLETE 	} */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE resource 'WIND' (wConsole, preload, purgeable) { */
/* OBSOLETE 	{40, 40, 310, 572}, */
/* OBSOLETE 	zoomDocProc, */
/* OBSOLETE 	visible, */
/* OBSOLETE 	goAway, */
/* OBSOLETE 	0x0, */
/* OBSOLETE 	"GDB Console" */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE resource 'SIZE' (-1) { */
/* OBSOLETE 	reserved, */
/* OBSOLETE 	acceptSuspendResumeEvents, */
/* OBSOLETE 	reserved, */
/* OBSOLETE 	canBackground, */
/* OBSOLETE 	multiFinderAware, */
/* OBSOLETE 	backgroundAndForeground, */
/* OBSOLETE 	dontGetFrontClicks, */
/* OBSOLETE 	ignoreChildDiedEvents, */
/* OBSOLETE 	not32BitCompatible, */
/* OBSOLETE 	isHighLevelEventAware, */
/* OBSOLETE 	localAndRemoteHLEvents, */
/* OBSOLETE 	notStationeryAware, */
/* OBSOLETE 	dontUseTextEditServices, */
/* OBSOLETE 	reserved, */
/* OBSOLETE 	reserved, */
/* OBSOLETE 	reserved, */
/* OBSOLETE 	5000*1024, */
/* OBSOLETE 	2000*1024 */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE resource 'DLOG' (128) { */
/* OBSOLETE 	{40, 40, 240, 280}, */
/* OBSOLETE 	documentProc, */
/* OBSOLETE 	visible, */
/* OBSOLETE 	goAway, */
/* OBSOLETE 	0x0, */
/* OBSOLETE 	128, */
/* OBSOLETE 	"" */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE #endif */
