#include "stdafx.h"
#include <MeshLib/FEMesh.h>
#include "FEModifier.h"
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------
FETet4ToTet5::FETet4ToTet5() : FEModifier("Tet4-to-Tet5")
{

}

//-----------------------------------------------------------------------------
FEMesh* FETet4ToTet5::Apply(FEMesh* pm)
{
	// before we get started, let's make sure this is a tet4 mesh
	if (pm->IsType(FE_TET4) == false) return 0;

	int NN = pm->Nodes();
	int NF = pm->Faces();
	int NT = pm->Elements();
	int NC = pm->Edges();

	// a new node is added for each element
	int NN1 = NN + NT;

	// allocate a new mesh
	FEMesh* pnew = new FEMesh;
	pnew->Create(NN1, NT, NF, NC);

	// copy the old nodes
	for (int i = 0; i<NN; ++i)
	{
		FENode& n0 = pm->Node(i);
		FENode& n1 = pnew->Node(i);
		n1.r = n0.r;
		n1.m_gid = n0.m_gid;
	}

	// create the new nodes
	for (int i = 0; i<NT; ++i)
	{
		FEElement& el = pm->Element(i);
		vec3d& r0 = pm->Node(el.m_node[0]).r;
		vec3d& r1 = pm->Node(el.m_node[1]).r;
		vec3d& r2 = pm->Node(el.m_node[2]).r;
		vec3d& r3 = pm->Node(el.m_node[3]).r;

		FENode& n1 = pnew->Node(i + NN);
		n1.r = (r0 + r1 + r2 + r3)*0.25;
	}

	// create the elements
	for (int i = 0; i<NT; ++i)
	{
		FEElement& e0 = pm->Element(i);
		FEElement& e1 = pnew->Element(i);
		e1 = e0;

		e1.m_gid = e0.m_gid;

		e1.SetType(FE_TET5);
		e1.m_node[0] = e0.m_node[0];
		e1.m_node[1] = e0.m_node[1];
		e1.m_node[2] = e0.m_node[2];
		e1.m_node[3] = e0.m_node[3];
		e1.m_node[4] = NN + i;
	}

	// create the new faces
	for (int i = 0; i<NF; ++i)
	{
		FEFace& f0 = pm->Face(i);
		FEFace& f1 = pnew->Face(i);
		f1 = f0;
	}

	// create the new edges
	for (int i = 0; i<NC; ++i)
	{
		FEEdge& e0 = pm->Edge(i);
		FEEdge& e1 = pnew->Edge(i);
		e1 = e0;
	}

	//	pnew->UpdateElementNeighbors();
	pnew->MarkExteriorNodes();
	pnew->UpdateEdgeNeighbors();
	pnew->UpdateNormals();

	return pnew;
}

//-----------------------------------------------------------------------------
FETet5ToTet4::FETet5ToTet4() : FEModifier("Tet5-to-Tet4")
{

}

//-----------------------------------------------------------------------------
FEMesh* FETet5ToTet4::Apply(FEMesh* pm)
{
	// before we get started, let's make sure this is a tet4 mesh
	if (pm->IsType(FE_TET5) == false) return 0;

	int NN = pm->Nodes();
	int NF = pm->Faces();
	int NT = pm->Elements();
	int NC = pm->Edges();

	// figure out how many nodes we actually need
	vector<int> nodeTag(NN, -1);
	for (int i = 0; i < NT; ++i)
	{
		FEElement& el = pm->Element(i);
		for (int j = 0; j < 4; ++j) nodeTag[el.m_node[j]] = 1;
	}
	int NN1 = 0;
	for (int i = 0; i < NN; ++i)
	{
		if (nodeTag[i] != -1) nodeTag[i] = NN1++;
	}

	// allocate a new mesh
	FEMesh* pnew = new FEMesh;
	pnew->Create(NN1, NT, NF, NC);

	// create the new nodes
	for (int i = 0; i<NN; ++i)
	{
		if (nodeTag[i] >= 0)
		{
			FENode& nd = pnew->Node(nodeTag[i]);
			FENode& ns = pm->Node(i);
			nd = ns;
		}
	}

	// create the elements
	for (int i = 0; i<NT; ++i)
	{
		FEElement& e0 = pm->Element(i);
		FEElement& e1 = pnew->Element(i);
		e1 = e0;

		e1.m_gid = e0.m_gid;

		e1.SetType(FE_TET4);
		e1.m_node[0] = e0.m_node[0];
		e1.m_node[1] = e0.m_node[1];
		e1.m_node[2] = e0.m_node[2];
		e1.m_node[3] = e0.m_node[3];
	}

	// create the new faces
	for (int i = 0; i<NF; ++i)
	{
		FEFace& f0 = pm->Face(i);
		FEFace& f1 = pnew->Face(i);
		f1 = f0;
	}

	// create the new edges
	for (int i = 0; i<NC; ++i)
	{
		FEEdge& e0 = pm->Edge(i);
		FEEdge& e1 = pnew->Edge(i);
		e1 = e0;
	}

	//	pnew->UpdateElementNeighbors();
	pnew->MarkExteriorNodes();
	pnew->UpdateEdgeNeighbors();
	pnew->UpdateNormals();

	return pnew;
}
