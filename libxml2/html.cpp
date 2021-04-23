#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void attempt_one (string contents)
{
  // Parse HTML and create a DOM tree.
  int options = HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING;
  xmlDoc* xmldoc = htmlReadDoc ((xmlChar*)contents.c_str(), NULL, NULL, options);
  //cout << xmldoc  << endl;
  if (xmldoc) {
    xmlChar *s;
    int size;
    xmlDocDumpMemory(xmldoc, &s, &size);
    string xml = (char *)s;
    cout << xml << endl;
    xmlFree(s);
  }
  if (xmldoc) xmlFreeDoc(xmldoc);
}

void walkTree(xmlNode * a_node)
{
  xmlNode *cur_node = NULL;
  xmlAttr *cur_attr = NULL;
  for (cur_node = a_node; cur_node; cur_node = cur_node->next)
  {
    // Do something with that node information,
    // like printing the tag's name and attributes
    cout << "Node name " << cur_node->name << endl;
    for (cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next)
    {
      cout << "Attribute " << cur_attr->name << endl;
    }
    walkTree(cur_node->children);
  }
}

void attempt_two (string contents)
{
  // https://stackoverflow.com/questions/10740250/c-cpp-version-of-beautifulsoup-especially-at-handling-malformed-html/10741112
  
  // Create a parser context.
  htmlParserCtxtPtr parser = htmlCreatePushParserCtxt (NULL, NULL, NULL, 0, NULL, XML_CHAR_ENCODING_UTF8);
  
  // Set relevant options on that parser context.
  htmlCtxtUseOptions(parser, HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);

  // Start to parse an (X)HTML document.
  // char * data : buffer containing part of the web page
  // int len : number of bytes in data
  // Last argument is 0 if the web page isn't complete, and 1 for the final call.
  htmlParseChunk(parser, contents.c_str(), contents.size(), 0);
  htmlParseChunk(parser, "", 0, 1);

  // Get the data parsed by walking the XML tree created.
  walkTree(xmlDocGetRootElement(parser->myDoc));
}

void attempt_three (string contents)
{
  // Create a parser context.
  htmlParserCtxtPtr parser = htmlCreatePushParserCtxt (NULL, NULL, NULL, 0, NULL, XML_CHAR_ENCODING_UTF8);
  
  // Set relevant options on that parser context.
  htmlCtxtUseOptions(parser, HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);

  // Start to parse an (X)HTML document.
  // char * data : buffer containing part of the web page
  // int len : number of bytes in data
  // Last argument is 0 if the web page isn't complete, and 1 for the final call.
  htmlParseChunk(parser, contents.c_str(), contents.size(), 1);
  //htmlParseChunk(parser, "", 0, 1);

  if (parser->myDoc) {
    xmlChar *s;
    int size;
    xmlDocDumpMemory(parser->myDoc, &s, &size);
    string xml = (char *)s;
    cout << xml << endl;
    xmlFree(s);
  }
  if (parser) xmlFree (parser);
 
}

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  // Load text into memory.
  string path = "hebrews-10.htms";
  path = "html.txt";
  ifstream ifs (path);
  string contents ((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>() ) );
  // Parse broken HTML.
  //attempt_one (contents);
  //attempt_two (contents);
  attempt_three (contents);
  return 0;
}
