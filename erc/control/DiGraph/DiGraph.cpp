#include <iostream>
#include <string>
#include <vector>
#include "Edge.h"
#include "Vertex.h"
#include "DiGraph.h"
#include "../../modules/DataPointer.h"

using namespace std;
using namespace erc;

DiGraph::DiGraph()
{
}

DiGraph::~DiGraph()
{
}

void DiGraph::clear(){
    vertices.clear();
}

bool DiGraph::addEdge(string fromKey, string toKey, double cost){
    if(!isEdge(fromKey, toKey)){
        getVertex(fromKey)->addEdge(toKey, cost);
        return true;
    }
    return false;
}


bool DiGraph::addVertex(string key, DataPointer data){
    vertices.push_back(Vertex(key, data));
}


bool DiGraph::isEdge(string fromKey, string toKey){
    Vertex* vert=getVertex(fromKey);
    if(vert!=NULL){ //Does the vertex exist?
        if(vert->getEdge(toKey)!=NULL){  //Edge exist?
            return true;    //Sure
        }
    }
    return false;   //No such edge
}


bool DiGraph::isVertex(string key){
    if(getVertex(key)!=NULL){
        return true;
    }
    return false;
}

Vertex* DiGraph::getVertex(string key){
    for(int i=0; i<vertices.size(); i++){
        if(vertices[i].getKey()==key){
            return &vertices[i];    //Found it
        }
    }
    return false;
}


double DiGraph::getEdgeCost(string fromKey, string toKey){
    Vertex* fromVertex=getVertex(fromKey);
    if(fromVertex!=NULL){
        Edge* edge=fromVertex->getEdge(toKey);
        if(edge!=NULL){
            return edge->getCost();
        }
    }
    return 0;
}


DataPointer DiGraph::getVertexData(string key){
    Vertex* vert=getVertex(key);
    if(vert!=NULL){
        return vert->getData();
    }
    return DataPointer();
}


vector<string> DiGraph::vertexKeys(){
    vector<string> keys;
    for(int i=0; i<vertices.size(); i++){
        keys.push_back(vertices[i].getKey());
    }
    return keys;
}


vector<DataPointer> DiGraph::vertexData(){
    vector<DataPointer> data;
    for(int i=0; i<vertices.size(); i++){
        data.push_back(vertices[i].getData());
    }
    return data;
}

