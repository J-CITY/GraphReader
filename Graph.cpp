#include "Graph.h"
#include <utility>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

void Graph::clear() {
    this->bound.maxlat =
    this->bound.maxlon =
    this->bound.minlat =
    this->bound.minlon = 0;
    this->nodes.clear();
}

void Graph::ReadGraphFromXML(std::string input) {
    //Setup TinyXML parser
    TiXmlDocument doc(input.c_str());

    if (!doc.LoadFile()) {
        std::cout << "ERROR: Could not load XML file: " + input << std::endl;
        return;
    }

    TiXmlHandle hDoc(&doc);

    //Start "recording" data
    TiXmlElement *pRoot = doc.FirstChildElement(OSM);

    if (pRoot == nullptr) return;

    TiXmlElement *pElem = pRoot->FirstChildElement();

    if (pElem == nullptr) return;

    //Read info about bounds (if it exists)
    if (!strcmp(pElem->Value(), BOUNDS)) {
        ReadXmlBounds(pElem);
        pElem = pElem->NextSiblingElement();
        if (pElem == nullptr) return;
    }

    //Read info about ways & nodes
    do {
        if (!strcmp(pElem->Value(), NODE)) {
            ReadXmlNode(pElem);
            continue;
        }
        if (!strcmp(pElem->Value(), WAY))
            ReadXmlWay(pElem);
    } while ((pElem = pElem->NextSiblingElement()) != nullptr);
}

void Graph::ReadXmlBounds(TiXmlElement* element) {
    if (!element) return;

    bound.minlat = atof(element->Attribute(BOUND_MIN_LAT) == nullptr ? "0" : element->Attribute(BOUND_MIN_LAT));
    bound.minlon = atof(element->Attribute(BOUND_MIN_LON) == nullptr ? "0" : element->Attribute(BOUND_MIN_LON));
    bound.maxlat = atof(element->Attribute(BOUND_MAX_LAT) == nullptr ? "0" : element->Attribute(BOUND_MAX_LAT));
    bound.maxlon = atof(element->Attribute(BOUND_MAX_LON) == nullptr ? "0" : element->Attribute(BOUND_MIN_LON));
    if (__PRINT == __PRINT_CONSOLE) {
        std::cout << "BOUNDS: \n";
        std::cout << "\tMIN LAT: " << bound.minlat << std::endl;
        std::cout << "\tMIN LON: " << bound.minlon << std::endl;
        std::cout << "\tMAX LAT: " << bound.maxlat << std::endl;
        std::cout << "\tMAX LON: " << bound.maxlon << std::endl;
    }

}

