#include <iostream>
#include "Edge.h"
#include <string.h>

using namespace erc;
using namespace std;

Edge::Edge(string key, double cost){
    vertexKey=key;
    this->cost=cost;
}

Edge::~Edge(){

}

double Edge::getCost(){
    return cost;
}

void Edge::setCost(double cost){
    this->cost=cost;
}

string Edge::getVertexKey(){
    return vertexKey;
}
