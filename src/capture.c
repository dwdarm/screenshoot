/*
 * 2018 - Fajar Dwi Darmanto (fajardwidarm@gmail.com)
 * 
 * */


#include "capture.h"

GdkPixbuf *
take_screenshoot ()
{
	GdkPixbuf *pixbuf = NULL;
	gint x=0, y=0, w=0, h=0;
	
	GdkScreen *screen = gdk_screen_get_default ();
	if (!screen) {
		return pixbuf;
	}
	
	GdkWindow *window = gdk_screen_get_root_window (screen);
	if (!window) {
		return pixbuf;
	}
	
	gdk_window_get_geometry (window, &x, &y, &w, &h);
	pixbuf = gdk_pixbuf_get_from_window (window, x, y, w, h);
	
	return pixbuf;
}

GdkPixbuf *
take_active_window ()
{
	GdkPixbuf *pixbuf = NULL;
	gint x=0, y=0, w=0, h=0;
	gint rox=0, roy=0;
	gint ox=0, oy=0; 
	
	GdkScreen *screen = gdk_screen_get_default ();
	if (!screen) {
		return pixbuf;
	}
	
	GdkWindow *root = gdk_screen_get_root_window (screen);
	if (!root) {
		return pixbuf;
	}
	
	GdkWindow *window = gdk_screen_get_active_window (screen);
	if (!window) {
		return pixbuf;
	}
	
	gdk_window_get_geometry (window, &x, &y, &w, &h);
	gdk_window_get_root_origin (window, &rox, &roy);
	gdk_window_get_origin (window, &ox, &oy);
	pixbuf = gdk_pixbuf_get_from_window (root, rox, roy, w+(ox-rox), h+(oy-roy));
	g_object_unref(window);
	
	return pixbuf;
}
