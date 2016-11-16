/*graphutils.h*/
#ifndef _GRAPH_HPP_
#define _GRAPH_HPP_

#ifndef _PRIMITIVES_H_
#include "../primitives.hpp"
#endif

#ifndef _GRAPHUTILS_H_
#include "../graphutils.hpp"
#endif

#include <boost/graph/adjacency_list.hpp>

typedef boost::adjacency_list<> GdbGraphTypeDirected;
typedef boost::adjacency_list<boost::vecS,boost::vecS,boost::undirectedS> GdbGraphTypeUndirected;

/*
    The interface for GDB defining all the primary graph operations
    V -> Vertex Type Representation
    U -> Edge Type Representation
*/
template < typename U, typename V >
class GdbGraph : public GdbUtils::GdbObject
{
    private:
        bool m_is_directed;

        GdbGraphTypeDirected m_graph;

        Gdb_N_t m_vertexes;
        Gdb_N_t m_edges;

    public:
        // Ctor only for direct initialization
        explicit GdbGraph(bool _isDirected, Gdb_N_t _edges, Gdb_N_t _vertexes)
        : m_is_directed(_isDirected)
        , m_edges( _edges )
        , m_vertexes( _vertexes ) {}

        /*
            Add edge between _v1 and _v2 vertexes.
        */
        void            AddEdge(const V &_v1, const V &_v2);
        /*
            Add vertex _v to the graph
        */
        void            AddVertex(V &_v);
        /*
            Tests whether there is an edge from _v1 to _v2;
        */
        bool            Adjacent(const V &_v1, const V &_v2);

        //virtual GetEdgeValue(const V &_v1, GdbString &_prop)=0;
        //virtual V& GetVertexValue(GdbString &_id)=0;

        GdbGraphType    create_empty_directed_graph()
        {
            return {};
        }

        /*
            Gets a list of all vertexes that have an edge to _x
        */
        V*              Neighbours (Gdb_N_t &_size, const V& _x);
        /*
            Remove edge between _v1 and _v2
        */
        void            RemoveEdge(const V &_v1, const V &_v2);
        /*
            Remove vertex _v from the graph (also removing edges)
        */
        void            RemoveVertex(V &_v);

        //virtual void SetEdgeValue(GdbString &_prop)=0;
        //virtual void SetVertexValue(GdbString &_prop)=0;
};

#endif