void Graph::ReadXmlNode(TiXmlElement* element) {
    if (element != nullptr) {
        Node n;
        n.id = element->Attribute(PARAM_ID) == nullptr ? "" : element->Attribute(PARAM_ID);
        if (__PRINT == __PRINT_CONSOLE) {
            std::cout << "NODE ID: " << n.id << std::endl;
        }
        const char* buff;

        buff = element->Attribute(PARAM_LAT);
        if (buff != nullptr) {
            n.params[PARAM_LAT] = buff;
            if (__PRINT == __PRINT_CONSOLE) {
                std::cout << "\tLAT: " << buff << std::endl;
            }
        } else n.params[PARAM_LAT] = "";

        buff = element->Attribute(PARAM_LON);
        if (buff != nullptr) {
            n.params[PARAM_LON] = buff;
            if (__PRINT == __PRINT_CONSOLE) {
                std::cout << "\tLON: " << buff << std::endl;
            }
        } else n.params[PARAM_LON] = "";

        buff = element->Attribute(PARAM_USER);
        if (buff != nullptr) {
            n.params[PARAM_USER] = buff;
            if (__PRINT == __PRINT_CONSOLE) {
                std::cout << "\tUSER: " << buff << std::endl;
            }
        } else n.params[PARAM_USER] = "";

        buff = element->Attribute(PARAM_UID);
        if (buff != nullptr) {
            n.params[PARAM_UID] = buff;
            if (__PRINT == __PRINT_CONSOLE) {
                std::cout << "\tUID: " << buff << std::endl;
            }
        } else n.params[PARAM_UID] = "";

        buff = element->Attribute(PARAM_VISIBLE);
        if (buff != nullptr) {
            n.params[PARAM_VISIBLE] = buff;
            if (__PRINT == __PRINT_CONSOLE) {
                std::cout << "\tVISIBLE: " << buff << std::endl;
            }
        } else n.params[PARAM_VISIBLE] = "";

        buff = element->Attribute(PARAM_VERSION);
        if (buff != nullptr) {
            n.params[PARAM_VERSION] = buff;
            if (__PRINT == __PRINT_CONSOLE) {
                std::cout << "\tVERSION: " << buff << std::endl;
            }
        } else n.params[PARAM_VERSION] = "";

        buff = element->Attribute(PARAM_CHANGESET);
        if (buff != nullptr) {
            n.params[PARAM_CHANGESET] = buff;
            if (__PRINT == __PRINT_CONSOLE) {
                std::cout << "\tCHANGESET: " << buff << std::endl;
            }
        } else n.params[PARAM_CHANGESET] = "";

        buff = element->Attribute(PARAM_TIMESTAMP);
        if (buff != nullptr) {
            n.params[PARAM_TIMESTAMP] = buff;
            if (__PRINT == __PRINT_CONSOLE) {
                std::cout << "\tTIMESTAMP: " << buff << std::endl;
            }
        } else n.params[PARAM_TIMESTAMP] = "";

        TiXmlElement *pTag = element->FirstChildElement(TAG);

        //Read all tags from node's "children"
        if (pTag != nullptr) ReadXmlTags(pTag, n);

        nodes[n.id] = n;
    }
}

void Graph::ReadXmlTags(TiXmlElement* element, Node &n) {
    do {
        const char* key = element->Attribute(TAG_KEY);
        const char* val = element->Attribute(TAG_VALUE);
        if (__PRINT == __PRINT_CONSOLE) {
            std::cout << "\tTAG: "
                      << (key == nullptr ? "" : key)
                      << " > "
                      << (val == nullptr ? "" : val) << std::endl;
        }
        n.tags[key] = val;
    } while((element = element->NextSiblingElement(TAG)) != nullptr);
}

void Graph::ReadXmlWay(TiXmlElement* element) {
    Way way;
    const char* id = element->Attribute(PARAM_ID);
    if (__PRINT == __PRINT_CONSOLE) {
        std::cout << "WAY: " << (id == nullptr ? "" : id) << std::endl;
    }
    way.id = id;
    int i = 0;
    std::string first;
    element = element->FirstChildElement();

    do {
        //Read WAY TAG
        if (!strcmp(element->Value(), TAG)) {
            const char* key = element->Attribute(TAG_KEY);
            const char* val = element->Attribute(TAG_VALUE);
            if (__PRINT == __PRINT_CONSOLE) {
                std::cout << "\tTAG: "
                          << (key == nullptr ? "" : key)
                          << " > "
                          << (val == nullptr ? "" : val) << std::endl;
            }
            way.nodes.push_back(first);
            continue;
        }
        //Read NODE id & calculate distance
        if (strcmp(element->Value(), ND)) break;

        if (i == 0) {
            first = element->Attribute(ND_REF) == nullptr ? "" : element->Attribute(ND_REF);
            if (__PRINT == __PRINT_CONSOLE) {
                std::cout << "\tNODE: " << first << std::endl;
            }
            ++i;
            continue;
        }
        double cost = 0;
        /** Average distance
         * L = d*R
         * R = 6371km
         * cos(d) = sin(p_a)*sin(p_b) + cos(p_a)*cos(p_b)*cos(l_a-l_b)
         * l - lon, p - lat
         */
        const char* ref = element->Attribute(ND_REF) == nullptr ? "" : element->Attribute(ND_REF);
        way.nodes.push_back(ref);
        double p_a = std::stod(nodes[first].params[PARAM_LAT] == "" ?
                               "-1" : nodes[first].params[PARAM_LAT]);
        double p_b = std::stod(nodes[element->Attribute(ND_REF)].params[PARAM_LAT] == "" ?
                               "-1" : nodes[element->Attribute(ND_REF)].params[PARAM_LAT]);
        double l_a = std::stod(nodes[first].params[PARAM_LON] == "" ?
                               "-1" : nodes[first].params[PARAM_LON]);
        double l_b = std::stod(nodes[element->Attribute(ND_REF)].params[PARAM_LON] == "" ?
                               "-1" : nodes[element->Attribute(ND_REF)].params[PARAM_LON]);
        double d = sin(p_a) * sin(p_b) +
                   cos(p_a) * cos(p_b) *
                   cos(l_a - l_b);
        d = acos(d);
        cost = d * 6371;//km

        nodes[first].neighbors.emplace_back(std::make_pair(ref, cost));
        nodes[ref].neighbors.emplace_back(std::make_pair(first, cost));
        if (__PRINT == __PRINT_CONSOLE) {
            std::cout << "\tNODE: " << ref << std::endl
                      << "\t\tDIST: " << cost << std::endl;
        }

        first = ref;
        ++i;
    } while((element = element->NextSiblingElement()) != nullptr);
    ways.push_back(way);
}


