
#include <stdio.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include "parse.h"

const gchar *url = "https://www.google.com";

int width = 800;
int height = 600;
GtkWidget *webview;
GtkWidget *entry;

static void html_finished(GObject *object, GAsyncResult *result, gpointer user_data)
{
	static int isChanged = 0;
	WebKitWebResource *wr = WEBKIT_WEB_RESOURCE(object);
	GError *error = NULL;
	gsize length;
	guchar *h = webkit_web_resource_get_data_finish(wr, result, &length, &error);
	gtk_entry_set_text(GTK_ENTRY(entry), webkit_web_resource_get_uri(wr));
	if (isChanged == 0)
	{
		int status = parser(h);
		if (status == -1)
			{
				isChanged = 0;
				return;
			}
		webkit_web_view_load_html(WEBKIT_WEB_VIEW(webview), originalWithTranslatedText->string, webkit_web_resource_get_uri(wr));
		free(originalWithTranslatedText->string);
		free(originalWithTranslatedText);
		isChanged = 1;
	}
	else
		isChanged = 0;

}

static void load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data)
{
	if (load_event == WEBKIT_LOAD_FINISHED)
	{
		WebKitWebResource *wr = webkit_web_view_get_main_resource(web_view);
		webkit_web_resource_get_data(wr, NULL, html_finished, NULL);
	}
}

static void load_failed(WebKitWebView *web_view, WebKitLoadEvent load_event,
						gchar *failing_uri, gpointer error, gpointer user_data)
{
	printf("load failed: %s: %s", ((GError *)error)->message, failing_uri);
}

static void webview_desroy_cb(GtkWidget *widget, GtkWidget *window)
{
	(void)widget;
	(void)window;
	gtk_main_quit();
}

/** Update the Webview when the URI is changed */
void update_WebKit_Uri_cb(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	if (event->keyval == 65293)
		webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), gtk_entry_get_text(GTK_ENTRY(entry)));

	else
		gtk_entry_set_text(GTK_ENTRY(entry), gtk_entry_get_text(GTK_ENTRY(entry)));
}

int main()
{
	GtkWidget *window;

	if (gtk_init_check(0, NULL) == FALSE)
	{
		return -1;
	}

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "");
	gtk_window_set_default_size(GTK_WINDOW(window), width, height);
	gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(webview_desroy_cb), NULL);

	webview = webkit_web_view_new();
	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), url);
	g_signal_connect(WEBKIT_WEB_VIEW(webview), "load-changed", G_CALLBACK(load_changed), NULL);
	g_signal_connect(WEBKIT_WEB_VIEW(webview), "load-failed", G_CALLBACK(load_failed), NULL);

	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), url);
	g_signal_connect(G_OBJECT(entry), "key-press-event", G_CALLBACK(update_WebKit_Uri_cb), NULL); //(gpointer)window

	gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(webview), TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	gtk_widget_show_all(window);

	gtk_main();
	return 0;
}