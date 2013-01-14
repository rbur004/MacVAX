#ifndef __WINDOW_H_#define __WINDOW_H_/* * Window IDs */#include <windows.h>#ifndef MEMORY_H#include "mmemory.h"#endif#define progwID		128#define regwID		129#define memwID		130#define iowID		131#define dumpdwID	132#define dumphwID	133#define physmemwID	134#define syswID		135#define stackwID	136typedef struct{	WindowPtr	windowptr;			/*pointer to window structure*/	Rect		usable;				/*actual useable area after removing space taken by scroll bars*/	ControlHandle  hScroll;			/*scroll bar*/ 	ControlHandle  vScroll;			/*scroll bar*/ 	/*ControlHandle  extracntl;		/*extra control handle for strange windows*/	long		grow;				/*can the window grow*/	Point		theOrigin;			/*window origin*/	long		fsize;				/*size of font in this window*/	long		lsize;				/*spacing for lines*/	long 		forigin;			/* # pixels up from base line to place pen*/	long		leading;			/*space between lines as defined in font info*/	long		nlines;				/*number of lines in display*/	long 		tlines;				/*total num lines 4 window*/	long		topMargin;			/*top of scrollable rect*/	address		mbase;				/*first memory location*/	long         secure;             /*if can't open it*/	void		(*update_window)(void);		/*handles update events*/	void		(*activate_window)(long); 	/*handles window specific activ events*/	void		(*click_window)(Point *, short);	/*handle mouse clicks*/	void 		(*keypress)(long);			/*handle keyboard input */	void 		(*undo_keypress)(void); 	/*handle Undo's for this window*/	void		(*doscroll)(short, short);	/*scroll for this window*/	void		(*dogrow)(void);			/*grow for this window*/} OurWindows, *OurWindowPtr;#define progWindow 		window[progwID - progwID]#define regWindow 		window[regwID - progwID]#define memWindow 		window[memwID - progwID]#define ioWindow 		window[iowID - progwID]#define dumpdWindow 	window[dumpdwID - progwID]#define dumphWindow 	window[dumphwID - progwID]#define physmemWindow	window[physmemwID - progwID]#define sysWindow		window[syswID - progwID]#define stackWindow		window[stackwID - progwID]#define NumWindows	9extern Rect					dragRect;extern OurWindows			window[NumWindows];extern WindowRecord			wRecord[NumWindows];	/* allocate space for windows */	extern ControlHandle 		whichControl;extern WindowPtr			theActiveWindow;extern WindowPtr			whichWindow;extern CursHandle			ibeamHdl;extern Rect					screenRect;extern   address last_lbase_h;extern   address first_stackaddress_h;extern   address last_sysbase_h;extern 	address last_lbase_d;extern 	address first_stackaddress_d;extern 	address last_sysbase_d; /*last line of system space*/#define ANDREG	1#define NOTREG	0#ifdef __WINDOW_C_Rect				dragRect;OurWindows			window[NumWindows];WindowRecord		wRecord[NumWindows];	/* allocate space for windows */WindowRecord		*wRecord_base_p = wRecord;	/* Base address for allocated space for windows */	ControlHandle 		whichControl = 0;WindowPtr			theActiveWindow  = 0;WindowPtr			whichWindow  = 0;CursHandle			ibeamHdl  = 0;Rect				screenRect;static RgnHandle	updateRgn;ControlActionUPP		sScrollUpProc = nil;ControlActionUPP		sScrollDownProc = nil;#endif __WINDOW_C_extern ControlActionUPP		sScrollUpProc;extern ControlActionUPP		sScrollDownProc;/*Functions*/void setupwindows(void);void setup_a_window(	short window_id,	short hscroll_id,	short vscroll_id,	short font_size,	void (*update_fn)(void),	void (*activate_window_fn)(long),	long top_margin_offset,	long growable,	void (*click_fn)(Point *, short),	void (*keypress_fn)(long),	void (*undo_keypress_fn)(void),	void (*doscroll_fn)(short,short),	void (*dogrow_fn)(void));void clear_all_windows(void);void doupdate(	OurWindowPtr owp,	long erase);void update_display(void);void update_memdisplay(	long andreg);void update_prog_reg_display(void);void do_activate(	OurWindowPtr wp,	long is_active);void MyDrawGrow(	OurWindowPtr lWindow);void cdlines(void);void sklines(void);void syslines(void);void fix_scrollbars_dec_hex(void);void MoveScrollBars(	OurWindowPtr lWindow);void UsableRect(	OurWindowPtr lWindow);void GrowWnd(	OurWindowPtr lWindow,	Point	where);void ScrollBits(void);pascal voidScrollUp(	ControlHandle whichControl,	short theCode);pascal voidScrollDown(	ControlHandle whichControl,	short theCode);void PageScroll(	short code,	short amount);void PageScroll_highlight(	short code,	short amount);#endif __WINDOW_H_