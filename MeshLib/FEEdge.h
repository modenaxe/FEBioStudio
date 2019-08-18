#pragma once
#include "FEItem.h"

//-----------------------------------------------------------------------------
// Edge types
// (NOTE: Do not change the order of these values)
enum FEEdgeType
{
	FE_EDGE2,
	FE_EDGE3,
	FE_EDGE4,
	FE_EDGE_INVALID
};

//-----------------------------------------------------------------------------
// The FEEdge class stores the edge data.
// An edge can be linear, quadratic or cubic. Note that the first two nodes 
// always represent the outside nodes. 
//
//   1                    2 
//   +--------------------+        linear element
//
//   1         3          2
//   +---------o----------+        quadratic element
//
//   1      3       4     2
//   +------o-------o-----+        cubic element
//
// An edge can have two neighors, one on each end. A neighbor is only set
// if only one other edge connects to that end. If no other edge, or multiple
// edges connect, the neighbor is set to -1.
//
class FEEdge : public FEItem
{
public:
	enum { MAX_NODES = 4 };

public:
	//! constructor
	FEEdge();

	//! constructor
	FEEdge(const FEEdge& e);

	//! assignment operator
	void operator = (const FEEdge& e);

	//! edge comparison
	bool operator == (const FEEdge& e) const;

	//! return number of nodes
	int Nodes() const;

	//! find a node index
	int FindNodeIndex(int node) const;

	//! Get the edge type
	int Type() const { return m_type; }

	//! set the type
	void SetType(FEEdgeType type);

public:
	int	m_type;			//!< edge type
	int n[MAX_NODES];	//!< edge nodes
	int	m_elem;			//!< the element to which this edge belongs (used only by beams)
	int	m_nbr[2];		//!< the two adjacent edges (only defined for edges with gid >= 0, and if there are more edges incident to a node, the neighbour is set to -1)
	int	m_face[2];		//!< the two faces adjacent to this edge (TODO: I think I should delete this since I cannot assume that each edge is shared by only one or two faces)
};
