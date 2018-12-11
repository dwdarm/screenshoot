/*
 * 2018 - Fajar Dwi Darmanto (fajardwidarm@gmail.com)
 * 
 * */
 
#ifndef WINDOW_H
#define WINDOW_H

#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif

typedef struct {
	GtkWidget *window;
	GtkWidget *entirerb;
	GtkWidget *activerb;
	GtkWidget *jpegrb;
	GtkWidget *pngrb;
	GtkWidget *bmprb;
	GtkWidget *takebtn;
	GtkWidget *aboutbtn;
	GtkWidget *exitbtn;
} window;

window *window_init (GtkApplication *app);

#endif
