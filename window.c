#include	<types.h>#include	<quickdraw.h>#include	<toolutils.h>#include	<fonts.h>#include	<events.h>#include	<windows.h>#include	<dialogs.h>#include	<menus.h>//#include	<desk.h>#include	<textedit.h>#include	<scrap.h>#include	<segload.h>#include	<resources.h>#include	<OSutils.h>#include	<controls.h>#define __WINDOW_C_#include "main.h"#include "window.h"#include "mmemory.h"#include "defines.h"#include "structs.h"#include "globals.h"#include "physmem.h"#include "hexdump.h"#include "decdump.h"#include "lmem.h"#include "regw.h"#include "sysw.h"#include "stackw.h"#include "iow.h"#include "progw.h"#include "controldef.h"#include "docmds.h"#include "settings.h"#include "aprintf.h"#pragma segment WINDOWSvoid setup_a_window(	short window_id,	short hscroll_id,	short vscroll_id,	short font_size,	void (*update_fn)(void),	void (*activate_window_fn)(long),	long top_margin_offset,	long growable,	void (*click_fn)(Point *, short),	void (*keypress_fn)(long),	void (*undo_keypress_fn)(void),	void (*doscroll_fn)(short,short),	void (*dogrow_fn)(void)){OurWindowPtr wp;FontInfo finfo;GrafPtr savePort;WindowRecord * wrecord_p;	GetPort(&savePort);	wp = window ;	wp += (window_id - progwID);	wrecord_p = wRecord_base_p + (window_id - progwID);		wp->windowptr = GetNewWindow( window_id, (void *)wrecord_p, (WindowPtr) -1);	if(wp->windowptr == 0)	{		aprintf("Unable to load window resource %d", window_id);		return;	}	wp->update_window   = update_fn;	wp->activate_window = activate_window_fn;	wp->click_window    = click_fn;	wp->keypress        = keypress_fn;	wp->undo_keypress   = undo_keypress_fn;	wp->doscroll        = doscroll_fn;	wp->dogrow          = dogrow_fn;	SetPort(wp->windowptr);	SetWRefCon(wp->windowptr, (long)window_id);	if(vscroll_id)		wp->vScroll = GetNewControl( vscroll_id, wp->windowptr); /*get a handle to a scroll bar control*/	if(hscroll_id)		wp->hScroll = GetNewControl(hscroll_id, wp->windowptr); /*get a handle to a scroll bar control*/	UsableRect(wp); 	MoveScrollBars(wp);	/*resize it to fit window*/	wp->fsize = font_size;	wp->grow = growable;	TextFont(monaco);	TextSize(wp->fsize);	TextMode(srcCopy);	TextFace(normal);	GetFontInfo(&finfo);	wp->leading = finfo.leading + 1;	wp->forigin = finfo.descent + wp->leading;	wp->lsize = finfo.ascent + wp->forigin;	wp->topMargin = wp->lsize * top_margin_offset;	wp->nlines = (wp->usable.bottom - wp->topMargin) / wp->lsize ; /*calculate num lines*/	wp->secure = 0;	/*do_activate(wp,0);*/	SetPort(savePort);}void setupwindows(void){	if( sScrollUpProc == nil )		sScrollUpProc = NewControlActionProc( ScrollUp );	if( sScrollDownProc == nil )		sScrollDownProc = NewControlActionProc( ScrollDown );	screenRect = qd.screenBits.bounds;	SETRECT(&dragRect, 4, 20 + 4, screenRect.right-4, screenRect.bottom-4);	updateRgn = NewRgn();	/*	 * create some of our windows from the WIND resource.	 * If the WIND resource isn't there, we die.	 */	init_finger();	ibeamHdl = GetCursor(iBeamCursor);		/* Grab this for use later */	setup_a_window(progwID,  257, 256, 9, update_prog_window,     activate_prog_window,     1,1, progw_click,    progw_keypress,    progw_undo_keypress,    progw_scroll, (void *)0L);	setup_a_window(regwID,     0, 261,   9, update_register_window, activate_register_window, 0,1, regw_click,     regw_keypress,     regw_undo_keypress,     (void *)0L,   fix_scrollbars_regw);	setup_a_window(dumphwID,   0, 264, 9, update_hexmem_window,   activate_hexmem_window,   1,1, dumphw_click,   dumphw_keypress,   dumphw_undo_keypress,   (void *)0L,   (void *)0L); 	setup_a_window(memwID,     0, 258, 9, update_lmem_window,     activate_lmem_window,     1,1, lmemw_click,    lmemw_keypress,    lmemw_undo_keypress,    (void *)0L,   (void *)0L);	setup_a_window(iowID,      0, 260, 9, update_IO_window,       activate_io_window,       0,1, iow_click,      iow_keypress,      iow_undo_keypress,      iow_scroll,   iow_grow);	setup_a_window(dumpdwID,   0, 262, 9, update_decmem_window,   activate_decmem_window,   1,1, dumpdw_click,   dumpdw_keypress,   dumpdw_undo_keypress,   (void *)0L,   (void *)0L);	setup_a_window(physmemwID, 0, 263, 9, update_physmem_window,  activate_physmem_window,  1,1, physmemw_click, physmemw_keypress, physmemw_undo_keypress, (void *)0L,   (void *)0L);	setup_a_window(syswID,     0, 265, 9, update_sys_window,      activate_system_window,   0,1, sysw_click,     sysw_keypress,     sysw_undo_keypress,     (void *)0L,   (void *)0L);	setup_a_window(stackwID, 267, 266, 9, update_stack_window,    activate_stack_window,   0,1, stackw_click,   stackw_keypress,   stackw_undo_keypress,   (void *)0L,   (void *)0L);	iow_init();	physmem_init();	progw_init();	regw_init();	fix_scrollbars_dec_hex();	set_maxscroll_sysw();	set_maxscroll_stackw(0);	restorestate(0);}void clear_all_windows(void){GrafPtr savePort;long i;	GetPort(&savePort);	for(i = 0; i < NumWindows; i++)	{		SetPort(window[i].windowptr);		InvalRect(&window[i].windowptr->portRect);		doupdate(&window[i], 1L);	}	SetPort(savePort);}void doupdate(	OurWindowPtr owp,	long erase){GrafPtr savePort;WindowPtr wp;	wp = owp->windowptr;	GetPort(&savePort);	SetPort(wp);	BeginUpdate(wp);		if(erase)			EraseRect(&wp->portRect);		MyDrawGrow(owp);		DrawControls(wp);		if(owp->update_window)			owp->update_window();	EndUpdate(wp);	SetPort(savePort);}void update_display(void){long i;	for(i = 0; i < NumWindows; i++)	{		if(window[i].update_window)			window[i].update_window();	}}void update_memdisplay(	long andreg){	if(memWindow.update_window)		memWindow.update_window();		if(physmemWindow.update_window)		physmemWindow.update_window();		if(update_sys_window)		update_sys_window();		if(stackWindow.update_window)	{			stackWindow.update_window();	}		if(hex_mem_dump == 1)	{		if(dumphWindow.update_window)			dumphWindow.update_window();	}	else	{		if(dumpdWindow.update_window)			dumpdWindow.update_window();	}	if(andreg)	{		if(regWindow.update_window)			regWindow.update_window();	}}void update_prog_reg_display(void){	if(progWindow.update_window)		progWindow.update_window();	if(regWindow.update_window)		regWindow.update_window();}void do_activate(	OurWindowPtr wp,	long is_active){		if( ((WindowPeek) (wp->windowptr))->visible != false )	{		if (is_active) 		{			SetPort(wp->windowptr);			theActiveWindow = wp->windowptr;			TextFont((wp->windowptr)->txFont);			TextSize(wp->fsize);			TextMode(srcCopy);			TextFace((wp->windowptr)->txFace);			if(wp->vScroll)				ShowControl(wp->vScroll);			if(wp->hScroll)				ShowControl(wp->hScroll);		} 		else 		{			if(wp->vScroll)				HideControl(wp->vScroll);			if(wp->hScroll && wp != &stackWindow && wp != &progWindow)				HideControl(wp->hScroll);		}		MyDrawGrow(wp);	}	if(wp->activate_window)		wp->activate_window(is_active);}void MyDrawGrow(	OurWindowPtr lWindow){GrafPtr savePort;Rect GrowRect;RgnHandle	hold_h;		if(lWindow->grow)	{		GetPort(&savePort);		SetPort(lWindow->windowptr);		GrowRect = (lWindow->windowptr)->portRect;		GrowRect.left = GrowRect.right - 14;		GrowRect.top = GrowRect.bottom - 14;		hold_h = NewRgn();		GetClip(hold_h);		ClipRect(&GrowRect);		DrawGrowIcon(lWindow->windowptr);		SetClip(hold_h);		DisposeRgn(hold_h);		if(lWindow->vScroll)		{			MoveTo(GrowRect.left - 1 ,0);			Line(0, GrowRect.bottom);		}			if(lWindow->hScroll)		{			MoveTo(0 , GrowRect.top - 1);			Line(GrowRect.right,0);		}		SetPort(savePort);	}}unsigned long sklines_dumpdwindow =  0;					/*Number of stack lines dec dump window*/unsigned long sklines_dumphwindow =  0;					/*Number of stack lines hex dump window*/unsigned long cdlines_dumpdwindow =  0;					/*Number of code lines dec dump window*/unsigned long cdlines_dumphwindow =  0;					/*Number of code lines hex dump window*/unsigned long syslines_dumpdwindow = 0;					/*Number of system lines dec dump window*/unsigned long syslines_dumphwindow = 0;					/*Number of system lines hex dump window*/address  last_lbase_d = 0;								/*address of first byte on last line of P0 space*/address  last_lbase_h = 0;								/*address of first byte on last line of P0 space*/address  last_sysbase_d = S_SPACE_C;					/*address of first byte on last line of sys space*/address  last_sysbase_h = S_SPACE_C;					/*address of first byte on last line of sys space*/address  first_stackaddress_h = S_SPACE_C;				/*address of first byte on last line of P1 Space*/address  first_stackaddress_d = S_SPACE_C;				/*address of first byte on last line of P1 Space*/void cdlines( void ){	if(IPR(MME) == 1)	{		cdlines_dumpdwindow = ( ( IPR(P0LR) << 9 ) + (unsigned long) 9 ) / (unsigned long)10 ;		last_lbase_d  = cdlines_dumpdwindow * (unsigned long)10;		cdlines_dumphwindow = ( ( IPR(P0LR) << 9 ) + (unsigned long)15 ) / (unsigned long)16;		last_lbase_h  = cdlines_dumphwindow * (unsigned long)16;	}	else	{		cdlines_dumpdwindow = ( REAL_MEM_SIZE + (unsigned long) 9 ) / (unsigned long)10 ;		last_lbase_d  = cdlines_dumpdwindow * (unsigned long)10;		cdlines_dumphwindow = ( REAL_MEM_SIZE + (unsigned long)15 ) / (unsigned long)16;		last_lbase_h  = cdlines_dumphwindow * (unsigned long)16;	}	fix_scrollbars_dec_hex();}void sklines( void ){	if(IPR(MME) == 1)	{		sklines_dumpdwindow = ( ( (TOTAL_P1_PAGES - IPR(P1LR) ) << 9) + (unsigned long) 9 ) / (unsigned long)10;		first_stackaddress_d = (unsigned long) S_SPACE - (sklines_dumpdwindow * (unsigned long) 10);		sklines_dumphwindow = ( ( (TOTAL_P1_PAGES - IPR(P1LR) ) << 9) + (unsigned long)15 ) / (unsigned long)16;		first_stackaddress_h = (unsigned long) S_SPACE - (sklines_dumphwindow * (unsigned long) 16);	}	else	{		sklines_dumpdwindow =  0;					/*Number of stack lines dec dump window*/		sklines_dumphwindow =  0;					/*Number of stack lines hex dump window*/		first_stackaddress_h = S_SPACE_C;			/*address of first byte on last line of P1 Space*/		first_stackaddress_d = S_SPACE_C;			/*address of first byte on last line of P1 Space*/	}	fix_scrollbars_dec_hex();}void syslines( void ){	if(IPR(MME) == 1)	{		syslines_dumpdwindow = ((IPR(SLR) << 9) + (unsigned long) 9) / (unsigned long)10;		last_sysbase_d  = (unsigned long) S_SPACE +  syslines_dumpdwindow * (unsigned long)10;		syslines_dumphwindow = ((IPR(SLR) << 9) + (unsigned long)15) / (unsigned long)16;		last_sysbase_h  = (unsigned long) S_SPACE + syslines_dumphwindow * (unsigned long)16;	}	else	{	/*Window display same as the Physical memory Window*/		syslines_dumpdwindow = 0;					/*Number of system lines dec dump window*/		syslines_dumphwindow = 0;					/*Number of system lines hex dump window*/		last_sysbase_d = S_SPACE_C;					/*address of first byte on last line of sys space*/		last_sysbase_h = S_SPACE_C;					/*address of first byte on last line of sys space*/	}		fix_scrollbars_dec_hex();}void fix_scrollbars_dec_hex(void){short nlines_h;short nlines_d;	nlines_d = cdlines_dumpdwindow + 1 + sklines_dumpdwindow + syslines_dumpdwindow;	nlines_h = cdlines_dumphwindow + 1 + sklines_dumphwindow + syslines_dumphwindow;	SetCtlMax(dumpdWindow.vScroll, nlines_d); 	SetCtlMax(dumphWindow.vScroll, nlines_h); 	if(dumphWindow.theOrigin.v > nlines_h)		dumphWindow.theOrigin.v = nlines_h;	if(dumpdWindow.theOrigin.v > nlines_d)		dumpdWindow.theOrigin.v = nlines_d;}void MoveScrollBars(	OurWindowPtr lWindow){register Rect *rp;	rp = &((lWindow->windowptr)->portRect);	if(lWindow->vScroll)	{		HideControl(lWindow->vScroll);		MoveControl(lWindow->vScroll, rp->right-15, rp->top-1);		SizeControl(lWindow->vScroll, 16, rp->bottom-rp->top-13);		ShowControl(lWindow->vScroll);	}	if(lWindow->hScroll)	{		HideControl(lWindow->hScroll);		MoveControl(lWindow->hScroll, rp->left-1, rp->bottom-15);		SizeControl(lWindow->hScroll, rp->right-rp->left-13, 16);		ShowControl(lWindow->hScroll);	}}void UsableRect(	OurWindowPtr lWindow){register Rect pRect;	pRect = (lWindow->windowptr)->portRect;	pRect.left = pRect.left + 4;	if(lWindow->vScroll)		pRect.right = pRect.right - 16;	if(lWindow->hScroll)		pRect.bottom = pRect.bottom - 16;	lWindow->usable = pRect;}void GrowWnd(	OurWindowPtr lWindow,	Point	where){long longResult;short height, width;Rect tRect;long cw;long lh;	if(lWindow->grow)	{		longResult = GrowWindow(lWindow->windowptr, where, &dragRect);		if (longResult == 0)			return;		cw = CharWidth('0');		lh = lWindow->lsize;		height = longResult >> 16;		width = longResult & WORD_MASK;		if(lWindow->vScroll)		{			tRect = (lWindow->windowptr)->portRect;			tRect.left = tRect.right - 16;			InvalRect(&tRect);							/*blat vertical scroll bars*/		}		if(lWindow->hScroll)		{			tRect = (lWindow->windowptr)->portRect;			if(tRect.bottom < height)				height = (((height + lh - 17) / lh) * lh) + 17;			else				height = (((height - 16) / lh) * lh) + 17;			tRect.top = tRect.bottom - 16;				InvalRect(&tRect);	/*blat Horizontal scroll bars*/		}		else		{				tRect = (lWindow->windowptr)->portRect;			if(tRect.bottom < height)				height = ((height + lh - 1) / lh) * lh + 1;			else				height = (height / lh) * lh + 1;		}					tRect = (lWindow->windowptr)->portRect;		tRect.left = tRect.right - 14;		tRect.top = tRect.bottom - 14;		InvalRect(&tRect);								/*blat old grow symbol*/		SizeWindow(lWindow->windowptr, width, height, TRUE); /*get system to redraw the window*/		MoveScrollBars(lWindow);							 /*resize the scroll bars to fit*/		UsableRect(lWindow);								/*calculate new usable area*/		lWindow->nlines = (lWindow->usable.bottom - lWindow->topMargin) / lh; /*calculate num lines*/		if(lWindow->vScroll)		{			tRect = (lWindow->windowptr)->portRect;	/*mark new positions of scroll bars as needing updating*/			tRect.left = tRect.right - 16;			InvalRect(&tRect);		}		if(lWindow->hScroll)		{			tRect = (lWindow->windowptr)->portRect;			tRect.top = tRect.bottom - 16;			InvalRect(&tRect);		}		tRect = (lWindow->windowptr)->portRect;		tRect.left = tRect.right - 14;		tRect.top = tRect.bottom - 14;		InvalRect(&tRect);		if(lWindow->dogrow)			lWindow->dogrow();	}}void ScrollBits(void){Point oldOrigin;short dh;long  dv;long windownum;long windowrefnum;Rect	srect;long line_depth;	windowrefnum = GetWRefCon(whichWindow);	windownum = windowrefnum - progwID;	/*find out which window we are dealing with*/	line_depth = window[windownum].lsize;	/*depth in pixels of a line*/		oldOrigin = window[windownum].theOrigin;	/*note previous position*/	if(windowrefnum != progwID && windowrefnum != stackwID && window[windownum].hScroll)	{		window[windownum].theOrigin.h = GetCtlValue(window[windownum].hScroll);	/*calculate a new position*/		dh = CharWidth('0') * (oldOrigin.h - window[windownum].theOrigin.h);	/*the thumb	 */	}	else 		dh = 0;	if(window[windownum].vScroll)	{		window[windownum].theOrigin.v = GetCtlValue(window[windownum].vScroll);	/*based on the position of */		dv = line_depth * (oldOrigin.v - window[windownum].theOrigin.v);	/*calc amount needed to shift to new spot*/	}	else		dv = 0;		srect =  window[windownum].usable;	if(dv)		srect.top = window[windownum].topMargin + 1;		if(dv > S_MAX_WORD)	/*catch large jumps*/		dv = S_MAX_WORD;	else if(dv < S_MIN_WORD)		dv = S_MIN_WORD;			if(window[windownum].doscroll)			window[windownum].doscroll(dh, dv);	else	{		ScrollRect(&srect, dh, dv, updateRgn);		InvalRgn(updateRgn);		doupdate(&window[windownum],1);	}}pascal voidScrollUp(	ControlHandle whichControl,	short theCode){	if(theCode == 0)		return;	if(theCode == inUpByte) /*special hack for prog window*/		up_a_byte = 1;	else if(theCode == inUpInstruction) /*special hack for prog window*/		up_an_instruction = 1;	SetCtlValue(whichControl, GetCtlValue(whichControl) - 1);	ScrollBits();}pascal voidScrollDown(	ControlHandle whichControl,	short theCode){	 	if(theCode == 0)		return;	if(theCode == inDownByte) /*special hack for prog window*/		down_a_byte = 1;	else if(theCode == inDownInstruction) /*special hack for prog window*/		down_an_instruction = 1;	SetCtlValue(whichControl, GetCtlValue(whichControl)+1);	ScrollBits();}void PageScroll(	short code,	short amount){Point myPt;	do {		GetMouse(&myPt);		if (TestControl(whichControl, myPt) == code) 		{			SetCtlValue(whichControl, GetCtlValue(whichControl)+amount);			ScrollBits();		}	} while (StillDown());}void PageScroll_highlight(	short code,	short amount){Point myPt;	do {		GetMouse(&myPt);		if (TestControl(whichControl, myPt) == code) 		{			HiliteControl(whichControl,code);			SetCtlValue(whichControl, GetCtlValue(whichControl) + amount);			ScrollBits();		}		else			HiliteControl(whichControl,0);	} while (StillDown());	HiliteControl(whichControl,0);}