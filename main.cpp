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
