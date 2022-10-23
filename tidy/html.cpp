#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <gumbo.h>
#include "pugixml.hpp"

using namespace std;
using namespace pugi;

// Read the broken html file.
// It leaves lots and lots of html out.
// It leaks no memory.
string attempt_one (string contents)
{
    string result {};
    // Parse HTML and create a DOM tree.
    int options = HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING;
    xmlDoc* xmldoc = htmlReadDoc ((xmlChar*)contents.c_str(), NULL, NULL, options);
    if (xmldoc) {
        xmlChar *s {};
        int size {0};
        xmlDocDumpMemory(xmldoc, &s, &size);
        result = (char *)s;
        xmlFree(s);
        xmlFreeDoc(xmldoc);
    }
    // Done.
    return result;
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

// Read the broken html file.
// It leaves lots and lots of html out.
// It leaks lots of memory.
string attempt_two (string contents)
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
    //walkTree(xmlDocGetRootElement(parser->myDoc));

    // Get the text representation of the html
    string result {};
    if (parser->myDoc) {
        xmlChar *s {};
        int size {0};
        xmlDocDumpMemory(parser->myDoc, &s, &size);
        result = (char *)s;
        xmlFree(s);
        xmlFreeDoc(parser->myDoc);
    }

    // Done
    return result;
}

// Read the broken html file.
// It properly fixes the broken html.
// It leaks lots and lots of memory.
string attempt_three (string contents)
{
    // Create a parser context.
    htmlParserCtxtPtr parser = htmlCreatePushParserCtxt (NULL, NULL, NULL, 0, NULL, XML_CHAR_ENCODING_UTF8);
    
    // Set relevant options on that parser context.
    htmlCtxtUseOptions(parser, HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);
    
    // Start to parse an (X)HTML document.
    // char * data : buffer containing part of the web page
    // int len : number of bytes in data
    // Last argument is 0 if the web page isn't complete, and 1 for the final call.
    htmlParseChunk(parser, contents.c_str(), contents.size(), 0);
    //htmlParseChunk(parser, "", 0, 0);
    // Finalize parsing the chunk.
    htmlParseChunk(parser, NULL, 0, 1);
    
    string result {};
    if (parser->myDoc) {
        xmlChar *s;
        int size;
        xmlDocDumpMemory(parser->myDoc, &s, &size);
        result = (char *)s;
        xmlFree(s);
        xmlFree (parser->myDoc);
    }
    
    if (parser) xmlFree (parser);
    
    return result;
}


static std::string nonbreaking_inline  = "|a|abbr|acronym|b|bdo|big|cite|code|dfn|em|font|i|img|kbd|nobr|s|small|span|strike|strong|sub|sup|tt|";
static std::string empty_tags          = "|area|base|basefont|bgsound|br|command|col|embed|event-source|frame|hr|image|img|input|keygen|link|menuitem|meta|param|source|spacer|track|wbr|";
static std::string preserve_whitespace = "|pre|textarea|script|style|";
static std::string special_handling    = "|html|body|";
static std::string no_entity_sub       = "|script|style|";
static std::string treat_like_inline   = "|p|";

static inline void rtrim(std::string &s)
{
    s.erase(s.find_last_not_of(" \n\r\t")+1);
}


static inline void ltrim(std::string &s)
{
    s.erase(0,s.find_first_not_of(" \n\r\t"));
}


static void replace_all(std::string &s, const char * s1, const char * s2)
{
    std::string t1(s1);
    size_t len = t1.length();
    size_t pos = s.find(t1);
    while (pos != std::string::npos) {
        s.replace(pos, len, s2);
        pos = s.find(t1, pos + len);
    }
}


static std::string substitute_xml_entities_into_text(const std::string &text)
{
    std::string result = text;
    // replacing & must come first
    replace_all(result, "&", "&amp;");
    replace_all(result, "<", "&lt;");
    replace_all(result, ">", "&gt;");
    return result;
}


static std::string substitute_xml_entities_into_attributes(char quote, const std::string &text)
{
    std::string result = substitute_xml_entities_into_text(text);
    if (quote == '"') {
        replace_all(result,"\"","&quot;");
    }
    else if (quote == '\'') {
        replace_all(result,"'","&apos;");
    }
    return result;
}


static std::string handle_unknown_tag(GumboStringPiece *text)
{
    std::string tagname = "";
    if (text->data == NULL) {
        return tagname;
    }
    // work with copy GumboStringPiece to prevent asserts
    // if try to read same unknown tag name more than once
    GumboStringPiece gsp = *text;
    gumbo_tag_from_original_text(&gsp);
    tagname = std::string(gsp.data, gsp.length);
    return tagname;
}


