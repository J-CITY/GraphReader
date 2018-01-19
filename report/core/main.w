\chapter*{Проект}
\addcontentsline{toc}{chapter}{Проект}
\section{Средства реализации}
Для реализации проекта были использваны следующие решения:
\begin{itemize}
  \item Язык программирования --- C++
  \item Парсер TinyXML для десериализации данных формата XML (OSM XML)
\end{itemize}

\section{Модули и алгоритмы}
\noindent\indent Алгоритм работы программы состоит в считывании OSM XML
файла и выводе информации в основной выходной поток в один проход путем
десериализации входных данных спомощью парсера TinyXML, небольшой предобработки,
заключающейся в рассчете расстояния между связанными узлами, и в потоковом выводе
освоенных данных, а также сохранении их во временный контейнер.
\par Для рассчета расстояния между узлами использовалась формула нахождения
расстояния между точками на сфере, где под сферой подразумевалась земная оболочка.
\subsection{Реализация}
\noindent\indent Интерфейс программного решения расположен в файле Graph.h и
представляет из себя класс \codepart{Graph}
@D Graph @{
#ifndef GRAPH_INCLUDED
#define GRAPH_INCLUDED

#include <vector>
#include <map>
#include <string>
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
class Way {
public:
    std::string id;
    std::vector<std::string> nodes;
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
    Graph() = default;
    const int __PRINT_CONSOLE = 1;
    const int __PRINT_NOT = 0;
    int __PRINT = 0;
    void ReadGraphFromXML(std::string input);
    void clear();
private:
    void ReadXmlBounds(TiXmlElement* element);
    void ReadXmlNode(TiXmlElement* element);
    void ReadXmlTags(TiXmlElement* element, Node &n);
    void ReadXmlWay(TiXmlElement* element);
public:
    std::map<std::string, Node> nodes;
    std::vector<Way> ways;
    void SaveToXML(std::string filename);
    void SaveToTXT(std::string filename);
    void GenTest(unsigned int nodeSize, std::string filename);
};
#endif // GRAPH_INCLUDED
@}
Основным методом данного класса является \codepart{ReadGraphFromXML},
принимающий на вход путь до OSM XML файла.
@d method ReadGraphFromXML @{
void Graph::ReadGraphFromXML(std::string input) {
    @< Setup TinyXML parser @>
    //Start "recording" data
    TiXmlElement *pRoot = doc.FirstChildElement(OSM);

    if (pRoot == nullptr) return;

    TiXmlElement *pElem = pRoot->FirstChildElement();

    if (pElem == nullptr) return;

    @< Read info about bounds @>

    @< Read info about ways and nodes @>

}
@}
При необходимости можно считать данные из нескольких файлов в один граф,
путем вызова данного метода на каждом из файлов, содержащих необходимую для
считывания информацию.
\par Для <<очистки>> графа есть метод \codepart{clear}.
@d method clear @{
void Graph::clear() {
    this->bound.maxlat =
    this->bound.maxlon =
    this->bound.minlat =
    this->bound.minlon = 0;
    this->nodes.clear();
}
@}
Внутри метода \codepart{ReadGraphFromXML} исходный файл "передается" (файл открывается уже
внутри TinyXML - передается только путь к файлу) в качестве параметра парсеру.
@d Setup TinyXML parser @{
TiXmlDocument doc(input.c_str());

if (!doc.LoadFile()) {
    std::cout << "ERROR: Could not load XML file: " + input << std::endl;
    return;
}

TiXmlHandle hDoc(&doc);
@}

