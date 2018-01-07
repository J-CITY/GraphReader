#include "Graph.h"

void Graph::ReadGraphFromXML(std::string input) {
    TiXmlDocument doc(input.c_str());

    if (!doc.LoadFile()) {
        std::cout << "ERROR: Could not load XML file: " + input << std::endl;
        return;
    }

    TiXmlHandle hDoc(&doc);
    TiXmlElement *pRoot;
    pRoot = doc.FirstChildElement(OSM);

    if (pRoot == nullptr) return;

    TiXmlElement *pBounds = pRoot->FirstChildElement(BOUNDS);
    ReadXmlBounds(pBounds);

    TiXmlElement *pElem = pBounds;
    while ((pElem = pElem->NextSiblingElement()) != nullptr) {
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

void Graph::ReadXmlBounds(TiXmlElement* element) {
    if (!element) return;

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

void Graph::ReadXmlNode(TiXmlElement* element) {
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

void Graph::ReadXmlTags(TiXmlElement* element, Node &n) {
    do {
        std::string key = element->Attribute(TAG_KEY) == nullptr ? "" : element->Attribute(TAG_KEY);
        std::string val = element->Attribute(TAG_VALUE) == nullptr ? "" : element->Attribute(TAG_VALUE);
        std::cout << "TAG: " << key << " : " << val << std::endl;
        n.tags[key] = val;
    } while((element = element->NextSiblingElement(TAG)) != nullptr);
}

void Graph::ReadXmlWay(TiXmlElement* element) {
    int i = 0;
    std::string first;
    element = element->FirstChildElement();
    if (strcmp(element->Value(), ND)) {
        while ((element = element->NextSiblingElement()) != nullptr) {
            if (strcmp(element->Value(), ND))
                continue;
            if (!strcmp(element->Value(), ND))
                break;
            return;
        }
    }
    do {
        if (i == 0) {
            first = element->Attribute(ND_REF) == nullptr ? "" : element->Attribute(ND_REF);
            std::cout << "WAY: " << first << std::endl;
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
        double p_a = std::stod(nodes[first].params[PARAM_LAT]);
        double p_b = std::stod(nodes[element->Attribute(ND_REF)].params[PARAM_LAT]);
        double l_a = std::stod(nodes[first].params[PARAM_LON]);
        double l_b = std::stod(nodes[element->Attribute(ND_REF)].params[PARAM_LON]);
        double d = sin(p_a) * sin(p_b) +
                   cos(p_a) * cos(p_b) *
                   cos(l_a - l_b);
        d = acos(d);
        cost = d * 6371;//km
        nodes[first].neighbors.emplace_back(std::make_pair(ref, cost));
        first = ref;
        std::cout << "WAY: " << ref << " " << cost << std::endl;
        ++i;
    } while((element = element->NextSiblingElement(ND)) != nullptr);
}