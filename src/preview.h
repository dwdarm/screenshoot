/*
 * 2018 - Fajar Dwi Darmanto (fajardwidarm@gmail.com)
 * 
 * */
 
#ifndef PREVIEW_H
#define PREVIEW_H

#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif

void preview_init (GtkWidget *parent, GdkPixbuf *pixbuf, int format);

#endif
