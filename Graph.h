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

const char* const BOUND_MIN_LAT="minlat";
const char* const BOUND_MIN_LON="minlon";
const char* const BOUND_MAX_LAT="maxlat";
const char* const BOUND_MAX_LON="maxlon";

const char* const OSM="osm";
const char* const BOUNDS="bounds";
const char* const NODE="node";
const char* const WAY="way";
const char* const TAG="tag";

const char* const PARAM_ID="id";
const char* const PARAM_LAT="lat";
const char* const PARAM_LON="lon";
const char* const PARAM_USER="user";
const char* const PARAM_UID="uid";
const char* const PARAM_VISIBLE="visible";
const char* const PARAM_VERSION="version";
const char* const PARAM_CHANGESET="changeset";
const char* const PARAM_TIMESTAMP="timestamp";

const char* const TAG_KEY="k";
const char* const TAG_VALUE="v";

const char* const ND="nd";
const char* const ND_REF="ref";

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
