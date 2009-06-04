#include  <gtk/gtk.h>
#include "LinuxCamCapture.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

using namespace tv::ip;

//class CamAbstractionLayerV4L;
class CGui 
{

private:

    GtkWidget * m_Capture;
    CamAbstractionLayer & m_Cam;

public:

    CGui(CamAbstractionLayer & _cam) : m_Cam(_cam) 
    { 
        m_Capture = NULL;
    }
    
    ~CGui() {}

    static  gboolean on_capture_destroy_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
    static  void on_brightness_value_changed(GtkRange *range, gpointer user_data);
    static  void on_contrast_value_changed(GtkRange *range, gpointer user_data);
    static  void on_hue_value_changed( GtkRange *range, gpointer user_data);
    static  void on_saturation_value_changed(GtkRange *range, gpointer user_data);
    static  void on_gamma_value_changed(GtkRange *range, gpointer   user_data);
    static  void on_xoff_value_changed(GtkRange *range, gpointer user_data);
    static  void on_yoff_value_changed(GtkRange *range, gpointer user_data);
    static  int DrawFrame(CGui *_pGui);

    void CreateWindow();

    GtkWidget * lookup_widget(const gchar *widget_name);
};
