#include "compat.h"
#include "LinuxCamCapture.h"
#include "gui.h"
#include "ithread.h"
#include "pixel_conversion.h"
#include "CVideoFrame.h"
#include <iostream>

gboolean CGui::on_capture_destroy_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    gtk_main_quit();
    return FALSE;
}

void CGui::on_brightness_value_changed(GtkRange *range, gpointer user_data)
{
    GtkAdjustment *adj;
    adj = gtk_range_get_adjustment(range);

    CGui *pGui = (CGui *)user_data; 
    pGui->m_Cam.SetControlValue(BRIGHTNESS, (int)adj->value);
}

void CGui::on_contrast_value_changed(GtkRange *range, gpointer user_data)
{
    GtkAdjustment *adj;
    adj = gtk_range_get_adjustment(range);

    CGui *pGui = (CGui *)user_data;
    pGui->m_Cam.SetControlValue(CONTRAST, (int)adj->value);
}

void CGui::on_hue_value_changed( GtkRange *range, gpointer user_data)
{
    GtkAdjustment *adj;
    adj = gtk_range_get_adjustment(range);

    CGui *pGui = (CGui *)user_data;
    pGui->m_Cam.SetControlValue(HUE, (int)adj->value);
}

void CGui::on_saturation_value_changed(GtkRange *range, gpointer user_data)
{
    GtkAdjustment *adj;
    adj = gtk_range_get_adjustment(range);

    CGui *pGui = (CGui *)user_data;
    pGui->m_Cam.SetControlValue(SATURATION, (int)adj->value);
}

void CGui::on_gamma_value_changed(GtkRange *range, gpointer user_data)
{
    GtkAdjustment *adj;
    adj = gtk_range_get_adjustment(range);

    CGui *pGui = (CGui *)user_data;
    pGui->m_Cam.SetControlValue(0, (int)adj->value);
}

void CGui::on_xoff_value_changed(GtkRange *range, gpointer user_data)
{
    GtkAdjustment *adj;
    adj = gtk_range_get_adjustment(range);
}


void CGui::on_yoff_value_changed(GtkRange *range, gpointer user_data)
{
    GtkAdjustment *adj;
    adj = gtk_range_get_adjustment(range);
}

GtkWidget* CGui::lookup_widget(const gchar *widget_name)
{
    GtkWidget *parent, *found_widget;

    for (;;) {
        if (GTK_IS_MENU (m_Capture))
            parent = gtk_menu_get_attach_widget (GTK_MENU (m_Capture));
        else
            parent = m_Capture->parent;

        if (!parent)
            parent = (GtkWidget*) g_object_get_data (G_OBJECT (m_Capture), "GladeParentKey");

        if (parent == NULL)
            break;

        m_Capture = parent;
    }

    found_widget = (GtkWidget*) g_object_get_data (G_OBJECT (m_Capture), widget_name);

    if (!found_widget)
        g_warning ("Widget not found: %s", widget_name);

    return found_widget;
}


int CGui::DrawFrame(CGui *_pGui) 
{
    GtkWidget *picFrameWnd = _pGui->lookup_widget("picture");

    CVideoFrame videoFrame;
    _pGui->m_Cam.GrabFrame(videoFrame);
   	
    unsigned long width, height, datalen;
    unsigned char *pData;

    videoFrame.GetFrameDimensions(&width, &height);
    videoFrame.GetFrameData(&pData, &datalen);

    gdk_draw_rgb_image(picFrameWnd->window,
                       picFrameWnd->style->fg_gc[GTK_STATE_NORMAL],
                       0, 0, width, height,
                       GDK_RGB_DITHER_NONE,
                       pData,
                       width * 3);

    return 1;
}

