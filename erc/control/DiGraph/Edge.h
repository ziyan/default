#ifndef ERC_GraphEdge_H
#define ERC_GraphEdge_H

#include <iostream>
#include <string>

using namespace std;

namespace erc{
    /**
    *   Represents an edge in a directed graph
    */
    class Edge
    {
        public:
            /**
            *   Create a new edge, that goes to key, and costs cost
            */
            Edge(string key, double cost);

            /**
            *   Destroy
            */
            ~Edge();

            /**
            *   Returns this edge's cost
            */
            double getCost();

            /**
            *   Changes this edge's cost
            */
            void setCost(double cost);

            /**
            *   Returns the key of the vertex that this edge goes to
            */
            string getVertexKey();

        private:
            double cost;    //How much the edge costs
            string vertexKey;   //The key of the vertex at the end of the edge
    };
}

#endif
