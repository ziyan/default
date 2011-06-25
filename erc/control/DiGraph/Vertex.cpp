#include <iostream>
#include <string>
#include <vector>
#include "Edge.h"
#include "../../modules/DataPointer.h"
#include "Vertex.h"

using namespace std;
using namespace erc;

Vertex::Vertex(string key, DataPointer data):vertexKey(key),data(data)
{
}

Vertex::~Vertex()
{
}

Edge* Vertex::getEdge(string vertex){
    for(int i=0; i<edges.size(); i++){  //Loop through edge list
        if(vertex==edges[i].getVertexKey()){
            return &edges[i];   //Found it!
        }
    }
    return NULL;    //No such out-going edge
}

void Vertex::addEdge(string vertex, double cost){
    Edge temp(vertex, cost);    //Create edge
    edges.push_back(temp);  //Add to list
}

string Vertex::getKey(){
    return vertexKey;   //This vertex's key
}

DataPointer Vertex::getData(){
    return data;    //If its data you want, its data you get
}

void Vertex::setData(DataPointer newData){
    data=newData;   //Update data
}

vector<Edge> Vertex::getNeighborEdges(){
    return edges;   //Edge list
}

vector<string> Vertex::getNeighborKeys(){
    vector<string> keyRing; //I love keyrings, I'd be lost without mine :)
    for(int i=0; i<edges.size(); i++){
        keyRing.push_back(edges[i].getVertexKey()); //Add to vector
    }
    return keyRing; //Key list
}