static std::string get_tag_name(GumboNode *node)
{
    std::string tagname;
    // work around lack of proper name for document node
    if (node->type == GUMBO_NODE_DOCUMENT) {
        tagname = "document";
    } else {
        tagname = gumbo_normalized_tagname(node->v.element.tag);
    }
    if (tagname.empty()) {
        tagname = handle_unknown_tag(&node->v.element.original_tag);
    }
    return tagname;
}


static std::string build_doctype(GumboNode *node)
{
    std::string results = "";
    if (node->v.document.has_doctype) {
        results.append("<!DOCTYPE ");
        results.append(node->v.document.name);
        std::string pi(node->v.document.public_identifier);
        if ((node->v.document.public_identifier != NULL) && !pi.empty() ) {
            results.append(" PUBLIC \"");
            results.append(node->v.document.public_identifier);
            results.append("\" \"");
            results.append(node->v.document.system_identifier);
            results.append("\"");
        }
        results.append(">\n");
    }
    return results;
}


static std::string build_attributes(GumboAttribute * at, bool no_entities)
{
    std::string atts = "";
    atts.append(" ");
    atts.append(at->name);
    
    // how do we want to handle attributes with empty values
    // <input type="checkbox" checked />  or <input type="checkbox" checked="" />
    
    if ( (!std::string(at->value).empty())   ||
        (at->original_value.data[0] == '"') ||
        (at->original_value.data[0] == '\'') ) {
        
        // determine original quote character used if it exists
        char quote = at->original_value.data[0];
        std::string qs = "";
        if (quote == '\'') qs = std::string("'");
        if (quote == '"') qs = std::string("\"");
        
        atts.append("=");
        
        atts.append(qs);
        
        if (no_entities) {
            atts.append(at->value);
        } else {
            atts.append(substitute_xml_entities_into_attributes(quote, std::string(at->value)));
        }
        
        atts.append(qs);
    }
    return atts;
}


// forward declaration

static std::string prettyprint(GumboNode*, int lvl, const std::string indent_chars);


// prettyprint children of a node
// may be invoked recursively

static std::string prettyprint_contents(GumboNode* node, int lvl, const std::string indent_chars) {
    
    std::string contents        = "";
    std::string tagname         = get_tag_name(node);
    std::string key             = "|" + tagname + "|";
    bool no_entity_substitution = no_entity_sub.find(key) != std::string::npos;
    bool keep_whitespace        = preserve_whitespace.find(key) != std::string::npos;
    bool is_inline              = nonbreaking_inline.find(key) != std::string::npos;
    bool pp_okay                = !is_inline && !keep_whitespace;
    
    GumboVector* children = &node->v.element.children;
    
    for (unsigned int i = 0; i < children->length; ++i) {
        GumboNode* child = static_cast<GumboNode*> (children->data[i]);
        
        if (child->type == GUMBO_NODE_TEXT) {
            
            std::string val;
            
            if (no_entity_substitution) {
                val = std::string(child->v.text.text);
            } else {
                val = substitute_xml_entities_into_text(std::string(child->v.text.text));
            }
            
            if (pp_okay) rtrim(val);
            
            if (pp_okay && (contents.length() == 0)) {
                // add required indentation
                char c = indent_chars.at(0);
                int n  = indent_chars.length();
                contents.append(std::string((lvl-1)*n,c));
            }
            
            contents.append(val);
            
            
        } else if ((child->type == GUMBO_NODE_ELEMENT) || (child->type == GUMBO_NODE_TEMPLATE)) {
            
            std::string val = prettyprint(child, lvl, indent_chars);
            
            // remove any indentation if this child is inline and not first child
            std::string childname = get_tag_name(child);
            std::string childkey = "|" + childname + "|";
            if ((nonbreaking_inline.find(childkey) != std::string::npos) && (contents.length() > 0)) {
                ltrim(val);
            }
            
            contents.append(val);
            
        } else if (child->type == GUMBO_NODE_WHITESPACE) {
            
            if (keep_whitespace || is_inline) {
                contents.append(std::string(child->v.text.text));
            }
            
        } else if (child->type != GUMBO_NODE_COMMENT) {
            
            // Does this actually exist: (child->type == GUMBO_NODE_CDATA)
            fprintf(stderr, "unknown element of type: %d\n", child->type);
            
        }
        
    }
    
    return contents;
}


// prettyprint a GumboNode back to html/xhtml
// may be invoked recursively

