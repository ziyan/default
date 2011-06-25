#ifndef ERC_GraphVertex_H
#define ERC_GraphVertex_H

#include <iostream>
#include <string>
#include <vector>
#include "Edge.h"
#include "../../modules/DataPointer.h"

using namespace std;

namespace erc{
    class Vertex
    {
        public:
            /**
            *   Creates now vertex with key, key and data, data
            */
            Vertex(string key, DataPointer data);

            /**
            *   Destructor
            */
            ~Vertex();

            /**
            *   Returns edge point to the edge that links this vertex with
            *   vertex or returns NULL if vertex does not exist
            */
            Edge* getEdge(string vertex);

            /**
            *   Creates an edge between this vertex and some other vertex.
            *   Addeds cost data aswell
            */
            void addEdge(string vertex, double cost);

            /**
            *   Returns this vertices' key
            */
            string getKey();

            /**
            *   Returns the data held by this vertex
            */
            DataPointer getData();

            /**
            *   Sets the data held by this vertex to newData
            */
            void setData(DataPointer newData);

            /**
            *   Returns a vector of all of this vertex's closest friends... err
            *   neighbors
            */
            vector<Edge> getNeighborEdges();

            /**
            *   Returns a vector of strings of all of this vertex's neighbors
            *   id keys
            */
            vector<string> getNeighborKeys();

        private:
            string vertexKey;   //This vertex's key
            DataPointer data;   //Data that this vertex holds
            vector<Edge> edges; //Vector of this vertex's out-going edges
    };
}

#endif
