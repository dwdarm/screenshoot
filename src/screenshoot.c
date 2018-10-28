/*
 * 2018 - Fajar Dwi Darmanto (fajardwidarm@gmail.com)
 * 
 * */


#include "screenshoot.h"
#include <stdlib.h>
#include <time.h>

#define DEFAULT_AREA ENTIRE_SCREEN
#define DEFAULT_FORMAT PNG
#define DEFAULT_DELAY 250

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
	gint area;
	gint format;
	gint delay;
} Comp;


/* 
 * Preview dialog
 */

typedef struct {
	GtkWidget *entry;
	GdkPixbuf *pixbuf;
	gint format;
	gchar *path;
	gchar *filename;
} preview_dialog_data;

static void
preview_dialog_init (GdkPixbuf *pixbuf, Comp *comp);

static void
preview_dialog_select_button_cb (GtkWidget *widget, gpointer user_data); 

static void
preview_dialog_action_button_cb (GtkDialog *dialog, gint response_id, gpointer user_data);

/* 
 * About dialog
 */

static void
about_dialog_init ();

/*
 * save image to path
 */

static gboolean
save_image_to_path (GdkPixbuf *pixbuf, const gchar *path, gint format);



/* 
 * function definition
 */ 

static void
preview_dialog_init (GdkPixbuf *pixbuf, Comp *comp)
{
	GtkWidget *label = NULL;
	GtkWidget *dialog = NULL;
	GtkWidget *box1 = NULL, *box2 = NULL;
	GtkWidget *image = NULL;
	GtkWidget *entry = NULL;
	GtkWidget *sbtn = NULL;
	GdkPixbuf *spixbuf = NULL;
	preview_dialog_data *data = NULL;
	time_t rawtime;
	struct tm *timeinfo;
	
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	
	data = g_malloc (sizeof (preview_dialog_data));
	
	data->filename = g_strdup_printf ("screenshoot-%d-%d-%d-%d-%d-%d", 
			         timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900,
			         timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
			 
	data->path = g_strdup_printf ("%s/%s", (gchar*)getenv ("HOME"), data->filename);
	data->pixbuf = pixbuf;
	data->format = comp->format;
	
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
	preview_dialog_data *data = NULL;
	GtkWidget *chooser_dialog = NULL;
	gchar *folder = NULL;
	gint ret;
	
	data = (preview_dialog_data*) user_data;
	
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
	preview_dialog_data *data = NULL;
	GtkWidget *saved_dialog = NULL;
	gboolean saved;
	
	data = (preview_dialog_data*) user_data;
	
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

static gboolean
timeout_cb (gpointer user_data)
{
	Comp *comp = (Comp*) user_data;
	GdkPixbuf *pixbuf = NULL;
	GtkWidget *window = comp->window;
			 
	if (comp->area == ENTIRE_SCREEN) {
		pixbuf = take_screenshoot ();
	} else if (comp->area == ACTIVE_WINDOW) {
		pixbuf = take_active_window ();
	}
	
	if (pixbuf) {
		preview_dialog_init (pixbuf, comp);
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
	about_dialog_init ();
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
