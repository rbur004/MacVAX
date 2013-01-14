#ifndef __HEXDUMP_H_#define __HEXDUMP_H_#ifdef __HEXDUMP_C_#define ASCII_START 49#define ASCII_END 	64static long		dumphw_cline  = -1; /* line start value for cursor */static long		dumphw_cbyte  =  0;  /* offset within the line */static long		dumphw_byte   =  0 ; /* # of char spacings from column1 to cursor */static short	dumphw_cshift =  0;  /* shift needed to access nible (0,4) */long			in_ascii      =  0;	 /*in ascii part of display */static void draw_h_cursor(	/*draw the cursor*/	short v);static void draw_h_watches(	/*Draw the watch point outlined boxes*/	char *box,	/*array of bytes with 1's set if a watch point box needed*/	short v		);#else __HEXDUMP_C_extern long			in_ascii;	 /*in ascii part of display */#endif __HEXDUMP_C_void dumphw_goto(	/*Shift the window origin to display address a*/	register address a);void dumphw_click(	/*Process a click in the window*/	Point *p,	short modifiers);address get_c_pos_h(	/*Return the cursor position	 */	/*Return -1 if the cursor is nil */	void);void dumphw_keypress(	/*Handle Key presses when window is active*/	long key);void dumphw_undo_keypress(	/*Handle Undo's*/	void);void update_hexmem_window(	/*Called in response to an update event*/	void);void activate_hexmem_window(	/*Called in response to an activate event*/	long is_active);#endif __HEXDUMP_H_