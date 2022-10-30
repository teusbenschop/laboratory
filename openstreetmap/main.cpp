#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include "pugixml.hpp"

using namespace std;
using namespace pugi;

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


int long2tilex(double lon, int z)
{
    return (int)(floor((lon + 180.0) / 360.0 * (1 << z)));
}

int lat2tiley(double lat, int z)
{
    double latrad = lat * M_PI/180.0;
    return (int)(floor((1.0 - asinh(tan(latrad)) / M_PI) / 2.0 * (1 << z)));
}

double tilex2long(int x, int z)
{
    return x / (double)(1 << z) * 360.0 - 180;
}

double tiley2lat(int y, int z)
{
    double n = M_PI - 2.0 * M_PI * y / (double)(1 << z);
    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}



int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    cout << "Generating a html file with all OpenStreetMap tiles for Sweden" << endl;
    cout << "Open this html file in the browser to check the server" << endl;
    cout << "Crawl and download this html file to fill the cache on the server" << endl;

    // Information about pre-render all the possible tiles:
    // https://akaritech.com/prerendering-map-tiles-in-z-x-y-format/
    // Routines to calculate the z/x/y is here:
    // https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames

    
    // A bounding box (usually shortened to bbox) is an area
    // defined by two longitudes and two latitudes, where:
    //   Latitude is a decimal number between -90.0 and 90.0.
    //   Longitude is a decimal number between -180.0 and 180.0.
    // They usually follow the standard format of:
    //  bbox = left,bottom,right,top
    //  bbox = min Longitude , min Latitude , max Longitude , max Latitude
    // A good web site to find the bounding box for an area is:
    // https://boundingbox.klokantech.com/
    // Set your output to CSV.

    string base {"http://debian11.local/"};

    // Generate the html skeleton with styles to ensure a scrollable map.
    xml_document document {};
    xml_node html_node = document.append_child("html");
    xml_node head_node = html_node.append_child("head");
    {
        xml_node base_node = head_node.append_child ("base");
        base_node.append_attribute("href") = base.c_str();
    }
    xml_node body_node = html_node.append_child("body");
    body_node.append_attribute("style") = "overflow:scroll;";
    
    // Iterate over a couple of zoom levels.
    for (int zoom {0}; zoom <= 8; zoom++)
    {
        xml_node h1_node = body_node.append_child ("h1");
        string h1_text = "OpenStreetMap tiles at zoom level " + to_string (zoom);
        h1_node.text().set(h1_text.c_str());
        
        double minimum_longitude {4.00};
        double minimum_latitude {70.00};
        int left_tile_x = long2tilex (minimum_longitude, zoom);
        int top_tile_y = lat2tiley (minimum_latitude, zoom);
        double maximum_longitude {26.00};
        double maximum_latitude {54.00};
        int right_tile_x = long2tilex (maximum_longitude, zoom);
        int bottom_tile_y = lat2tiley (maximum_latitude, zoom);
        
        cout << "Processing zoom level " << zoom << " with " << pow ((right_tile_x - left_tile_x), 2) << " tiles" << endl;

        for (int tile_y {top_tile_y}; tile_y <= bottom_tile_y; tile_y++)
        {
            xml_node div_node = body_node.append_child ("div");
            div_node.append_attribute ("style") = "display:flex; white-space:nowrap;";
            for (int tile_x {left_tile_x}; tile_x <= right_tile_x; tile_x++)
            {
                xml_node img_node = div_node.append_child ("img");
                string url = "hot/" + to_string (zoom) + "/" + to_string (tile_x) + "/" + to_string (tile_y) + ".png";
                img_node.append_attribute("src") = url.c_str();
                img_node.append_attribute("width") = "256px";
                img_node.append_attribute("height") = "256px";
            }
        }

    }
    
    
    

    // Save the html to file.
    stringstream html {};
    document.print(html, "", format_raw);
    string path {"page.html"};
    cout << "Saving page to " << path << endl;
    ofstream file {};
    file.open(path, ios::binary | ios::trunc);
    file << html.str();
    file.close ();

    // Ready.
    return EXIT_SUCCESS;
}
