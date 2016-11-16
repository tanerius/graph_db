/*graphutils.h*/
#ifndef _GRAPH_HPP_
#define _GRAPH_HPP_

#ifndef _PRIMITIVES_H_
#include "../primitives.hpp"
#endif

#ifndef _GRAPHUTILS_H_
#include "../graphutils.hpp"
#endif

/*
    The interface for GDB defining all the primary graph operations
    V -> Vertex Type Representation
    U -> Edge Type Representation
*/
template < typename U, typename V >
class GdbGraph : public GdbUtils::GdbObject
{
    protected:
        bool m_is_directional;
        Gdb_N_t m_vertexes;
        Gdb_N_t m_edges;

    public:
        // Ctor only for direct initialization
        explicit IGdb(bool _isDirectional, Gdb_N_t _edges, Gdb_N_t _vertexes)
        : m_is_directional(_isDirectional)
        , m_edges( _edges )
        , m_vertexes( _vertexes ) {}

        virtual ~IGdb() {};
        /*
            Add edge between _v1 and _v2 vertexes.
        */
        virtual void AddEdge(const V &_v1, const V &_v2)=0;
        /*
            Add vertex _v to the graph
        */
        virtual void AddVertex(V &_v)=0;
        /*
            Tests whether there is an edge from _v1 to _v2;
        */
        virtual bool Adjacent(const V &_v1, const V &_v2) = 0;

        //virtual GetEdgeValue(const V &_v1, GdbString &_prop)=0;
        //virtual V& GetVertexValue(GdbString &_id)=0;

        /*
            Gets a list of all vertexes that have an edge to _x
        */
        virtual V* Neighbours (Gdb_N_t &_size, const V& _x) = 0;
        /*
            Remove edge between _v1 and _v2
        */
        virtual void RemoveEdge(const V &_v1, const V &_v2)=0;
        /*
            Remove vertex _v from the graph (also removing edges)
        */
        virtual void RemoveVertex(V &_v)=0;

        //virtual void SetEdgeValue(GdbString &_prop)=0;
        //virtual void SetVertexValue(GdbString &_prop)=0;
};

#endif