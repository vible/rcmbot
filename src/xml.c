/* 
 * rcmbot - IRC bot
 * Copyright (C) 2013  Andreas "vible" Ölund
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "xml.h"

#include "includes.h"

#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <stdio.h>
#include <string.h>

#include "xmem.h"

/* Callback used by curl */
static size_t xml_callback(char *ptr, size_t size, size_t nmemb, void *data)
{
        size_t realsize = size * nmemb;
        struct xml_memory *mem = (struct xml_memory*)data;

        if (!mem)
                return 0;

        if (mem->string)
                free(mem->string);

        mem->string = xmalloc(realsize + 1);

        if (mem->string == NULL) {
                return 0;
        }

        memcpy(mem->string, ptr, realsize);
        mem->size = realsize;

        return realsize;
}

/* Cleans up and frees xml_memory */
void vxml_cleanup(struct xml_memory *mem)
{
        xmlCleanupParser();

        if (mem) {
                if (mem->string)
                        free(mem->string);

                curl_easy_cleanup(mem->curl);
                curl_global_cleanup();
                free(mem);
        }
}

/* Downloads anything with curl and places it into memory */
int vxml_download(struct xml_memory *mem, char *url)
{
        CURLcode res;

        curl_easy_setopt(mem->curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(mem->curl, CURLOPT_WRITEFUNCTION, xml_callback);
        curl_easy_setopt(mem->curl, CURLOPT_WRITEDATA, mem);
        curl_easy_setopt(mem->curl, CURLOPT_URL, url);


        res = curl_easy_perform(mem->curl);

        if (res != CURLE_OK) {
                debug_print(curl_easy_strerror(res));
                return -1;
        }
        
        return 0;
}

/* Initializes the xml_memory struct */
struct xml_memory* vxml_init()
{
        struct xml_memory *mem = xmalloc(sizeof(struct xml_memory));
        
        mem->curl = curl_easy_init();
        xmlInitParser();

        mem->string = NULL;
        mem->size = 0;

        if (!mem->curl) {
                debug_print("Couldnt initiate curl exiting...");
                exit(EXIT_FAILURE);;
        }

        return mem;
}

/* Parses the memory of the xml and returns whats in the XPath */
char *vxml_parse(struct xml_memory *mem, char *xpath)
{
        xmlDocPtr doc;
        xmlXPathContextPtr xpathCtx; 
        xmlXPathObjectPtr xpathObj; 
        xmlNodePtr node;
        char *ret;

        doc = xmlReadMemory(mem->string, mem->size, "none.xml", NULL, 0);

        if (doc == NULL) {
                debug_print("xmlReadMemory failed.");
                return NULL;
        }

        xpathCtx = xmlXPathNewContext(doc);

        if (xpathCtx == NULL) {
                debug_print("xmlXPathNewContext failed.");
                xmlFreeDoc(doc);
                return NULL;
        }

        xpathObj = xmlXPathEvalExpression((xmlChar*)xpath, xpathCtx);

        if (xpathObj == NULL) {
                debug_print("xmlXpathEvalExpression failed.");
                xmlXPathFreeContext(xpathCtx); 
                xmlFreeDoc(doc);
                return NULL;
        }
        
        xmlXPathFreeContext(xpathCtx); 

        if (xpathObj->nodesetval->nodeNr < 1)
        {
                debug_print("no nodes in current nodeset.");
                xmlXPathFreeObject(xpathObj);
                xmlFreeDoc(doc);
                return NULL;
        }

        node = xpathObj->nodesetval->nodeTab[0];


        ret = (char*)xmlNodeListGetString(doc,node->xmlChildrenNode, 1);
        
        xmlXPathFreeObject(xpathObj);
        xmlFreeDoc(doc);
        
        return ret;
}
