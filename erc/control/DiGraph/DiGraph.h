#ifndef ERC_DiGraph_H
#define ERC_DiGraph_H

#include <iostream>
#include <string>
#include <vector>
#include "Edge.h"
#include "Vertex.h"
#include "../../modules/DataPointer.h"

using namespace std;

namespace erc{
    class DiGraph{
        public:
            /**
            *   Construct
            */
            DiGraph();

            /**
            *   Destroy
            */
            ~DiGraph();

            /**
            *   Clears out the graph
            */
            void clear();

            /**
            *   Adds and edge from a vertex identified by fromKey to a vertex
            *   identified by toKey with a cost of cost. If an edge already
            *   exists between fromKey and toKey then false is returned,
            *   otherwise returns true.
            */
            bool addEdge(string fromKey, string toKey, double cost);

            /**
            *   Adds a vertex identified by key and containing data
            */
            bool addVertex(string key, DataPointer data);

            /**
            *   Returns true if an edge exists between fromKey and toKey
            */
            bool isEdge(string fromKey, string toKey);

            /**
            *   Returns true if a vertex identified by key already exists
            */
            bool isVertex(string key);

            /**
            *   Returns a vertex pointer to the vertex identified by key
            */
            Vertex* getVertex(string key);

            /**
            *   Returns cost of the edge starting at fromKey and ending at toKey
            */
            double getEdgeCost(string fromKey, string toKey);

            /**
            *   Returns the data held in the vertex identified by key
            */
            DataPointer getVertexData(string key);

            /**
            *   Returns a list of all vertices' keys
            */
            vector<string> vertexKeys();

            /**
            *   Returns a list of all vertex data
            */
            vector<DataPointer> vertexData();

        private:
            vector<Vertex> vertices;    //List of vertices
    };
}

#endif
