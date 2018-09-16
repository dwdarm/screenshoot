/*
 * 2018 - Fajar Dwi Darmanto (fajardwidarm@gmail.com)
 * 
 * */


#include "screenshoot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// area crop options
enum {
	ENTIRE_SCREEN = 0,
	ACTIVE_WINDOW
};

// image output format options
enum {
	JPEG = 0,
	PNG,
	BMP
};

// main variables
typedef struct {
	GtkWidget *window;
	int area;
	int format;
	int delay;
} Comp;

#define DEFAULT_AREA ENTIRE_SCREEN
#define DEFAULT_FORMAT PNG
#define DEFAULT_DELAY 250

// file saving prompt dialog
GtkWidget *
init_save_dialog (GdkPixbuf *pixbuf, const char *filepath, GtkWidget **pathentry)
{
	GtkWidget *dialog = gtk_dialog_new ();
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	
	GtkWidget *box = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	gtk_container_set_border_width (GTK_CONTAINER (box), 25);
	gtk_box_set_spacing (GTK_BOX (box), 10);
	
	GdkPixbuf *scaledpixbuf = gdk_pixbuf_scale_simple (pixbuf, 
									gdk_pixbuf_get_width (pixbuf)/2,
									gdk_pixbuf_get_height (pixbuf)/2,
									GDK_INTERP_NEAREST);
	
	GtkWidget *label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), "<b>Preview</b>");
	gtk_container_add (GTK_CONTAINER (box), label);
	
	GtkWidget *image = gtk_image_new_from_pixbuf (scaledpixbuf);
	gtk_container_add (GTK_CONTAINER (box), image);
	
	GtkWidget *box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_pack_start (GTK_BOX (box2), gtk_label_new ("Save as:"), FALSE, FALSE, 0);
	GtkWidget *path_entry = gtk_entry_new ();
	*pathentry = path_entry;
	gtk_entry_set_text (GTK_ENTRY (path_entry), filepath);
	gtk_box_pack_start (GTK_BOX (box2), path_entry, TRUE, TRUE, 0);
	gtk_container_add (GTK_CONTAINER (box), box2);
	
	gtk_dialog_add_buttons (GTK_DIALOG (dialog), 
							"Cancel", 0, "Save", 1, NULL);
	
	gtk_widget_show_all (dialog);
	
	return dialog;
}

//file saved info dialog
GtkWidget *
init_saved_dialog (GtkWidget *parent) 
{
	GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW (parent),
												GTK_DIALOG_MODAL,
												GTK_MESSAGE_INFO,
												GTK_BUTTONS_OK,
												"File has been saved!");
	gtk_window_set_title (GTK_WINDOW (dialog), "Information");
	return dialog;
}

// about dialog
GtkWidget *
init_about_dialog () 
{
	char version[16];
	
	sprintf (version, "%d.%d", SCREENSHOOT_VERSION_MAJOR, SCREENSHOOT_VERSION_MINOR);
	GtkWidget *dialog = gtk_about_dialog_new ();
	gtk_about_dialog_set_program_name ((GtkAboutDialog*)dialog, "Screenshoot");
	gtk_about_dialog_set_logo_icon_name ((GtkAboutDialog*)dialog, "help-about");
	gtk_about_dialog_set_version ((GtkAboutDialog*)dialog, version);
	gtk_about_dialog_set_copyright ((GtkAboutDialog*)dialog, "(c) Fajar Dwi Darmanto");
	return dialog;
}

