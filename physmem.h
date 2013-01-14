#ifndef __PHYSMEM_H_#define __PHYSMEM_H_#ifdef __PHYSMEM_C_#define ASCII_START 49#define ASCII_END 	64static long  physmemw_cline = -1;	/*line start value for cursor*/static long  physmemw_cbyte = 0; 	/*offset within the line*/static long	 physmemw_byte = 0 ;	/*# of char spacings from column1 to cursor*/static short physmemw_cshift = 0;	/*shift needed to access nible (0,4)*/static long  physmem_in_ascii = 0;	/*in ascii part of display*/static void draw_p_cursor(	short v);static void draw_p_watches(	char *box,	short v);#endif __PHYSMEM_C_/*Functions*/void physmemw_goto(	register address a);void physmemw_click(	Point *p,	short modifiers);address get_c_pos_phys(void);void physmemw_keypress(	long key);void physmemw_undo_keypress(void);void update_physmem_window(void);void activate_physmem_window(	long is_active);void physmem_init(void);#endif __PHYSMEM_H_