void CGui::CreateWindow () 
{
    GtkWidget *vbox1;
    GtkWidget *scrolledwindow3;
    GtkWidget *viewport2;
    GtkWidget *picture;
    GtkWidget *table1;
    GtkWidget *frame1;
    GtkWidget *alignment1;
    GtkWidget *brightness;
    GtkWidget *label6;
    GtkWidget *frame2;
    GtkWidget *alignment2;
    GtkWidget *contrast;
    GtkWidget *label7;
    GtkWidget *frame3;
    GtkWidget *alignment3;
    GtkWidget *gamma;
    GtkWidget *label8;
    GtkWidget *frame4;
    GtkWidget *alignment4;
    GtkWidget *saturation;
    GtkWidget *label9;
    GtkWidget *frame5;
    GtkWidget *alignment5;
    GtkWidget *xoff;
    GtkWidget *label10;
    GtkWidget *frame6;
    GtkWidget *alignment6;
    GtkWidget *yoff;
    GtkWidget *label11;

    m_Capture = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_name (m_Capture, "capture");
    gtk_window_set_title (GTK_WINDOW (m_Capture), "capture");
    gtk_window_set_destroy_with_parent (GTK_WINDOW (m_Capture), TRUE);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_set_name (vbox1, "vbox1");
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (m_Capture), vbox1);

    scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_name (scrolledwindow3, "scrolledwindow3");
    gtk_widget_show (scrolledwindow3);
    gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow3, TRUE, TRUE, 0);
    gtk_widget_set_size_request (scrolledwindow3, 320, 240);

    viewport2 = gtk_viewport_new (NULL, NULL);
    gtk_widget_set_name (viewport2, "viewport2");
    gtk_widget_show (viewport2);
    gtk_container_add (GTK_CONTAINER (scrolledwindow3), viewport2);

    picture = gtk_drawing_area_new ();
    gtk_widget_set_name (picture, "picture");
    gtk_widget_show (picture);
    gtk_container_add (GTK_CONTAINER (viewport2), picture);
    gtk_widget_set_size_request (picture, 640, 480);

    table1 = gtk_table_new (3, 2, FALSE);
    gtk_widget_set_name (table1, "table1");
    gtk_widget_show (table1);
    gtk_box_pack_start (GTK_BOX (vbox1), table1, FALSE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (table1), 2);
    gtk_table_set_row_spacings (GTK_TABLE (table1), 1);
    gtk_table_set_col_spacings (GTK_TABLE (table1), 1);

    frame1 = gtk_frame_new (NULL);
    gtk_widget_set_name (frame1, "frame1");
    gtk_widget_show (frame1);
    gtk_table_attach (GTK_TABLE (table1), frame1, 0, 1, 0, 1,
                     (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                     (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL), 0, 0);
    gtk_frame_set_label_align (GTK_FRAME (frame1), 0.04, 0.5);

    alignment1 = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_set_name (alignment1, "alignment1");
    gtk_widget_show (alignment1);
    gtk_container_add (GTK_CONTAINER (frame1), alignment1);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment1), 0, 0, 12, 0);

    brightness = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (100, 0, 255, 0, 0, 0)));
    gtk_widget_set_name (brightness, "brightness");
    gtk_widget_show (brightness);
    gtk_container_add (GTK_CONTAINER (alignment1), brightness);
    gtk_scale_set_digits (GTK_SCALE (brightness), 0);

    label6 = gtk_label_new ("<b>bright</b>");
    gtk_widget_set_name (label6, "label6");
    gtk_widget_show (label6);
    gtk_frame_set_label_widget (GTK_FRAME (frame1), label6);
    gtk_label_set_use_markup (GTK_LABEL (label6), TRUE);

    frame2 = gtk_frame_new (NULL);
    gtk_widget_set_name (frame2, "frame2");
    gtk_widget_show (frame2);
    gtk_table_attach (GTK_TABLE (table1), frame2, 0, 1, 1, 2,
                     (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                     (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL), 0, 0);
    gtk_frame_set_label_align (GTK_FRAME (frame2), 0.04, 0.5);

    alignment2 = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_set_name (alignment2, "alignment2");
    gtk_widget_show (alignment2);
    gtk_container_add (GTK_CONTAINER (frame2), alignment2);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment2), 0, 0, 12, 0);

    contrast = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (40, 0, 255, 0, 0, 0)));
    gtk_widget_set_name (contrast, "contrast");
    gtk_widget_show (contrast);
    gtk_container_add (GTK_CONTAINER (alignment2), contrast);
    gtk_scale_set_digits (GTK_SCALE (contrast), 0);

    label7 = gtk_label_new ("<b>contrast</b>");
    gtk_widget_set_name (label7, "label7");
    gtk_widget_show (label7);
    gtk_frame_set_label_widget (GTK_FRAME (frame2), label7);
    gtk_label_set_use_markup (GTK_LABEL (label7), TRUE);

    frame3 = gtk_frame_new (NULL);
    gtk_widget_set_name (frame3, "frame3");
    gtk_widget_show (frame3);
    gtk_table_attach (GTK_TABLE (table1), frame3, 1, 2, 0, 1,
                     (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                     (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL), 0, 0);
    gtk_frame_set_label_align (GTK_FRAME (frame3), 0.04, 0.5);

    alignment3 = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_set_name (alignment3, "alignment3");
    gtk_widget_show (alignment3);
    gtk_container_add (GTK_CONTAINER (frame3), alignment3);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment3), 0, 0, 12, 0);

    gamma = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (30, 0, 255, 0, 0, 0)));
    gtk_widget_set_name (gamma, "gamma");
    gtk_widget_show (gamma);
    gtk_container_add (GTK_CONTAINER (alignment3), gamma);
    gtk_scale_set_digits (GTK_SCALE (gamma), 0);

    label8 = gtk_label_new ("<b>gamma</b>");
    gtk_widget_set_name (label8, "label8");
    gtk_widget_show (label8);
    gtk_frame_set_label_widget (GTK_FRAME (frame3), label8);
    gtk_label_set_use_markup (GTK_LABEL (label8), TRUE);

    frame4 = gtk_frame_new (NULL);
    gtk_widget_set_name (frame4, "frame4");
    gtk_widget_show (frame4);
    gtk_table_attach (GTK_TABLE (table1), frame4, 1, 2, 1, 2,
                     (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                     (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL), 0, 0);
    gtk_frame_set_label_align (GTK_FRAME (frame4), 0.04, 0.5);

    alignment4 = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_set_name (alignment4, "alignment4");
    gtk_widget_show (alignment4);
    gtk_container_add (GTK_CONTAINER (frame4), alignment4);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment4), 0, 0, 12, 0);

    saturation = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (20, 0, 255, 0, 0, 0)));
    gtk_widget_set_name (saturation, "saturation");
    gtk_widget_show (saturation);
    gtk_container_add (GTK_CONTAINER (alignment4), saturation);
    gtk_scale_set_digits (GTK_SCALE (saturation), 0);

    label9 = gtk_label_new ("<b>saturation</b>");
    gtk_widget_set_name (label9, "label9");
    gtk_widget_show (label9);
    gtk_frame_set_label_widget (GTK_FRAME (frame4), label9);
    gtk_label_set_use_markup (GTK_LABEL (label9), TRUE);

    frame5 = gtk_frame_new (NULL);
    gtk_widget_set_name (frame5, "frame5");
    gtk_widget_show (frame5);
    gtk_table_attach (GTK_TABLE (table1), frame5, 0, 1, 2, 3,
                     (GtkAttachOptions) (GTK_FILL),
                     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_frame_set_label_align (GTK_FRAME (frame5), 0.04, 0.5);

    alignment5 = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_set_name (alignment5, "alignment5");
    gtk_widget_show (alignment5);
    gtk_container_add (GTK_CONTAINER (frame5), alignment5);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment5), 0, 0, 12, 0);

    xoff = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 0, 0, 0, 0)));
    gtk_widget_set_name (xoff, "xoff");
    gtk_widget_show (xoff);
    gtk_container_add (GTK_CONTAINER (alignment5), xoff);
    gtk_scale_set_digits (GTK_SCALE (xoff), 0);

    label10 = gtk_label_new ("<b>x off</b>");
    gtk_widget_set_name (label10, "label10");
    gtk_widget_show (label10);
    gtk_frame_set_label_widget (GTK_FRAME (frame5), label10);
    gtk_label_set_use_markup (GTK_LABEL (label10), TRUE);

    frame6 = gtk_frame_new (NULL);
    gtk_widget_set_name (frame6, "frame6");
    gtk_widget_show (frame6);
    gtk_table_attach (GTK_TABLE (table1), frame6, 1, 2, 2, 3,
                     (GtkAttachOptions) (GTK_FILL),
                     (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_frame_set_label_align (GTK_FRAME (frame6), 0.04, 0.5);

    alignment6 = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_set_name (alignment6, "alicapturegnment6");
    gtk_widget_show (alignment6);
    gtk_container_add (GTK_CONTAINER (frame6), alignment6);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment6), 0, 0, 12, 0);

    yoff = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 0, 0, 0, 0)));
    gtk_widget_set_name (yoff, "yoff");
    gtk_widget_show (yoff);
    gtk_container_add (GTK_CONTAINER (alignment6), yoff);
    gtk_scale_set_digits (GTK_SCALE (yoff), 0);

    label11 = gtk_label_new ("<b>y off</b>");
    gtk_widget_set_name (label11, "label11");
    gtk_widget_show (label11);
    gtk_frame_set_label_widget (GTK_FRAME (frame6), label11);
    gtk_label_set_use_markup (GTK_LABEL (label11), TRUE);

    g_signal_connect ((gpointer) m_Capture, "delete_event",
                      G_CALLBACK (on_capture_destroy_event),
                      this);
    g_signal_connect ((gpointer) brightness, "value_changed",
                      G_CALLBACK (on_brightness_value_changed),
                      this);
    g_signal_connect ((gpointer) contrast, "value_changed",
                      G_CALLBACK (on_contrast_value_changed),
                      this);
    g_signal_connect ((gpointer) gamma, "value_changed",
                      G_CALLBACK (on_gamma_value_changed),
                      this);
    g_signal_connect ((gpointer) saturation, "value_changed",
                      G_CALLBACK (on_saturation_value_changed),
                      this);
    g_signal_connect ((gpointer) xoff, "value_changed",
                      G_CALLBACK (on_xoff_value_changed),
                      this);
    g_signal_connect ((gpointer) yoff, "value_changed",
                      G_CALLBACK (on_yoff_value_changed),
                      this);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF (m_Capture, m_Capture, "capture");
    GLADE_HOOKUP_OBJECT (m_Capture, vbox1, "vbox1");
    GLADE_HOOKUP_OBJECT (m_Capture, scrolledwindow3, "scrolledwindow3");
    GLADE_HOOKUP_OBJECT (m_Capture, viewport2, "viewport2");
    GLADE_HOOKUP_OBJECT (m_Capture, picture, "picture");
    GLADE_HOOKUP_OBJECT (m_Capture, table1, "table1");
    GLADE_HOOKUP_OBJECT (m_Capture, frame1, "frame1");
    GLADE_HOOKUP_OBJECT (m_Capture, alignment1, "alignment1");
    GLADE_HOOKUP_OBJECT (m_Capture, brightness, "brightness");
    GLADE_HOOKUP_OBJECT (m_Capture, label6, "label6");
    GLADE_HOOKUP_OBJECT (m_Capture, frame2, "frame2");
    GLADE_HOOKUP_OBJECT (m_Capture, alignment2, "alignment2");
    GLADE_HOOKUP_OBJECT (m_Capture, contrast, "contrast");
    GLADE_HOOKUP_OBJECT (m_Capture, label7, "label7");
    GLADE_HOOKUP_OBJECT (m_Capture, frame3, "frame3");
    GLADE_HOOKUP_OBJECT (m_Capture, alignment3, "alignment3");
    GLADE_HOOKUP_OBJECT (m_Capture, gamma, "gamma");
    GLADE_HOOKUP_OBJECT (m_Capture, label8, "label8");
    GLADE_HOOKUP_OBJECT (m_Capture, frame4, "frame4");
    GLADE_HOOKUP_OBJECT (m_Capture, alignment4, "alignment4");
    GLADE_HOOKUP_OBJECT (m_Capture, saturation, "saturation");
    GLADE_HOOKUP_OBJECT (m_Capture, label9, "label9");
    GLADE_HOOKUP_OBJECT (m_Capture, frame5, "frame5");
    GLADE_HOOKUP_OBJECT (m_Capture, alignment5, "alignment5");
    GLADE_HOOKUP_OBJECT (m_Capture, xoff, "xoff");
    GLADE_HOOKUP_OBJECT (m_Capture, label10, "label10");
    GLADE_HOOKUP_OBJECT (m_Capture, frame6, "frame6");
    GLADE_HOOKUP_OBJECT (m_Capture, alignment6, "alignment6");
    GLADE_HOOKUP_OBJECT (m_Capture, yoff, "yoff");
    GLADE_HOOKUP_OBJECT (m_Capture, label11, "label11");

    gtk_widget_show(m_Capture);

    gtk_idle_add((GtkFunction)CGui::DrawFrame,(gpointer)this);
    gtk_main();
}