В дальнейшем файл поэлементно десериализуется.
Сначала программа попытается считать границы.
@d Read info about bounds @{
if (!strcmp(pElem->Value(), BOUNDS)) {
    ReadXmlBounds(pElem);
    pElem = pElem->NextSiblingElement();
    if (pElem == nullptr) return;
}
@}
Для чего используется вспомогательный метод \codepart{ReadXmlBounds}.
@d method ReadXmlBounds @{
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
@}
После чего перейдет в состояние считывания информации об узлах и путях между ними.
@d Read info about ways and nodes @{
do {
    if (!strcmp(pElem->Value(), NODE)) {
        ReadXmlNode(pElem);
        continue;
    }
    if (!strcmp(pElem->Value(), WAY))
        ReadXmlWay(pElem);
} while ((pElem = pElem->NextSiblingElement()) != nullptr);
@}
Для обработки данных о каждом узле используется метод \codepart{ReadXmlNode},
внутри которого считывается и выводится в стандартный выходной поток его содержимое.
@D method ReadXmlNode @{
void Graph::ReadXmlNode(TiXmlElement* element) {
    if (element == nullptr)
      return;
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
@}
Для обработки дополнительных тэгов узла используется вспомогательный метод
\codepart{ReadXmlTags}.
@d method ReadXmlTags @{
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
@}
Для обработки данных о путях используется метод \codepart{ReadXmlWay}.
@d method ReadXmlWay @{
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
        @< Read way tag @>
        @< Read node id @>
        @< Calculate distance @>

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
@}
Внутри которого в цикле считывается вся информация о текущем пути.
@d Read way tag @{
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
@}
@d Read node id @{
if (strcmp(element->Value(), ND)) break;
if (i == 0) {
    first = element->Attribute(ND_REF) == nullptr ? "" : element->Attribute(ND_REF);
    if (__PRINT == __PRINT_CONSOLE) {
        std::cout << "\tNODE: " << first << std::endl;
    }
    ++i;
    continue;
}
const char* ref = element->Attribute(ND_REF) == nullptr ? "" : element->Attribute(ND_REF);
way.nodes.push_back(ref);
@}
И рассчитывается расстояние между узлами.
@d Calculate distance @{
/** Average distance
 * L = d*R
 * R = 6371km
 * cos(d) = sin(p_a)*sin(p_b) + cos(p_a)*cos(p_b)*cos(l_a-l_b)
 * l - lon, p - lat
 */
double cost = 0;
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
@}
Для сохранения графа в файл реализованы фцнкции \codepart{SaveToXML} и \codepart{SaveToXML}.
SaveToXML - принимает название выходного файла и сохраняет граф в .xml файл.\par
@d method SaveToXML @{
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
@}
\codepart{SaveToTXT} - принимает название выходного файла и сохраняет граф в .txt файл.
Файл имеет структуру: <вершина от куда> <вершина куда> <вес ребра>.\par
@d method SaveToTXT @{
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
@}
\chapter*{Тестирование}
\addcontentsline{toc}{chapter}{Тестирование}
Для тестирования был написан генератор тестов, который генерируется OSM файл.
В нём идентификаторы гинерируются последовательно, что помогает упростить
проверку верности работы программы.
Были сгенерированны тесты от 5 до 10000 вершин, чтобы проверить стабильность и
корректность рабрты программы.
Данные считанные из сгенерированных файлов совпали с ожидаемым результатом.
Так же был написан тест вручную из 20 вершин, такой, чтобы длины рёбер были равны нулю.
Тем самым выполнилась проверка расчёта весов рёбер графа.
Тест пройден успешно.
Для тестирования большого файла был взят OMS файл Магаданской области.
В нём более 500 тысяч вершин. После считывания файла был построен граф.
Время работы приложения: 25 секунд.
Размер файла: 7 MB.
@d method GenTest @{
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
@}
Все тесты пройдены успешно.
Время работы от 0.1 до 20 секунд.
Список фрагментов
@m
\newpage
\chapter*{Приложение}
@o Graph.h @{
 @< Graph @>
@}
@o Graph.cpp @{
#include "Graph.h"
#include <utility>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

@< method clear @>
@< method ReadGraphFromXML @>
@< method ReadXmlBounds @>
@< method ReadXmlNode @>
@< method ReadXmlTags @>
@< method ReadXmlWay @>
@< method SaveToXML @>
@< method SaveToTXT @>
@< method GenTest @>
@}
@o main.cpp @{
#include "Graph.h"
#include <ctime>
#include <iostream>

using namespace std;

int main() {
    clock_t time;
    time = clock();

    Graph g;
    //g.ReadGraphFromXML("RU-MAG-1.osm");
    //g.ReadGraphFromXML("test1.xml");
    g.ReadGraphFromXML("test.xml");

    g.SaveToTXT("out.xml");

    time = clock() - time;
    std::cout << time/CLOCKS_PER_SEC << endl;
    return 0;
}
@}
\newpage
