/*
 * 2018 - Fajar Dwi Darmanto (fajardwidarm@gmail.com)
 * 
 * */


#include "screenshoot.h"
#include "type.h"
#include "window.h"
#include "preview.h"
#include "config.h"
#include "capture.h"
#include <stdlib.h>
#include <time.h>

// main variables
typedef struct {
	window *win;
	gint area;
	gint format;
	gint delay;
} Comp;

/* 
 * About dialog
 */

static void
about_dialog_init ();

/* 
 * function definition
 */ 

static void
about_dialog_init () 
{
	GtkWidget *dialog = NULL;
	gchar *version = NULL;
	
	version = g_strdup_printf ("%d.%d", SCREENSHOOT_VERSION_MAJOR, SCREENSHOOT_VERSION_MINOR);
	
	dialog = gtk_about_dialog_new ();
	gtk_about_dialog_set_program_name ((GtkAboutDialog*)dialog, "Screenshoot");
	gtk_about_dialog_set_logo_icon_name ((GtkAboutDialog*)dialog, "help-about");
	gtk_about_dialog_set_version ((GtkAboutDialog*)dialog, version);
	gtk_about_dialog_set_copyright ((GtkAboutDialog*)dialog, "(c) Fajar Dwi Darmanto");
	
	gtk_dialog_run (GTK_DIALOG (dialog));
	
	g_free (version);
	gtk_widget_destroy (dialog);
}

static gboolean
timeout_cb (gpointer user_data)
{
	Comp *comp = (Comp*) user_data;
	GdkPixbuf *pixbuf = NULL;
			 
	if (comp->area == ENTIRE_SCREEN) {
		pixbuf = take_screenshoot ();
	} else if (comp->area == ACTIVE_WINDOW) {
		pixbuf = take_active_window ();
	}
	
	if (pixbuf) {
		preview_init (NULL, pixbuf, comp->format);
		g_object_unref (pixbuf);
	}
	
	gtk_widget_show_all (comp->win->window);
	
	return FALSE;
}

static void 
take_screenshoot_cb (GtkWidget *widget, gpointer data)
{
	Comp *comp = (Comp*)data;
	
	gtk_widget_hide (comp->win->window);
	g_timeout_add (comp->delay, timeout_cb, data);
}

static void 
set_entire_cb (GtkWidget *widget, gpointer data)
{
	Comp *comp = (Comp*)data;
	if (gtk_toggle_button_get_active ((GtkToggleButton*)widget)) {
		comp->area = ENTIRE_SCREEN;
	}
}

static void 
set_active_cb (GtkWidget *widget, gpointer data)
{
	Comp *comp = (Comp*)data;
	if (gtk_toggle_button_get_active ((GtkToggleButton*)widget)) {
		comp->area = ACTIVE_WINDOW;
	}
}

static void 
set_jpeg_cb (GtkWidget *widget, gpointer data)
{
	Comp *comp = (Comp*)data;
	if (gtk_toggle_button_get_active ((GtkToggleButton*)widget)) {
		comp->format = JPEG;
	}
}

static void 
set_png_cb (GtkWidget *widget, gpointer data)
{
	Comp *comp = (Comp*)data;
	if (gtk_toggle_button_get_active ((GtkToggleButton*)widget)) {
		comp->format = PNG;
	}
}

static void 
set_bmp_cb (GtkWidget *widget, gpointer data)
{
	Comp *comp = (Comp*)data;
	if (gtk_toggle_button_get_active ((GtkToggleButton*)widget)) {
		comp->format = BMP;
	}
}

static void 
show_about_cb (GtkWidget *widget, gpointer data)
{
	about_dialog_init ();
}

// activate signal handler
static void 
activate (GtkApplication *app, gpointer data)
{
	Comp *comp = (Comp*)data;
	
	comp->win = window_init (app);
	
	g_signal_connect (comp->win->entirerb, "toggled", G_CALLBACK(set_entire_cb), data);
	g_signal_connect (comp->win->activerb, "toggled", G_CALLBACK(set_active_cb), data);
	g_signal_connect (comp->win->jpegrb, "toggled", G_CALLBACK(set_jpeg_cb), data);
	g_signal_connect (comp->win->pngrb, "toggled", G_CALLBACK(set_png_cb), data);
	g_signal_connect (comp->win->bmprb, "toggled", G_CALLBACK(set_bmp_cb), data);
	g_signal_connect (comp->win->takebtn, "clicked", G_CALLBACK(take_screenshoot_cb), data);
	g_signal_connect (comp->win->aboutbtn, "clicked", G_CALLBACK(show_about_cb), NULL);
	g_signal_connect_swapped (comp->win->exitbtn, "clicked", G_CALLBACK(gtk_widget_destroy), comp->win->window);
}

int 
screenshoot_init (int argc, char **argv)
{
	Comp comp;
	int status;
	
	comp.area = DEFAULT_AREA;
	comp.format = DEFAULT_FORMAT;
	comp.delay = DEFAULT_DELAY;
	
	gtk_init (&argc, &argv);
	
	GtkApplication *app = gtk_application_new ("org.gtk.screenshoot", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK(activate), &comp);
	
    status = g_application_run (G_APPLICATION(app), 0, NULL);
    
	g_object_unref (app);
	
	return status;
}