// timeout callback function
static int
timeout_cb (gpointer data)
{
	Comp *comp = (Comp*)data;
	time_t rawtime;
	struct tm * timeinfo;
	char filename[512];
	GdkPixbuf *pixbuf = NULL;
	GtkWidget *window = comp->window;
	gboolean status = FALSE;
	GtkWidget *pentry = NULL;
	
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	sprintf (filename, "%s/screenshoot-%d-%d-%d-%d-%d-%d", 
			 (char*)getenv("HOME"),
			 timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900,
			 timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
			 
	if (comp->area == ENTIRE_SCREEN) {
		pixbuf = take_screenshoot ();
	} else if (comp->area == ACTIVE_WINDOW) {
		pixbuf = take_active_window ();
	}
	
	if (pixbuf) {
		GtkWidget *dialog = init_save_dialog (pixbuf, filename, &pentry);
		int result = gtk_dialog_run (GTK_DIALOG (dialog));
		switch (result) {
			case 1:
				printf ("%s\n", gtk_entry_get_text (GTK_ENTRY (pentry)));
				if (comp->format == JPEG) {
					sprintf (filename, "%s.jpg", filename);
					status = gdk_pixbuf_save (pixbuf, filename, "jpeg", NULL, "quality", "100", NULL);
				} else if (comp->format == PNG) {
					sprintf (filename, "%s.png", filename);
					status = gdk_pixbuf_save (pixbuf, filename, "png", NULL, "quality", "100", NULL);
				} else if (comp->format == BMP) {
					sprintf (filename, "%s.bmp", filename);
					status = gdk_pixbuf_save (pixbuf, filename, "bmp", NULL, "quality", "100", NULL);
				}
				
				if (status) {
					GtkWidget *dialog2 = init_saved_dialog (dialog);
					gtk_dialog_run (GTK_DIALOG (dialog2));
					gtk_widget_destroy (dialog2);
				}
				
				break;
			default:
				break;
		}
		gtk_widget_destroy (dialog);
		g_object_unref (pixbuf);
	}
	
	gtk_widget_show_all (window);
	
	return FALSE;
}

static void 
take_screenshoot_cb (GtkWidget *widget, gpointer data)
{
	Comp *comp = (Comp*)data;
	GtkWidget *window = comp->window;
	gtk_widget_hide (window);
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
	GtkWidget *dialog = init_about_dialog ();
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

static void 
activate (GtkApplication *app, gpointer data)
{
	Comp *comp = (Comp*)data;
	
	GtkWidget *window = gtk_application_window_new (app);
	gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
	comp->window = window;
	
	GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER (window), box);
    
    GtkWidget *box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start (GTK_BOX (box), box1, FALSE, FALSE, 0);
    
    // choose area
    GtkWidget *box_area = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box_area), 15);
    gtk_box_pack_start (GTK_BOX (box1), box_area, FALSE, FALSE, 0);
    
    GtkWidget *arealbl = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (arealbl), "<b>Choose area</b>");
	gtk_box_pack_start (GTK_BOX (box_area), arealbl, FALSE, FALSE, 0);
	
	GtkWidget *entirerb = gtk_radio_button_new_with_label (NULL, "Entire screen");
	g_signal_connect (entirerb, "toggled", G_CALLBACK(set_entire_cb), data);
	gtk_box_pack_start (GTK_BOX (box_area), entirerb, FALSE, FALSE, 0);
	
	GtkWidget *activerb = gtk_radio_button_new_with_label (gtk_radio_button_get_group ((GtkRadioButton*)entirerb), 
															"Active window");
	g_signal_connect (activerb, "toggled", G_CALLBACK(set_active_cb), data);
	gtk_box_pack_start (GTK_BOX (box_area), activerb, FALSE, FALSE, 0);
	
	// format
	GtkWidget *box_format = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box_format), 15);
    gtk_box_pack_start (GTK_BOX (box1), box_format, FALSE, FALSE, 0);
    
    GtkWidget *formatlbl = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (formatlbl), "<b>Format</b>");
	gtk_box_pack_start (GTK_BOX (box_format), formatlbl, FALSE, FALSE, 0);
	
	GtkWidget *jpegrb = gtk_radio_button_new_with_label (NULL, "JPEG");
	gtk_box_pack_start (GTK_BOX (box_format), jpegrb, FALSE, FALSE, 0);
	g_signal_connect (jpegrb, "toggled", G_CALLBACK(set_jpeg_cb), data);
	
	GtkWidget *pngrb = gtk_radio_button_new_with_label (gtk_radio_button_get_group ((GtkRadioButton*)jpegrb), 
															"PNG");
	gtk_toggle_button_set_active ((GtkToggleButton*)pngrb, TRUE);
	g_signal_connect (pngrb, "toggled", G_CALLBACK(set_png_cb), data);
	gtk_box_pack_start (GTK_BOX (box_format), pngrb, FALSE, FALSE, 0);
	
	GtkWidget *bmprb = gtk_radio_button_new_with_label (gtk_radio_button_get_group ((GtkRadioButton*)jpegrb), 
															"BMP");
	g_signal_connect (bmprb, "toggled", G_CALLBACK(set_bmp_cb), data);
	gtk_box_pack_start (GTK_BOX (box_format), bmprb, FALSE, FALSE, 0);
    
    // button
    GtkWidget *box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box2), 15);
    gtk_box_pack_end (GTK_BOX (box), box2, FALSE, FALSE, 25);
	
	GtkWidget *button1 = gtk_button_new_with_label ("Take a screenshoot");
	g_signal_connect (button1, "clicked", G_CALLBACK(take_screenshoot_cb), data);
	gtk_box_pack_start (GTK_BOX (box2), button1, TRUE, TRUE, 5);
	
	GtkWidget *button2 = gtk_button_new_with_label ("About");
	g_signal_connect (button2, "clicked", G_CALLBACK(show_about_cb), NULL);
	gtk_box_pack_start (GTK_BOX (box2), button2, TRUE, TRUE, 5);
	
	GtkWidget *button3 = gtk_button_new_with_label ("Exit");
	g_signal_connect_swapped (button3, "clicked", G_CALLBACK(gtk_widget_destroy), window);
	gtk_box_pack_start (GTK_BOX (box2), button3, TRUE, TRUE, 5);
	
	gtk_widget_show_all (window);
}

int main (int argc, char **argv)
{
	Comp comp;
	comp.area = DEFAULT_AREA;
	comp.format = DEFAULT_FORMAT;
	comp.delay = DEFAULT_DELAY;
	
	gtk_init (&argc, &argv);
	
	GtkApplication *app = gtk_application_new ("org.gtk.screenshoot", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK(activate), &comp);
	
    int status = g_application_run (G_APPLICATION(app), 0, NULL);
	g_object_unref (app);
	return status;
}
