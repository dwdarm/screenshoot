/*
 * 2018 - Fajar Dwi Darmanto (fajardwidarm@gmail.com)
 * 
 * */

#include "preview.h"
#include "type.h"
#include <stdlib.h>
#include <time.h>

typedef struct {
	GtkWidget *entry;
	GdkPixbuf *pixbuf;
	gint format;
	gchar *path;
	gchar *filename;
} preview;

static void
preview_dialog_select_button_cb (GtkWidget *widget, gpointer user_data); 

static void
preview_dialog_action_button_cb (GtkDialog *dialog, gint response_id, gpointer user_data);

static gboolean
save_image_to_path (GdkPixbuf *pixbuf, const gchar *path, gint format);

void 
preview_init (GtkWidget *parent, GdkPixbuf *pixbuf, int format)
{
	GtkWidget *label = NULL;
	GtkWidget *dialog = NULL;
	GtkWidget *box1 = NULL, *box2 = NULL;
	GtkWidget *image = NULL;
	GtkWidget *entry = NULL;
	GtkWidget *sbtn = NULL;
	GdkPixbuf *spixbuf = NULL;
	preview *data = NULL;
	struct tm *timeinfo;
	time_t rawtime;
	
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	
	data = g_malloc (sizeof (preview));
	if (!data) {
		return;
	}
	
	data->filename = g_strdup_printf ("%d-%d-%d-%d-%d-%d", 
			         timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900,
			         timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
			 
	data->path = g_strdup_printf ("%s/%s", (gchar*)getenv ("HOME"), data->filename);
	data->pixbuf = pixbuf;
	data->format = format;
	
	dialog = gtk_dialog_new ();
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	
	// Dialog content area
	box1 = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	gtk_container_set_border_width (GTK_CONTAINER (box1), 25);
	gtk_box_set_spacing (GTK_BOX (box1), 10);
	
	// label
	label = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (label), "<b>Preview</b>");
	gtk_container_add (GTK_CONTAINER (box1), label);
	
	// scaled image data
	spixbuf = gdk_pixbuf_scale_simple (pixbuf, 
									   gdk_pixbuf_get_width (pixbuf)/2,
									   gdk_pixbuf_get_height (pixbuf)/2,
									   GDK_INTERP_NEAREST);
	
	// image
	image = gtk_image_new_from_pixbuf (spixbuf);
	gtk_container_add (GTK_CONTAINER (box1), image);
	
	// path entry box
	box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_pack_start (GTK_BOX (box2), gtk_label_new ("Save as:"), FALSE, FALSE, 0);
	
	// path entry
	entry = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (entry), data->path);
	gtk_widget_set_sensitive (entry, FALSE);
	gtk_box_pack_start (GTK_BOX (box2), entry, TRUE, TRUE, 0);
	data->entry = entry;
	
	sbtn = gtk_button_new_with_label ("Destination");
	gtk_box_pack_start (GTK_BOX (box2), sbtn, TRUE, TRUE, 0);
	
	gtk_container_add (GTK_CONTAINER (box1), box2);
	
	gtk_dialog_add_buttons (GTK_DIALOG (dialog), 
							"Cancel", 0, "Save", 1, NULL);
	
	g_signal_connect (sbtn, "clicked", G_CALLBACK(preview_dialog_select_button_cb), data);
	g_signal_connect (dialog, "response", G_CALLBACK(preview_dialog_action_button_cb), data);
	
	gtk_widget_show_all (dialog);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	g_object_unref (spixbuf);
	g_free (data->path);
	g_free (data->filename);
	g_free (data);
}

static void
preview_dialog_select_button_cb (GtkWidget *widget, gpointer user_data)
{
	preview *data = NULL;
	GtkWidget *chooser_dialog = NULL;
	gchar *folder = NULL;
	gint ret;
	
	data = (preview*) user_data;
	
	chooser_dialog = gtk_file_chooser_dialog_new ("Open File", NULL,
								  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, 
								  "Cancel", GTK_RESPONSE_CANCEL,
                                  "Open", GTK_RESPONSE_ACCEPT, NULL);
                                  
    ret = gtk_dialog_run (GTK_DIALOG (chooser_dialog));
    if (ret == GTK_RESPONSE_ACCEPT) {
		folder = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser_dialog));
		if (folder) {
			if (data->path) {
				g_free (data->path);
				data->path = NULL;
			}
			data->path = g_strdup_printf ("%s/%s", folder, data->filename);
			gtk_entry_set_text (GTK_ENTRY (data->entry), data->path);
		}	
	}
	
	gtk_widget_destroy (chooser_dialog);
}

static void
preview_dialog_action_button_cb (GtkDialog *dialog, gint response_id, gpointer user_data)
{
	preview *data = NULL;
	GtkWidget *saved_dialog = NULL;
	gboolean saved;
	
	data = (preview*) user_data;
	
	if (response_id != 1) {
		return;
	}
	
	saved = save_image_to_path (data->pixbuf, data->path, data->format);
	if (saved) {
		saved_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog),
										       GTK_DIALOG_MODAL,
										       GTK_MESSAGE_INFO,
										       GTK_BUTTONS_OK,
										       "File has been saved!");
		gtk_window_set_title (GTK_WINDOW (saved_dialog), "Information");
		gtk_dialog_run (GTK_DIALOG (saved_dialog));
		gtk_widget_destroy (saved_dialog);
	}
}

static gboolean
save_image_to_path (GdkPixbuf *pixbuf, const gchar *path, gint format)
{
	gchar *real_path = NULL;
	gboolean status = FALSE;
	
	switch (format) {
		case JPEG:
			real_path = g_strdup_printf ("%s.jpg", path);
			status = gdk_pixbuf_save (pixbuf, real_path, "jpeg", NULL, "quality", "100", NULL);
			break;
		case PNG:
			real_path = g_strdup_printf ("%s.png", path);
			status = gdk_pixbuf_save (pixbuf, real_path, "png", NULL, "quality", "100", NULL);
			break;
		case BMP:
			real_path = g_strdup_printf ("%s.bmp", path);
			status = gdk_pixbuf_save (pixbuf, real_path, "bmp", NULL, "quality", "100", NULL);
			break;
		default:
			break;
	}
	
	g_free (real_path);
	return status;
}
