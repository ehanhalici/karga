#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "cstring.h"
#include "transtlate.h"
#include "parse.h"

#define START "<body"
#define END "</body>"
#define SPACE " "
#define TAB "\t"
#define NEWLINE "\n"
#define TAGOPEN "<"
#define TAGCLOSE ">"
#define SCRIPTOPEN "<script"
#define SCRIPTCLOSE "</script>"
#define STYLEOPEN "<style"
#define STYLECLOSE "</style>"
#define HTMLSTART "<html"
#define LANG "lang=\"en\""
#define COMMENTOPEN "<!--"
#define COMMENTCLOSE "-->"

cstring modifiedHtml = NULL;
cstring originalWithTranslatedText = NULL; 

int isBanned(char *c)
{
	if (strlen(c) == 1)
		if (c[0] == '.' || c[0] == ',' || c[0] == ';' || c[0] == '?' || c[0] == '\"' || c[0] == '\'' || c[0] == '!' || c[0] == '-' || c[0] == '(' || c[0] == ')' || c[0] == '{' || c[0] == '}' || c[0] == '[' || c[0] == ']' || c[0] == '(')
			return 0;

	return -1;
}

void clearPunctuation(cstring c)
{
	int length = c->length(c) - 1;
	if (c->string[length] == '.' || c->string[length] == ',' || c->string[length] == ';')
		{
			c->deleteNtoN(c,length,length + 1);
		}
		
		
}

