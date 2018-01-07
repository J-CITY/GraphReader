#ifndef GRAPH_INCLUDED
#define GRAPH_INCLUDED

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <cmath>
#include "tinyxml.h"

/*
Read graph with tinyxml lib.

XML to Graph(V, E, C)
V - vertex
E - edges
C - cost
*/

const char* BOUND_MIN_LAT="minlat";
const char* BOUND_MIN_LON="minlon";
const char* BOUND_MAX_LAT="maxlat";
const char* BOUND_MAX_LON="maxlon";

const char* OSM="osm";
const char* BOUNDS="bounds";
const char* NODE="node";
const char* WAY="way";
const char* TAG="tag";

const char* PARAM_ID="id";
const char* PARAM_LAT="lat";
const char* PARAM_LON="lon";
const char* PARAM_USER="user";
const char* PARAM_UID="uid";
const char* PARAM_VISIBLE="visible";
const char* PARAM_VERSION="version";
const char* PARAM_CHANGESET="changeset";
const char* PARAM_TIMESTAMP="timestamp";

const char* TAG_KEY="k";
const char* TAG_VALUE="v";

const char* ND="nd";
const char* ND_REF="ref";

/*graph node*/
class Node {
public:
    /*unique id*/
    std::string id;
    /*node params like lat., lon., timestamp, etc*/
    std::map<std::string, std::string> params;
    /*some node tags <key, value>*/
    std::map<std::string, std::string> tags;
    /*neighbors id`s and way cost to neighbor*/
    std::vector<std::pair<std::string, double>> neighbors;//node, cost
};

/*geographical area of the graph*/
struct Bound {
    double minlat = 0;
    double minlon = 0;
    double maxlat = 0;
    double maxlon = 0;
};

class Graph {
public:
    Bound bound;

    Graph() = default;

    /*Read graph from xml.*/
    void ReadGraphFromXML(std::string input);

private:
    /*Read graph area from xml.*/
    void ReadXmlBounds(TiXmlElement* element);

    /*Read id and params.*/
    void ReadXmlNode(TiXmlElement* element);

    /*Read tags.*/
    void ReadXmlTags(TiXmlElement* element, Node &n);

    /*Read ways.*/
    void ReadXmlWay(TiXmlElement* element);

    /*Graph nodes.*/
    std::map<std::string, Node> nodes;
};

#endif // GRAPH_INCLUDED
