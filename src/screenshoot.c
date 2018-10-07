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

// preview dialog
typedef struct {
	GtkWidget *dialog;
	GtkWidget *label;
	GtkWidget *image;
	
	GtkWidget *pbox;
	GtkWidget *entry;
	GtkWidget *cbtn;
	
	GdkPixbuf *pixbuf;
	char path[512];
	char filename[512];
} PreviewDialog;

#define DEFAULT_AREA ENTIRE_SCREEN
#define DEFAULT_FORMAT PNG
#define DEFAULT_DELAY 250

static void
select_dest_cb (GtkWidget *widget, gpointer data)
{
	PreviewDialog *prevd = (PreviewDialog*) data;
	GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open File", NULL,
								  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, 
								  "Cancel", GTK_RESPONSE_CANCEL,
                                  "Open", GTK_RESPONSE_ACCEPT, NULL);
    int ret = gtk_dialog_run (GTK_DIALOG (dialog));
    if (ret == GTK_RESPONSE_ACCEPT) {
		gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		if (filename) {
			sprintf (prevd->path, "%s/%s", filename, prevd->filename);
			gtk_entry_set_text (GTK_ENTRY (prevd->entry), prevd->path);
		}	
	}
	
	gtk_widget_destroy (dialog);
}

// file saving prompt dialog
static PreviewDialog *
preview_dialog_init (GdkPixbuf *pixbuf)
{
	time_t rawtime;
	struct tm *timeinfo;
	
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	
	PreviewDialog *ret = malloc (sizeof (PreviewDialog));
	sprintf (ret->filename, "screenshoot-%d-%d-%d-%d-%d-%d", 
			 timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900,
			 timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
			 
	sprintf (ret->path, "%s/%s", (char*)getenv ("HOME"), ret->filename);
	
	ret->dialog = gtk_dialog_new ();
	gtk_window_set_resizable (GTK_WINDOW (ret->dialog), FALSE);
	
	// Dialog content area
	GtkWidget *box = gtk_dialog_get_content_area (GTK_DIALOG (ret->dialog));
	gtk_container_set_border_width (GTK_CONTAINER (box), 25);
	gtk_box_set_spacing (GTK_BOX (box), 10);
	
	// label
	ret->label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (ret->label), "<b>Preview</b>");
	gtk_container_add (GTK_CONTAINER (box), ret->label);
	
	// scaled image data
	ret->pixbuf = gdk_pixbuf_scale_simple (pixbuf, 
									gdk_pixbuf_get_width (pixbuf)/2,
									gdk_pixbuf_get_height (pixbuf)/2,
									GDK_INTERP_NEAREST);
	
	// image
	ret->image = gtk_image_new_from_pixbuf (ret->pixbuf);
	gtk_container_add (GTK_CONTAINER (box), ret->image);
	
	// path entry box
	ret->pbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_pack_start (GTK_BOX (ret->pbox), gtk_label_new ("Save as:"), FALSE, FALSE, 0);
	
	// path entry
	ret->entry = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (ret->entry), ret->path);
	gtk_widget_set_sensitive (ret->entry, FALSE);
	gtk_box_pack_start (GTK_BOX (ret->pbox), ret->entry, TRUE, TRUE, 0);
	
	ret->cbtn = gtk_button_new_with_label ("Destination");
	gtk_box_pack_start (GTK_BOX (ret->pbox), ret->cbtn, TRUE, TRUE, 0);
	
	gtk_container_add (GTK_CONTAINER (box), ret->pbox);
	
	gtk_dialog_add_buttons (GTK_DIALOG (ret->dialog), 
							"Cancel", 0, "Save", 1, NULL);
	
	g_signal_connect (ret->cbtn, "clicked", G_CALLBACK(select_dest_cb), ret);
	gtk_widget_show_all (ret->dialog);
	
	return ret;
}

static void
preview_dialog_destroy (PreviewDialog *p) 
{
	gtk_widget_destroy (p->dialog);
	g_object_unref (p->pixbuf);
	free (p);
}

//file saved info dialog
static GtkWidget *
saved_dialog_init (GtkWidget *parent) 
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
static GtkWidget *
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

static void
save_image_to_path (Comp *comp, GdkPixbuf *pixbuf, const char *path)
{
	char filename[512];
	gboolean status = FALSE;
	
	switch (comp->format) {
		case JPEG:
			sprintf (filename, "%s.jpg", path);
			status = gdk_pixbuf_save (pixbuf, filename, "jpeg", NULL, "quality", "100", NULL);
			break;
		case PNG:
			sprintf (filename, "%s.png", path);
			status = gdk_pixbuf_save (pixbuf, filename, "png", NULL, "quality", "100", NULL);
			break;
		case BMP:
			sprintf (filename, "%s.bmp", path);
			status = gdk_pixbuf_save (pixbuf, filename, "bmp", NULL, "quality", "100", NULL);
			break;
		default:
			break;
	}
	
	if (status) {
		GtkWidget *saved_dialog = saved_dialog_init (NULL);
		gtk_dialog_run (GTK_DIALOG (saved_dialog));
		gtk_widget_destroy (saved_dialog);
	}
	
}

// timeout callback function
static int
timeout_cb (gpointer data)
{
	Comp *comp = (Comp*)data;
	PreviewDialog *prevd = NULL;
	GdkPixbuf *pixbuf = NULL;
	GtkWidget *window = comp->window;
			 
	if (comp->area == ENTIRE_SCREEN) {
		pixbuf = take_screenshoot ();
	} else if (comp->area == ACTIVE_WINDOW) {
		pixbuf = take_active_window ();
	}
	
	if (pixbuf) {
		prevd = preview_dialog_init (pixbuf);
		int result = gtk_dialog_run (GTK_DIALOG (prevd->dialog));
		switch (result) {
			case 1:
				save_image_to_path (comp, pixbuf, gtk_entry_get_text (GTK_ENTRY (prevd->entry)));
				break;
			default:
				break;
		}
		preview_dialog_destroy (prevd);
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

// activate signal handler
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

// main function
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