void Graph::SaveToXML(std::string filename) {
    std::ofstream fout(filename);

    fout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    fout << "<osm version=\"0.6\" generator=\"CGImap 0.0.2\">\n";

    fout << "<bounds minlat=\"" << bound.minlat <<
        "\" minlon=\"" << bound.minlon <<
        "\" maxlat=\"" << bound.maxlat <<
        "\" maxlon=\"" << bound.maxlon << "\"/>\n";

    for(auto& item : nodes) {
        fout << "<node " << "id=" << "\"" << item.second.id << "\"";
        for(auto& p : item.second.params) {
            fout << p.first << "=\"" << p.second << "\" ";
        }

        fout << ">\n";
        for(auto& t : item.second.tags) {
            fout << "<tag k=\"" << t.first << "\" v=\"" << t.second << "\"/>\n";
        }
        fout << "</node>\n";
    }

    for (auto i = 0; i < ways.size(); ++i) {
        fout << "<way id=\"" << ways[i].id << "\" >\n";
        for (auto j = 0; j < ways[i].nodes.size(); ++j) {
            fout << "<nd ref=\"" << ways[i].nodes[j] << "\"/>\n";
        }
        fout << "</way>\n";
    }

    fout << "</osm>\n";
    fout.close();
}

/*Save grapx to <node0 node1 cost> format*/
void Graph::SaveToTXT(std::string filename) {
    std::ofstream fout(filename);
    for(auto& item : nodes) {
        for (auto i = 0; i < item.second.neighbors.size(); ++i) {
            fout << item.second.id << " " << item.second.neighbors[i].first
                << " " << item.second.neighbors[i].second << "\n";
        }
    }
    fout.close();
}

void Graph::GenTest(unsigned int nodeSize, std::string filename) {
    srand( time( 0 ) );
    std::ofstream fout(filename);

    fout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    fout << "<osm version=\"0.6\" generator=\"CGImap 0.0.2\">\n";

    fout << "<bounds minlat=\"" << std::to_string(0) <<
        "\" minlon=\"" << std::to_string(0) <<
        "\" maxlat=\"" << std::to_string(nodeSize) <<
        "\" maxlon=\"" << std::to_string(nodeSize) << "\"/>";

    for(auto i =0; i < nodeSize; ++i) {
        fout << "<node " << "id=" << "\"" << i << "\"";

        fout << "lat=\"" << rand() % nodeSize
            << "\" lon=\"" << rand() % nodeSize << "\"";

        fout << ">\n";
        fout << "</node>\n";
    }
    fout << "<way id=\"42\">\n";
    for (int i = 0; i < nodeSize / 2 + rand() % nodeSize; ++i) {
        int n = rand() % nodeSize;
        fout << "<nd ref=\"" << n << "\"/>\n";
    }
    fout << "</way>\n";

    fout << "</osm>\n";
    fout.close();
}
