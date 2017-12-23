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

    Graph() {}

    /*Read graph from xml.*/
    void ReadGraphFromXML(std::string input) {
        TiXmlDocument doc(input.c_str());
        //bool loadOkay = doc.LoadFile();
        if(doc.LoadFile()) {
            TiXmlHandle hDoc(&doc);
            TiXmlElement *pRoot;
            pRoot = doc.FirstChildElement(OSM);
            if(pRoot) {
                TiXmlElement *pBounds = pRoot->FirstChildElement(BOUNDS);
                ReadXmlBounds(pBounds);

                TiXmlElement *pElem = pBounds;
                while (pElem =  pElem->NextSiblingElement()) {
                    if (!strcmp(pElem->Value(), NODE)) {
                        ReadXmlNode(pElem);
                        continue;
                    }
                    if (!strcmp(pElem->Value(), WAY)) {
                        ReadXmlWay(pElem);
                        continue;
                    }
                }
            }
        } else {
            std::cout << "ERROR: Could not load XML file: " + input << std::endl;
            return;
        }
    }

private:
    /*Read graph area from xml.*/
    void ReadXmlBounds(TiXmlElement* element) {
        if (element) {
            bound.minlat = atof(element->Attribute(BOUND_MIN_LAT) == nullptr ? "0" : element->Attribute(BOUND_MIN_LAT));
            bound.minlon = atof(element->Attribute(BOUND_MIN_LON) == nullptr ? "0" : element->Attribute(BOUND_MIN_LON));
            bound.maxlat = atof(element->Attribute(BOUND_MAX_LAT) == nullptr ? "0" : element->Attribute(BOUND_MAX_LAT));
            bound.maxlon = atof(element->Attribute(BOUND_MAX_LON) == nullptr ? "0" : element->Attribute(BOUND_MIN_LON));
            std::cout << "BOUNDS: \n";
            std::cout << "MIN LAT: " << bound.minlat << std::endl;
            std::cout << "MIN LON: " << bound.minlon << std::endl;
            std::cout << "MAX LAT: " << bound.maxlat << std::endl;
            std::cout << "MAX LON: " << bound.maxlon << std::endl;
        }
    }

    /*Read id and params.*/
    void ReadXmlNode(TiXmlElement* element) {
        if (element) {
            Node n;
            n.id = element->Attribute(PARAM_ID) == nullptr ? "" : element->Attribute(PARAM_ID);
            std::cout << "NODE ID: " << n.id << std::endl;
            n.params[PARAM_LAT] = element->Attribute(PARAM_LAT) == nullptr ? "" : element->Attribute(PARAM_LAT);
            n.params[PARAM_LON] = element->Attribute(PARAM_LON) == nullptr ? "" : element->Attribute(PARAM_LON);
            n.params[PARAM_USER] = element->Attribute(PARAM_USER) == nullptr ? "" : element->Attribute(PARAM_USER);
            n.params[PARAM_UID] = element->Attribute(PARAM_UID) == nullptr ? "" : element->Attribute(PARAM_UID);
            n.params[PARAM_VISIBLE] = element->Attribute(PARAM_VISIBLE) == nullptr ? "" : element->Attribute(PARAM_VISIBLE);
            n.params[PARAM_VERSION] = element->Attribute(PARAM_VERSION) == nullptr ? "" : element->Attribute(PARAM_VERSION);
            n.params[PARAM_CHANGESET] = element->Attribute(PARAM_CHANGESET) == nullptr ? "" : element->Attribute(PARAM_CHANGESET);
            n.params[PARAM_TIMESTAMP] = element->Attribute(PARAM_TIMESTAMP) == nullptr ? "" : element->Attribute(PARAM_TIMESTAMP);

            TiXmlElement *pTag = element->FirstChildElement(TAG);
            if (pTag) {
                ReadXmlTags(pTag, n);
            }

            nodes[n.id] = n;
        }
    }

    /*Read tags.*/
    void ReadXmlTags(TiXmlElement* element, Node &n) {
        do {
            std::string key = element->Attribute(TAG_KEY) == nullptr ? "" : element->Attribute(TAG_KEY);
            std::string val = element->Attribute(TAG_VALUE) == nullptr ? "" : element->Attribute(TAG_VALUE);
            std::cout << "TAG: " << key << " : " << val << std::endl;
            n.tags[key] = val;
        } while(element = element->NextSiblingElement(TAG));
    }

    /*Read ways.*/
    void ReadXmlWay(TiXmlElement* element) {
        ///...
        int i = 0;
        std::string first;
        element = element->FirstChildElement();
        if (strcmp(element->Value(), ND)) {
            while (element = element->NextSiblingElement()) {
                if (element != nullptr && strcmp(element->Value(), ND)) {
                    continue;
                }
                if (element != nullptr && !strcmp(element->Value(), ND)) {
                    break;
                }
                return;
            }
        }
        do {
            if (i == 0) {
                first = element->Attribute(ND_REF) == nullptr ? "" : element->Attribute(ND_REF);
                std::cout << "WAY: " << first << std::endl;
            } else {
                double cost = 0;
                ///Average distance
                ///L = d*R
                ///R = 6371km
                ///cos(d) = sin(p_a)*sin(p_b) + cos(p_a)*cos(p_b)*cos(l_a-l_b)
                ///l - lon, p - lat
                const char* ref = element->Attribute(ND_REF) == nullptr ? "" : element->Attribute(ND_REF);
                double p_a = std::stod(nodes[first].params[PARAM_LAT]);
                double p_b = std::stod(nodes[element->Attribute(ND_REF)].params[PARAM_LAT]);
                double l_a = std::stod(nodes[first].params[PARAM_LON]);
                double l_b = std::stod(nodes[element->Attribute(ND_REF)].params[PARAM_LON]);
                double d = sin(p_a) * sin(p_b) +
                    cos(p_a) * cos(p_b) *
                    cos(l_a - l_b);
                d = acos(d);
                cost = d * 6371;//km
                nodes[first].neighbors.push_back(std::make_pair(ref, cost));
                first = ref;
                std::cout << "WAY: " << ref << " " << cost << std::endl;
            }
            ++i;
        } while(element = element->NextSiblingElement(ND));
    }

    /*Graph nodes.*/
    std::map<std::string, Node> nodes;
};

#endif // GRAPH_INCLUDED
