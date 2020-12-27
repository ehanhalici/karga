#include <stdio.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <curl/curl.h>
#include "cstring.h"
#include "transtlate.h"


#define BASIC_GOOGLE_TRANSLATE_URI "https://translate.google.com/m?tl="
#define SOURCE "auto"
#define DESTINATION "tr"
static char *URIString = NULL;
cstring processedText = NULL;

static GtkWidget *webview;
static GtkWidget *window;

static void webview_desroy_cb()
{

	free(URIString);
	URIString=NULL;
	gtk_main_quit();
}

static void html_finished(GObject *object, GAsyncResult *result, gpointer user_data)
{
	WebKitWebResource *wr = WEBKIT_WEB_RESOURCE(object);
	GError *error = NULL;
	gsize length;
	cstring html = String("");
	free(html->string);
	html->string = webkit_web_resource_get_data_finish(wr, result, &length, &error);
	if(processedText == NULL)
		processedText = String("");
	else
		processedText->new(processedText,"");
	
	cstring temp = String("");
	int index = -1;

	index = html->find(html,index + 1,"\"result-container\"");
	if(index == -1)
		return;
	int end = html->find(html,index,"<");

	html->slice(html,temp,index+19,end);
	processedText->addEnd(processedText,temp->string);
	//printf("%s",temp->string);
	
	free(temp->string);
	free(temp);
	free(html->string);
	free(html);
	webview_desroy_cb();
}

static void load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data)
{
	if(load_event == WEBKIT_LOAD_FINISHED)
	{
		WebKitWebResource *wr = webkit_web_view_get_main_resource(web_view);
		webkit_web_resource_get_data(wr, NULL, html_finished, NULL);
	}
	
}

static void load_failed(WebKitWebView *web_view, WebKitLoadEvent load_event,
  gchar *failing_uri, gpointer error, gpointer user_data) {
	  webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), failing_uri);
}


int buildURIString(char *originalText)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL *curl = curl_easy_init();
	char *escapedStr = NULL;

	// this replaces special characters  encoding into URI ( eg:  ' '  is %20 )
	escapedStr = curl_easy_escape(curl, originalText, strlen(originalText));
	
	if ((URIString = (char *)calloc(sizeof(char), strlen(BASIC_GOOGLE_TRANSLATE_URI) + strlen(escapedStr) + 30)) == NULL)
	{
		printf("\n\n CALLOC ERROR!  \n\n");
		return (-1);
	}
	strcpy(URIString, BASIC_GOOGLE_TRANSLATE_URI);
	strcat(URIString, SOURCE);	// source language
	strcat(URIString, "&sl=");		// source language
	strcat(URIString, DESTINATION);	// source language
	strcat(URIString, "&q=");		// 'to' language
	strcat(URIString, escapedStr); // original text in URI format

	//printf("%s", URIString);
	free(escapedStr);
	curl_easy_cleanup(curl);
	curl_global_cleanup();


	return 0;
}


void transtlate(char *originalText)
{
	buildURIString(originalText);
	
	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), URIString);
	
	
    //gtk_window_set_default_size(GTK_WINDOW(window), 1280, 1024);


	gtk_main();
}

void init()
{
	webview = webkit_web_view_new();
	window = gtk_offscreen_window_new();
	
	g_signal_connect(WEBKIT_WEB_VIEW(webview), "load-changed", G_CALLBACK(load_changed), NULL);
	g_signal_connect(WEBKIT_WEB_VIEW(webview), "load-failed", G_CALLBACK(load_failed), NULL);
	
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(webview));
    gtk_widget_show_all(window);
}

void finish()
{
	(void)webview;//free(webview);
	(void)window;//free(window);
	//free(processedText->string);
	//free(processedText);
}