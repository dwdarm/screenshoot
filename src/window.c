/*
 * 2018 - Fajar Dwi Darmanto (fajardwidarm@gmail.com)
 * 
 * */

#include "window.h"

window *
window_init (GtkApplication *app)
{
	window *ret = NULL;
	GtkWidget *box = NULL;
	GtkWidget *box1 = NULL;
	GtkWidget *box_area = NULL;
	GtkWidget *arealbl = NULL;
	GtkWidget *box_format = NULL;
	GtkWidget *formatlbl = NULL;
	GtkWidget *box2 = NULL;
	
	ret = g_malloc (sizeof (window));
	if (!ret) {
		return ret;
	}
	
	ret->window = gtk_application_window_new (app);
	gtk_window_set_resizable (GTK_WINDOW (ret->window), FALSE);
	gtk_window_set_position (GTK_WINDOW (ret->window), GTK_WIN_POS_CENTER);
	
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER (ret->window), box);
    
    box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start (GTK_BOX (box), box1, FALSE, FALSE, 0);
    
    // choose area
    box_area = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box_area), 15);
    gtk_box_pack_start (GTK_BOX (box1), box_area, FALSE, FALSE, 0);
    
    arealbl = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (arealbl), "<b>Choose area</b>");
	gtk_box_pack_start (GTK_BOX (box_area), arealbl, FALSE, FALSE, 0);
	
	ret->entirerb = gtk_radio_button_new_with_label (NULL, "Entire screen");
	gtk_box_pack_start (GTK_BOX (box_area), ret->entirerb, FALSE, FALSE, 0);
	
	ret->activerb = gtk_radio_button_new_with_label (gtk_radio_button_get_group ((GtkRadioButton*)ret->entirerb), 
															"Active window");
	gtk_box_pack_start (GTK_BOX (box_area), ret->activerb, FALSE, FALSE, 0);
	
	// format
	box_format = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box_format), 15);
    gtk_box_pack_start (GTK_BOX (box1), box_format, FALSE, FALSE, 0);
    
    formatlbl = gtk_label_new (NULL);
	gtk_label_set_markup (GTK_LABEL (formatlbl), "<b>Format</b>");
	gtk_box_pack_start (GTK_BOX (box_format), formatlbl, FALSE, FALSE, 0);
	
	ret->jpegrb = gtk_radio_button_new_with_label (NULL, "JPEG");
	gtk_box_pack_start (GTK_BOX (box_format), ret->jpegrb, FALSE, FALSE, 0);
	
	ret->pngrb = gtk_radio_button_new_with_label (gtk_radio_button_get_group ((GtkRadioButton*)ret->jpegrb), 
															"PNG");
	gtk_toggle_button_set_active ((GtkToggleButton*)ret->pngrb, TRUE);
	gtk_box_pack_start (GTK_BOX (box_format), ret->pngrb, FALSE, FALSE, 0);
	
	ret->bmprb = gtk_radio_button_new_with_label (gtk_radio_button_get_group ((GtkRadioButton*)ret->jpegrb), 
															"BMP");
	gtk_box_pack_start (GTK_BOX (box_format), ret->bmprb, FALSE, FALSE, 0);
    
    // button
    box2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box2), 15);
    gtk_box_pack_end (GTK_BOX (box), box2, FALSE, FALSE, 25);
	
	ret->takebtn = gtk_button_new_with_label ("Take a screenshoot");
	gtk_box_pack_start (GTK_BOX (box2), ret->takebtn, TRUE, TRUE, 5);
	
	ret->aboutbtn = gtk_button_new_with_label ("About");
	gtk_box_pack_start (GTK_BOX (box2), ret->aboutbtn, TRUE, TRUE, 5);
	
	ret->exitbtn = gtk_button_new_with_label ("Exit");
	gtk_box_pack_start (GTK_BOX (box2), ret->exitbtn, TRUE, TRUE, 5);
	
	gtk_widget_show_all (ret->window);
	
	return ret;
}
