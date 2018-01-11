#include "Graph.h"

void Graph::clear() {
    this->bound.maxlat =
    this->bound.maxlon =
    this->bound.minlat =
    this->bound.minlon = 0;
    this->nodes.clear();
}

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

    TiXmlElement *pElem = pRoot->FirstChildElement();

    if (pElem == nullptr) return;

    if (!strcmp(pElem->Value(), BOUNDS)) {
        ReadXmlBounds(pElem);
        pElem = pElem->NextSiblingElement();
        if (pElem == nullptr) return;
    }

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
    std::cout << "BOUNDS: \n";
    std::cout << "MIN LAT: " << bound.minlat << std::endl;
    std::cout << "MIN LON: " << bound.minlon << std::endl;
    std::cout << "MAX LAT: " << bound.maxlat << std::endl;
    std::cout << "MAX LON: " << bound.maxlon << std::endl;
}

void Graph::ReadXmlNode(TiXmlElement* element) {
    if (element != nullptr) {
        Node n;
        n.id = element->Attribute(PARAM_ID) == nullptr ? "" : element->Attribute(PARAM_ID);
        std::cout << "NODE ID: " << n.id << std::endl;
        const char* buff;

        buff = element->Attribute(PARAM_LAT);
        if (buff != nullptr) {
            n.params[PARAM_LAT] = buff;
            std::cout << "\tLAT: " << buff << std::endl;
        } else n.params[PARAM_LAT] = "";

        buff = element->Attribute(PARAM_LON);
        if (buff != nullptr) {
            n.params[PARAM_LON] = buff;
            std::cout << "\tLON: " << buff << std::endl;
        } else n.params[PARAM_LON] = "";

        buff = element->Attribute(PARAM_USER);
        if (buff != nullptr) {
            n.params[PARAM_USER] = buff;
            std::cout << "\tUSER: " << buff << std::endl;
        } else n.params[PARAM_USER] = "";

        buff = element->Attribute(PARAM_UID);
        if (buff != nullptr) {
            n.params[PARAM_UID] = buff;
            std::cout << "\tUID: " << buff << std::endl;
        } else n.params[PARAM_UID] = "";

        buff = element->Attribute(PARAM_VISIBLE);
        if (buff != nullptr) {
            n.params[PARAM_VISIBLE] = buff;
            std::cout << "\tVISIBLE: " << buff << std::endl;
        } else n.params[PARAM_VISIBLE] = "";

        buff = element->Attribute(PARAM_VERSION);
        if (buff != nullptr) {
            n.params[PARAM_VERSION] = buff;
            std::cout << "\tVERSION: " << buff << std::endl;
        } else n.params[PARAM_VERSION] = "";

        buff = element->Attribute(PARAM_CHANGESET);
        if (buff != nullptr) {
            n.params[PARAM_CHANGESET] = buff;
            std::cout << "\tCHANGESET: " << buff << std::endl;
        } else n.params[PARAM_CHANGESET] = "";

        buff = element->Attribute(PARAM_TIMESTAMP);
        if (buff != nullptr) {
            n.params[PARAM_TIMESTAMP] = buff;
            std::cout << "\tTIMESTAMP: " << buff << std::endl;
        } else n.params[PARAM_TIMESTAMP] = "";

        TiXmlElement *pTag = element->FirstChildElement(TAG);
        if (pTag) {
            ReadXmlTags(pTag, n);
        }

        nodes[n.id] = n;
    }
}

void Graph::ReadXmlTags(TiXmlElement* element, Node &n) {
    do {
        const char* key = element->Attribute(TAG_KEY);
        const char* val = element->Attribute(TAG_VALUE);
        std::cout << "\tTAG: "
                  << (key == nullptr ? "": key)
                  << " > "
                  << (val == nullptr ? "": val) << std::endl;
        n.tags[key] = val;
    } while((element = element->NextSiblingElement(TAG)) != nullptr);
}

void Graph::ReadXmlWay(TiXmlElement* element) {
    const char* id = element->Attribute(PARAM_ID);
    std::cout << "WAY: " << (id == nullptr ? "" : id) << std::endl;

    int i = 0;
    std::string first;
    element = element->FirstChildElement();

    do {
        //Read WAY TAG
        if (!strcmp(element->Value(), TAG)) {
            const char* key = element->Attribute(TAG_KEY);
            const char* val = element->Attribute(TAG_VALUE);
            std::cout << "\tTAG: "
                      << (key == nullptr ? "": key)
                      << " > "
                      << (val == nullptr ? "": val) << std::endl;
            continue;
        }
        //Read NODE id & calculate distance
        if (strcmp(element->Value(), ND)) break;

        if (i == 0) {
            first = element->Attribute(ND_REF) == nullptr ? "" : element->Attribute(ND_REF);
            std::cout << "\tNODE: " << first << std::endl;
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
        nodes[ref].neighbors.emplace_back(std::make_pair(first, cost));

        std::cout << "\tNODE: " << ref << std::endl
                  << "\t\tDIST: " << cost << std::endl;

        first = ref;
        ++i;
    } while((element = element->NextSiblingElement()) != nullptr);
}