int parser(char* html)
{	
	cstring htmlClass = String(html);
	//clear comment
	int commentOpen = 0;
	int commentClose = 0;
	while(1==1)
	{
		commentOpen = htmlClass->find(htmlClass,0,COMMENTOPEN);
		if (commentOpen == -1)
			break;
		commentClose = htmlClass->find(htmlClass,commentOpen,COMMENTCLOSE);
		commentClose += strlen(COMMENTCLOSE);
		htmlClass->deleteNtoN(htmlClass, commentOpen, commentClose);
	}

	//is english
	int htmlStart = htmlClass->find(htmlClass,0,HTMLSTART);
	int htmlEnd = htmlClass->find(htmlClass,htmlStart,TAGCLOSE);
	int language = htmlClass->find(htmlClass,htmlStart,LANG);
	//this is not english
	if(language >  htmlEnd && language != -1)
		return -1;


	//find start and end
	
	int start = htmlClass->find(htmlClass, 0, START);
	int end = htmlClass->find(htmlClass, start, END);

	//clean places other than start and end
	htmlClass->deleteNtoN(htmlClass, end + strlen(END), htmlClass->length(htmlClass));
	htmlClass->deleteNtoN(htmlClass, 0, start);

	//detect script and delete
	int scriptStart = 0;
	int scriptClose = 0;
	while (1 == 1)
	{
		scriptStart = htmlClass->find(htmlClass, 0, SCRIPTOPEN);
		if (scriptStart == -1)
			break;
		scriptClose = htmlClass->find(htmlClass, scriptStart, SCRIPTCLOSE);
		scriptClose += strlen(SCRIPTCLOSE);
		htmlClass->deleteNtoN(htmlClass, scriptStart, scriptClose);
	}

	//detect style and delete
	int styleStart = 0;
	int styleClose = 0;
	while (1 == 1)
	{
		styleStart = htmlClass->find(htmlClass, 0, STYLEOPEN);
		if (styleStart == -1)
			break;
		styleClose = htmlClass->find(htmlClass, styleStart, STYLECLOSE);
		styleClose += strlen(STYLECLOSE);
		htmlClass->deleteNtoN(htmlClass, styleStart, styleClose);
	}
	//printf("%s",htmlClass->string);

	//grab all text and translate
	cstringList List = StringList();
	int tagOpen = 0;
	int tagClose = 0;
	while (1 == 1)
	{
		//find tag open
		tagClose = htmlClass->find(htmlClass, tagOpen, TAGCLOSE);
		if (tagClose == -1)
			break;
		//find tag close
		tagOpen = htmlClass->find(htmlClass, tagClose, TAGOPEN);
		cstring temp = String("");
		if (tagOpen == -1)
			break;

		//the text is between tag close and tag open
		htmlClass->slice(htmlClass, temp, tagClose + 1, tagOpen);
		//lowercase
		//temp->lower(temp);
		temp->replaceThisString(temp, "\n", "");
		temp->replaceThisString(temp, "\t", "");
		temp->replaceThisString(temp, "\v", "");
		//temp->replaceThisString(temp, " a ", "");
		//temp->replaceThisString(temp, " am ", " ");
		//temp->replaceThisString(temp, " is ", " ");
		//temp->replaceThisString(temp, " are ", " ");
		//temp->replaceThisString(temp, " the ", " ");
		//if(temp->isSpace(temp) == 0)
		//{
		//	free(temp->string);
		//	free(temp);
		//	continue;
		//}

		//if text is not space or is not banned or is not null, add list
		cstringList parseList = temp->getListFromParse(temp, " ");
		cstringList p = parseList;
		while (p != NULL)
		{
			if (p->data->string[0] != '\0' && p->data->isSpace(p->data) != 0 && isBanned(p->data->string) != 0)
			{
				clearPunctuation(p->data);
				List->listAdd(&List, p->data->string);
			}

			p = p->next;
		}

		free(parseList);
		free(temp->string);
		free(temp);
	}

	//now we can transtlate this list
	cstring postTranstlate;
	cstringList translationTextList = StringList();
	cstringList ptr = List;
	//initialize trasnlation varriable
	init();
	//google transtle's max entity is 5000
	int requestEntity = 4850;
	int breakIt = 0;
	while (1 == 1)
	{
		//create it -> word1\nword2\n...
		postTranstlate = String("");
		while( postTranstlate->length(postTranstlate) < requestEntity)
		{
			postTranstlate->addEnd(postTranstlate, ptr->data->string);
			postTranstlate->addEndNewLine(postTranstlate);
			if (ptr->next != NULL)
				ptr = ptr->next;
			else
			{
				breakIt = 1;
				break;
			}

		}
		//and transtlete
		transtlate(postTranstlate->string);
		//get translation text
		cstringList t = processedText->getListFromParse(processedText,"\n");
		cstringList p = t;
		for (int i = 0; i < t->listCount(t); i++)
		{
			translationTextList->listAdd(&translationTextList,p->data->string );
			p=p->next;
		}
		
			
		//clear memory for other loop step
		free(processedText->string);
		free(processedText);
		processedText = NULL;
		free(postTranstlate->string);
		free(postTranstlate);
		//It's good idea to wait a little bit between two requests
		sleep(1);
		fprintf(stderr,"-\n");
		if(breakIt == 1)
			break;	
	}
	//clear memory form trasnlation varriable
	finish();

	//printf("%s",translationTextList->data->string);

	
	modifiedHtml = String("");
	//printf("%s",modifiedHtml->string);
	cstringList ListPtr = List;
	cstringList translationTextListPtr = translationTextList;
	int modifiedHtmlIndex = start;
	originalWithTranslatedText = String(html);
	//clear comment
	commentOpen = 0;
	commentClose = 0;
	while(1==1)
	{
		commentOpen = originalWithTranslatedText->find(originalWithTranslatedText,0,COMMENTOPEN);
		if (commentOpen == -1)
			break;
		commentClose = originalWithTranslatedText->find(originalWithTranslatedText,commentOpen,COMMENTCLOSE);
		commentClose += strlen(COMMENTCLOSE);
		originalWithTranslatedText->deleteNtoN(originalWithTranslatedText, commentOpen, commentClose);
	}

	//	//detect script and delete
	//scriptStart = 0;
	//scriptClose = 0;
	//while (1 == 1)
	//{
	//	scriptStart = originalWithTranslatedText->find(originalWithTranslatedText, 0, SCRIPTOPEN);
	//	if (scriptStart == -1)
	//		break;
	//	scriptClose = originalWithTranslatedText->find(originalWithTranslatedText, scriptStart, SCRIPTCLOSE);
	//	scriptClose += strlen(SCRIPTCLOSE);
	//	originalWithTranslatedText->deleteNtoN(originalWithTranslatedText, scriptStart, scriptClose);
	//}
//
	////detect style and delete
	//styleStart = 0;
	//styleClose = 0;
	//while (1 == 1)
	//{
	//	styleStart = originalWithTranslatedText->find(originalWithTranslatedText, 0, STYLEOPEN);
	//	if (styleStart == -1)
	//		break;
	//	styleClose = originalWithTranslatedText->find(originalWithTranslatedText, styleStart, STYLECLOSE);
	//	styleClose += strlen(STYLECLOSE);
	//	originalWithTranslatedText->deleteNtoN(originalWithTranslatedText, styleStart, styleClose);
	//}

	//printf("%s",originalWithTranslatedText->string);
	start = originalWithTranslatedText->find(originalWithTranslatedText,0 ,START);
	end = originalWithTranslatedText->find(originalWithTranslatedText,start ,END);;
	tagOpen = start + 1;
	tagClose = start + 1;
	scriptStart = originalWithTranslatedText->find(originalWithTranslatedText,start ,SCRIPTOPEN);
	scriptClose = originalWithTranslatedText->find(originalWithTranslatedText,scriptStart ,SCRIPTCLOSE);
	scriptClose += strlen(SCRIPTCLOSE);
	styleStart = originalWithTranslatedText->find(originalWithTranslatedText,start,STYLEOPEN);
	styleClose = originalWithTranslatedText->find(originalWithTranslatedText,styleStart,STYLECLOSE);
	styleClose += strlen(STYLECLOSE);


	

	for (int i = 0; i < List->listCount(List); i)
	{
		tagClose = originalWithTranslatedText->find(originalWithTranslatedText,tagOpen,TAGCLOSE);		
		if (tagClose == -1)
			break;
		tagOpen = originalWithTranslatedText->find(originalWithTranslatedText,tagClose,TAGOPEN);
		if (tagOpen == -1)
			break;
		
		if(tagClose >= scriptStart && tagClose <= scriptClose && scriptStart != -1)
			{

				
				tagOpen = scriptClose + 1;
				tagClose = scriptClose + 1;
				scriptStart = originalWithTranslatedText->find(originalWithTranslatedText,scriptClose,SCRIPTOPEN);
				scriptClose = originalWithTranslatedText->find(originalWithTranslatedText,scriptStart,SCRIPTCLOSE);
				scriptClose += strlen(SCRIPTCLOSE);
				continue;
			}
		if(tagClose >= styleStart && tagClose <= styleClose && styleStart != -1)
			{
				tagOpen = styleClose + 1;
				tagClose = styleClose + 1;
				styleStart = originalWithTranslatedText->find(originalWithTranslatedText,styleClose,STYLEOPEN);
				styleClose = originalWithTranslatedText->find(originalWithTranslatedText,styleStart,STYLECLOSE);
				styleClose += strlen(STYLECLOSE);
				continue;
			}
		modifiedHtmlIndex = originalWithTranslatedText->find(originalWithTranslatedText, tagClose,ListPtr->data->string);
		while(modifiedHtmlIndex < tagOpen && modifiedHtmlIndex > tagClose)
		{
 			modifiedHtml->addEnd(modifiedHtml,"<span title=\"");
			modifiedHtml->addEnd(modifiedHtml,translationTextListPtr->data->string);
			modifiedHtml->addEnd(modifiedHtml,"\">");
			modifiedHtml->addEnd(modifiedHtml,ListPtr->data->string);
			modifiedHtml->addEnd(modifiedHtml,"</span>");
			originalWithTranslatedText->replaceThisIndex(originalWithTranslatedText,modifiedHtmlIndex,modifiedHtmlIndex + ListPtr->data->length(ListPtr->data),modifiedHtml->string);
			
			tagClose =modifiedHtmlIndex + modifiedHtml->length(modifiedHtml);
			tagOpen =tagOpen +  modifiedHtml->length(modifiedHtml) - ListPtr->data->length(ListPtr->data);
			if(scriptStart != -1)
			{
				scriptStart += modifiedHtmlIndex + modifiedHtml->length(modifiedHtml);
				scriptClose += modifiedHtmlIndex + modifiedHtml->length(modifiedHtml);
			}
			if(styleStart != -1)
			{
				styleStart += modifiedHtmlIndex + modifiedHtml->length(modifiedHtml);
				styleClose += modifiedHtmlIndex + modifiedHtml->length(modifiedHtml);
			}
			modifiedHtml->new(modifiedHtml,"");
			ListPtr = ListPtr->next;
			i++;
			if(i == List->listCount(List))
				break;
			modifiedHtmlIndex = originalWithTranslatedText->find(originalWithTranslatedText, tagClose,ListPtr->data->string);
			translationTextListPtr = translationTextListPtr->next;
			
		}

	}

	//printf("%s",originalWithTranslatedText->string);
	return 0;

}