static std::string prettyprint(GumboNode* node, int lvl, const std::string indent_chars)
{
    // Special case: The document node.
    if (node->type == GUMBO_NODE_DOCUMENT) {
        std::string results {build_doctype(node)};
        results.append(prettyprint_contents (node, lvl + 1, indent_chars));
        return results;
    }
    
    std::string close {};
    std::string closeTag {};
    std::string atts {};
    std::string tagname {get_tag_name(node)};
    std::string key {"|" + tagname + "|"};
    bool need_special_handling {special_handling.find(key) != std::string::npos};
    bool is_empty_tag {empty_tags.find(key) != std::string::npos};
    bool no_entity_substitution {no_entity_sub.find(key) != std::string::npos};
    bool keep_whitespace {preserve_whitespace.find(key) != std::string::npos};
    bool is_inline {nonbreaking_inline.find(key) != std::string::npos};
    bool inline_like {treat_like_inline.find(key) != std::string::npos};
    bool pp_okay {!is_inline && !keep_whitespace};
    char c {indent_chars.at(0)};
    int n {static_cast<int>(indent_chars.length())};
    
    // Build the attr string.
    const GumboVector * attribs {&node->v.element.attributes};
    for (unsigned int i = 0; i < attribs->length; ++i) {
        GumboAttribute * at {static_cast<GumboAttribute*>(attribs->data[i])};
        atts.append (build_attributes (at, no_entity_substitution));
    }
    
    // Determine the closing tag type.
    if (is_empty_tag) {
        close = "/";
    } else {
        closeTag = "</" + tagname + ">";
    }
    
    std::string indent_space {std::string ((lvl-1)*n,c)};
    
    // Pretty print the contents.
    std::string contents {prettyprint_contents(node, lvl+1, indent_chars)};
    
    if (need_special_handling) {
        rtrim (contents);
        contents.append ("\n");
    }
    
    char last_char = ' ';
    if (!contents.empty()) {
        last_char = contents.at (contents.length() - 1);
    }
    
    // Build the results.
    std::string results;
    if (pp_okay) {
        results.append(indent_space);
    }
    results.append("<"+tagname+atts+close+">");
    if (pp_okay && !inline_like) {
        results.append("\n");
    }
    if (inline_like) {
        ltrim(contents);
    }
    results.append(contents);
    if (pp_okay && !contents.empty() && (last_char != '\n') && (!inline_like)) {
        results.append("\n");
    }
    if (pp_okay && !inline_like && !closeTag.empty()) {
        results.append(indent_space);
    }
    results.append(closeTag);
    if (pp_okay && !closeTag.empty()) {
        results.append("\n");
    }
    
    return results;
}


string attempt_four (string contents)
{
    // https://github.com/google/gumbo-parser
    GumboOptions options = kGumboDefaultOptions;
    GumboOutput* output = gumbo_parse_with_options(&options, contents.data(), contents.length());
    std::string indent_chars {" "};
    contents = prettyprint(output->document, 0, indent_chars);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return contents;
}


string attempt_five (string contents)
{
    // Split the contents up into lines.
    vector <string> lines;
    {
        istringstream iss (contents);
        for (string token; getline (iss, token, '\n'); ) lines.push_back (move (token));
    }
    
    contents.clear();

    int line_count {0};
    for (const auto & line : lines) {
        if (!line_count) {
            size_t pos = line.find (R"(class="verse )");
            if (pos == string::npos) continue;
            pos = line.find (" verse-2 ");
            if (pos == string::npos) continue;
            line_count++;
        }
        if (line_count) {
            if (line_count < 100) {
                line_count++;
                contents.append (line);
                contents.append ("\n");
            } else {
                line_count = 0;
            }
        }
    }
    
    return contents;
}


void parse_one (string contents)
{
    // Parse the html into a DOM.
    xml_document document {};
    document.load_string (contents.c_str());

    // Example verse container within the XML:
    // Verse 0:
    // <p class="summary">...</>
    // Other verses:
    // <div class="verse verse-1 active size-change bold-change cursive-change align-change">...
    int verse {2};
    string selector;
    if (verse != 0) selector = "//div[contains(@class,'verse-" + to_string (verse) + " ')]";
    else selector = "//p[@class='summary']";
    xpath_node xpathnode = document.select_node(selector.c_str());
    xml_node div_node = xpathnode.node();
    
    // Print it to see what it has become.
    {
        stringstream output {};
//        document.print (output, "", format_raw);
        div_node.print (output, "", format_raw);
        cout << output.str () << endl;
    }
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    // Load text into memory.
    string path {"original.html"};
    ifstream ifs (path);
    string contents ((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>() ) );
    
    // Parse broken HTML.
    //contents = attempt_one (contents);
    //contents = attempt_two (contents);
    //contents = attempt_three (contents);
    //contents = attempt_four (contents);
    contents = attempt_five (contents);
    parse_one (contents);

    // Save the tidied text to file.
    path = "output.html";
    ofstream file;
    file.open(path, ios::binary | ios::trunc);
    file << contents;
    file.close ();

    // Ready.
    return EXIT_SUCCESS;
}
