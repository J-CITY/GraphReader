#ifndef GRAPH_INCLUDED
#define GRAPH_INCLUDED

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include "tinyxml.h"

class Node {
public:

    std::string id;
    //double lon;
    //double lan;

    std::map<std::string, std::string> params;

    std::map<std::string, std::string> tags;

    std::vector<std::pair<std::string, double>> neighbors;//node, cost
};

struct Bound {
    double minlat = 0;
    double minlon = 0;
    double maxlat = 0;
    double maxlon = 0;
};

class Graph {
public:
    Bound bound;

    Grap(){}

    void ReadGraphFromXML(std::string input) {
        TiXmlDocument doc(input.c_str());
        //bool loadOkay = doc.LoadFile();
        if(doc.LoadFile()) {
            TiXmlHandle hDoc(&doc);
            TiXmlElement *pRoot;
            pRoot = doc.FirstChildElement("osm");
            if(pRoot) {
                TiXmlElement *pBounds = pRoot->FirstChildElement("bounds");
                ReadXmlBounds(pBounds);

                TiXmlElement *pNode = pBounds;
                while (pNode =  pNode->NextSiblingElement("node")) {
                    ReadXmlNode(pNode);
                }

                TiXmlElement *pWay = pBounds;
                while (pWay = pWay->NextSiblingElement("way")) {
                    ReadXmlWay(pWay);
                }
            }
        } else {
            std::cout << "Could not load XML File.";
            return;
        }
    }


private:
    void ReadXmlBounds(TiXmlElement* element) {
        if (element) {
            std::cout << "BOUNDS: " << element->Attribute("minlat") << std::endl;
            std::cout << "BOUNDS: "  << element->Attribute("minlon") << std::endl;
            std::cout << "BOUNDS: "  << element->Attribute("maxlat") << std::endl;
            std::cout << "BOUNDS: "  << element->Attribute("maxlon") << std::endl;

            bound.minlat = atof(element->Attribute("minlat"));
            bound.minlon = atof(element->Attribute("minlon"));
            bound.maxlat = atof(element->Attribute("maxlat"));
            bound.maxlon = atof(element->Attribute("maxlon"));
        }
    }

    void ReadXmlNode(TiXmlElement* element) {
        if (element) {
            Node n;
            n.id = element->Attribute("id");
            std::cout << "NODE ID: " << n.id << std::endl;
            n.params["lat"] = element->Attribute("lat");
            n.params["lon"] = element->Attribute("lon");
            n.params["user"] = element->Attribute("user");
            n.params["uid"] = element->Attribute("uid");
            n.params["visible"] = element->Attribute("visible");
            n.params["version"] = element->Attribute("version");
            n.params["changeset"] = element->Attribute("changeset");
            n.params["timestamp"] = element->Attribute("timestamp");

            TiXmlElement *pTag = element->FirstChildElement("tag");
            if (pTag) {
                ReadXmlTags(pTag, n);
            }

            nodes[n.id] = n;
        }
    }

    void ReadXmlTags(TiXmlElement* element, Node &n) {
        do {
            std::cout << "TAG: " << element->Attribute("k")
                << " : " << element->Attribute("v") << std::endl;
            n.tags[element->Attribute("k")] = element->Attribute("v");
        } while(element = element->NextSiblingElement("tag"));
    }

    void ReadXmlWay(TiXmlElement* element) {
        ///...
        int i = 0;
        std::string first;
        element = element->FirstChildElement("nd");
        do {
            std::cout << "ND: " << element->Attribute("ref") << std::endl;

            if (i == 0) {
                first = element->Attribute("ref");
            } else {
                int cost = 0;
                nodes[first].neighbors.push_back(std::make_pair(element->Attribute("ref"), cost));
                first = element->Attribute("ref");
            }
            ++i;
        } while(element = element->NextSiblingElement("nd"));
    }

    std::map<std::string, Node> nodes;
};


#endif // GRAPH_INCLUDED
