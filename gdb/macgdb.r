/* Resource file for MacGDB. */

#include "SysTypes.r"
#include "Types.r"

#include "mac-defs.h"

resource 'vers' (1) {
	0x02, 0x00, release, 0x00,
	verUS,
	"4.12.3",
	"4.12.3, Copyright \251 1994 Free Software Foundation, Inc."
};

resource 'MBAR' (128) {
	{ mApple, mFile, mEdit, mDebug };
};

resource 'MENU' (mApple, preload) {
	mApple,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	apple,
	{
		"About...", noIcon, noKey, noMark, plain,
		"-", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (mFile, preload) {
	mFile,
	textMenuProc,
	allEnabled,
	enabled,
	"File",
	{
		"New", noIcon, noKey, noMark, plain,
		"Open...", noIcon, "O", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Quit", noIcon, "Q", noMark, plain
	}
};

resource 'MENU' (mEdit, preload) {
	mEdit,
	textMenuProc,
	allEnabled,
	enabled,
	"Edit",
	{
		"Undo", noIcon, "Z", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Cut", noIcon, "X", noMark, plain,
		"Copy", noIcon, "C", noMark, plain,
		"Paste", noIcon, "V", noMark, plain,
		"Clear", noIcon, noKey, noMark, plain
	}
};

resource 'MENU' (mDebug, preload) {
	mDebug,
	textMenuProc,
	allEnabled,
	enabled,
	"Debug",
	{
		"Target", noIcon, "T", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Run", noIcon, "R", noMark, plain,
		"Continue", noIcon, "K", noMark, plain,
		"Step", noIcon, "S", noMark, plain,
		"Next", noIcon, "N", noMark, plain
	}
};

resource 'ALRT' (128) {
	{40, 40, 229, 363},
	128,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	}
};

resource 'DITL' (128) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{164, 115, 184, 185},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{5, 10, 43, 290},
		StaticText {
			disabled,
			"MacGDB"
		},
		/* [3] */
		{76, 3, 154, 301},
		StaticText {
			disabled,
			"GNU Debugger"
			"\nCopyright ©  1994 Free Software Foundation Inc.\n"
			"Written by Stan Shebs."
		},
		/* [4] */
		{53, 82, 71, 227},
		StaticText {
			disabled,
			"version 4.12.1"
		}
	}
};

resource 'WIND' (wConsole, preload, purgeable) {
	{40, 40, 310, 572},
	zoomDocProc,
	visible,
	goAway,
	0x0,
	"GDB Console"
};

resource 'SIZE' (-1) {
	reserved,
	acceptSuspendResumeEvents,
	reserved,
	canBackground,
	multiFinderAware,
	backgroundAndForeground,
	dontGetFrontClicks,
	ignoreChildDiedEvents,
	not32BitCompatible,
	isHighLevelEventAware,
	localAndRemoteHLEvents,
	notStationeryAware,
	dontUseTextEditServices,
	reserved,
	reserved,
	reserved,
	4000*1024,
	4000*1024
};

resource 'DLOG' (128) {
	{40, 40, 240, 280},
	documentProc,
	visible,
	goAway,
	0x0,
	128,
	""
};
