/*
 * 2018 - Fajar Dwi Darmanto (fajardwidarm@gmail.com)
 * 
 * */


#ifndef CAPTURE_H
#define CAPTURE_H

#ifndef __GTK_H__
#include <gtk/gtk.h>
#endif

GdkPixbuf *take_screenshoot ();

GdkPixbuf *take_active_window ();

#endif
