#include "stdafx.h"
#include "FESolidArc.h"
#include <GeomLib/GPrimitive.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FESolidArc::FESolidArc(GSolidArc* po)
{
	m_pobj = po;

	m_nd = 8;
	m_ns = 4;
	m_nz = 8;

	m_gz = m_gr = 1;
	m_bz = false;
	m_br = false;
	
	// define the tube parameters
	AddIntParam(m_nd, "nd", "Divisions");
	AddIntParam(m_ns, "ns", "Segments");
	AddIntParam(m_nz, "nz", "Stacks");

	AddDoubleParam(m_gz, "gz", "Z-bias");
	AddDoubleParam(m_gr, "gr", "R-bias");

	AddBoolParam(m_bz, "bz", "Z-mirrored bias");
	AddBoolParam(m_br, "br", "R-mirrored bias");
}

FEMesh* FESolidArc::BuildMesh()
{
	assert(m_pobj);

	int i, j, k, n;

	// get object parameters
	ParamBlock& param = m_pobj->GetParamBlock();
	double R0 = param.GetFloatValue(GSolidArc::RIN);
	double R1 = param.GetFloatValue(GSolidArc::ROUT);
	double h  = param.GetFloatValue(GSolidArc::HEIGHT);
	double w  = PI*param.GetFloatValue(GSolidArc::ARC)/180.0;

	// get mesh parameters
	m_nd = GetIntValue(NDIV);
	m_ns = GetIntValue(NSEG);
	m_nz = GetIntValue(NSTACK);

	m_gz = GetFloatValue(ZZ);
	m_gr = GetFloatValue(ZR);

	m_bz = GetBoolValue(GZ2);
	m_br = GetBoolValue(GR2);

	// check parameters
	if (m_nd < 1) m_nd = 1;
	if (m_ns < 1) m_ns = 1;
	if (m_nz < 1) m_nz = 1;

	if (m_nz == 1) m_bz = false;
	if (m_ns == 1) m_br = false;

	// get the parameters
	int nd = m_nd;
	int nr = m_ns;
	int nz = m_nz;

	double fz = m_gz;
	double fr = m_gr;

	int nodes = (nd+1)*(nr+1)*(nz+1);
	int elems = nd*nr*nz;

	FEMesh* pm = new FEMesh();
	pm->Create(nodes, elems);

	double cosa, sina;
	double x, y, z, R;

	double gz = 1;
	double gr = 1;

	if (m_bz)
	{
		gz = 2; if (m_nz%2) gz += fz;
		for (i=0; i<m_nz/2-1; ++i) gz = fz*gz+2;
		gz = h / gz;
	}
	else
	{
		for (i=0; i<nz-1; ++i) gz = fz*gz+1; 
		gz = h / gz;
	}

	if (m_br)
	{
		gr = 2; if (m_ns%2) gr += fr;
		for (i=0; i<m_ns/2-1; ++i) gr = fr*gr+2;
		gr = (R1 - R0) / gr;
	}
	else
	{
		for (i=0; i<nr-1; ++i) gr = fr*gr+1; 
		gr = (R1 - R0) / gr;
	}


	// create nodes
	n = 0;
	double dr;
	double dz = gz;
	z = 0;
	for (k=0; k<=nz; ++k)
	{
		for (j=0; j<=nd; ++j)
		{
			cosa = (double) cos(w*j/nd);
			sina = (double) sin(w*j/nd);

			dr = gr;
			R = R0;
			for (i=0; i<=nr; ++i, ++n)
			{
				x = R*cosa;
				y = R*sina;

				FENode& node = pm->Node(n);

				node.r = vec3d(x, y, z);

				R += dr;
				dr *= fr;
				if (m_br && (i == m_ns/2-1))
				{
					if (m_ns%2 == 0) dr /= fr;
					fr = 1.0/fr;
				}
			}
			if (m_br) fr = 1.0/fr;
		}

		z += dz;
		dz *= fz;
		if (m_bz && (k == m_nz/2-1))
		{
			if (m_nz%2 == 0) dz /= fz;
			fz = 1.0/fz;
		}
	}

	// assign node ID's
	pm->Node( NodeIndex(nr,    0, 0) ).m_gid = 0;
	pm->Node( NodeIndex(nr,   nd, 0) ).m_gid = 1;
	pm->Node( NodeIndex( 0,   nd, 0) ).m_gid = 2;
	pm->Node( NodeIndex( 0,    0, 0) ).m_gid = 3;
	pm->Node( NodeIndex(nr,    0, nz) ).m_gid = 4;
	pm->Node( NodeIndex(nr,   nd, nz) ).m_gid = 5;
	pm->Node( NodeIndex( 0,   nd, nz) ).m_gid = 6;
	pm->Node( NodeIndex( 0,    0, nz) ).m_gid = 7;

	// create elements
	n = 0;
	for (k=0; k<nz; ++k)
		for (j=0; j<nd; ++j)
			for (i=0; i<nr; ++i, ++n)
			{
				FEElement& e = pm->Element(n);
				e.SetType(FE_HEX8);
				e.m_gid = 0;

				e.m_node[0] = k*(nd+1)*(nr+1) + j*(nr+1) + i;
				e.m_node[1] = k*(nd+1)*(nr+1) + j*(nr+1) + i+1;
				e.m_node[2] = k*(nd+1)*(nr+1) + (j+1)*(nr+1) + i+1;
				e.m_node[3] = k*(nd+1)*(nr+1) + (j+1)*(nr+1) + i;
				e.m_node[4] = (k+1)*(nd+1)*(nr+1) + j*(nr+1) + i;
				e.m_node[5] = (k+1)*(nd+1)*(nr+1) + j*(nr+1) + i+1;
				e.m_node[6] = (k+1)*(nd+1)*(nr+1) + (j+1)*(nr+1) + i+1;
				e.m_node[7] = (k+1)*(nd+1)*(nr+1) + (j+1)*(nr+1) + i;
			}

	BuildFaces(pm);
	BuildEdges(pm);

	pm->Update();

	return pm;
}

void FESolidArc::BuildFaces(FEMesh* pm)
{
	int i, j;

	int nd = m_nd;
	int nz = m_nz;
	int nr = m_ns;

	// count the faces
	int NF = 2*nz*nd + 2*nr*nd + 2*nz*nr;
	pm->Create(0,0,NF);

	// build the faces
	FEFace* pf = pm->FacePtr();

	// face 0
	for (j=0; j<nz; ++j)
	{
		for (i=0; i<nd; ++i, ++pf)
		{
			FEFace& f = *pf;
			f.SetType(FE_FACE_QUAD4);
			f.m_gid = 0;
			f.m_sid = 0;
			f.n[0] = NodeIndex(nr,   i,   j);
			f.n[1] = NodeIndex(nr, i+1,   j);
			f.n[2] = NodeIndex(nr, i+1, j+1);
			f.n[3] = NodeIndex(nr,   i, j+1);
		}
	}

	// face 1
	for (j=0; j<nz; ++j)
	{
		for (i=0; i<nr; ++i, ++pf)
		{
			FEFace& f = *pf;
			f.SetType(FE_FACE_QUAD4);
			f.m_gid = 1;
			f.m_sid = 1;
			f.n[0] = NodeIndex(i  , nd,   j);
			f.n[1] = NodeIndex(i+1, nd,   j);
			f.n[2] = NodeIndex(i+1, nd, j+1);
			f.n[3] = NodeIndex(i  , nd, j+1);
		}
	}

	// face 2
	for (j=0; j<nz; ++j)
	{
		for (i=0; i<nd; ++i, ++pf)
		{
			FEFace& f = *pf;
			f.SetType(FE_FACE_QUAD4);
			f.m_gid = 2;
			f.m_sid = 2;
			f.n[0] = NodeIndex(0, i+1, j  );
			f.n[1] = NodeIndex(0, i  , j  );
			f.n[2] = NodeIndex(0, i  , j+1);
			f.n[3] = NodeIndex(0, i+1, j+1);
		}
	}

	// face 3
	for (j=0; j<nz; ++j)
	{
		for (i=0; i<nr; ++i, ++pf)
		{
			FEFace& f = *pf;
			f.SetType(FE_FACE_QUAD4);
			f.m_gid = 3;
			f.m_sid = 3;
			f.n[0] = NodeIndex(i  , 0,   j);
			f.n[1] = NodeIndex(i+1, 0,   j);
			f.n[2] = NodeIndex(i+1, 0, j+1);
			f.n[3] = NodeIndex(i  , 0, j+1);
		}
	}

	// face 4
	for (j=0; j<nd; ++j)
	{
		for (i=0; i<nr; ++i, ++pf)
		{
			FEFace& f = *pf;
			f.SetType(FE_FACE_QUAD4);
			f.m_gid = 4;
			f.m_sid = 4;
			f.n[0] = NodeIndex(i+1, j,   0);
			f.n[1] = NodeIndex(i  , j,   0);
			f.n[2] = NodeIndex(i  , j+1, 0);
			f.n[3] = NodeIndex(i+1, j+1, 0);
		}
	}

	// face 5
	for (j=0; j<nd; ++j)
	{
		for (i=0; i<nr; ++i, ++pf)
		{
			FEFace& f = *pf;
			f.SetType(FE_FACE_QUAD4);
			f.m_gid = 5;
			f.m_sid = 5;
			f.n[0] = NodeIndex(i  , j,   nz);
			f.n[1] = NodeIndex(i+1, j,   nz);
			f.n[2] = NodeIndex(i+1, j+1, nz);
			f.n[3] = NodeIndex(i  , j+1, nz);
		}
	}

}

void FESolidArc::BuildEdges(FEMesh* pm)
{
	int i;

	int nd = m_nd;
	int nz = m_nz;
	int nr = m_ns;

	// count edges
	int nedges = 4*nd + 4*nz + 4*nr;
	pm->Create(0,0,0,nedges);
	FEEdge* pe = pm->EdgePtr();

	for (i= 0; i<  nd; ++i, ++pe) { pe->SetType(FE_EDGE2); pe->m_gid = 0; pe->n[0] = NodeIndex(nr, i,  0); pe->n[1] = NodeIndex(nr, i+1,  0); }
	for (i= 0; i<  nd; ++i, ++pe) { pe->SetType(FE_EDGE2); pe->m_gid = 2; pe->n[0] = NodeIndex(0 , i,  0); pe->n[1] = NodeIndex(0 , i+1,  0); }
	for (i= 0; i<  nd; ++i, ++pe) { pe->SetType(FE_EDGE2); pe->m_gid = 4; pe->n[0] = NodeIndex(nr, i, nz); pe->n[1] = NodeIndex(nr, i+1, nz); }
	for (i= 0; i<  nd; ++i, ++pe) { pe->SetType(FE_EDGE2); pe->m_gid = 6; pe->n[0] = NodeIndex(0 , i, nz); pe->n[1] = NodeIndex(0 , i+1, nz); }

	for (i= 0; i<  nr; ++i, ++pe) { pe->SetType(FE_EDGE2); pe->m_gid = 1; pe->n[0] = NodeIndex(i , nd,  0); pe->n[1] = NodeIndex(i+1 , nd, 0); }
	for (i= 0; i<  nr; ++i, ++pe) { pe->SetType(FE_EDGE2); pe->m_gid = 3; pe->n[0] = NodeIndex(i ,  0,  0); pe->n[1] = NodeIndex(i+1 ,  0, 0); }
	for (i= 0; i<  nr; ++i, ++pe) { pe->SetType(FE_EDGE2); pe->m_gid = 5; pe->n[0] = NodeIndex(i , nd, nz); pe->n[1] = NodeIndex(i+1 , nd, nz); }
	for (i= 0; i<  nr; ++i, ++pe) { pe->SetType(FE_EDGE2); pe->m_gid = 7; pe->n[0] = NodeIndex(i ,  0, nz); pe->n[1] = NodeIndex(i+1 ,  0, nz); }

	for (i= 0; i<  nz; ++i, ++pe) { pe->SetType(FE_EDGE2); pe->m_gid =  8; pe->n[0] = NodeIndex(nr,  0, i); pe->n[1] = NodeIndex(nr,  0, i+1); }
	for (i= 0; i<  nz; ++i, ++pe) { pe->SetType(FE_EDGE2); pe->m_gid =  9; pe->n[0] = NodeIndex(nr, nd, i); pe->n[1] = NodeIndex(nr, nd, i+1); }
	for (i= 0; i<  nz; ++i, ++pe) { pe->SetType(FE_EDGE2); pe->m_gid = 10; pe->n[0] = NodeIndex( 0, nd, i); pe->n[1] = NodeIndex( 0, nd, i+1); }
	for (i= 0; i<  nz; ++i, ++pe) { pe->SetType(FE_EDGE2); pe->m_gid = 11; pe->n[0] = NodeIndex( 0,  0, i); pe->n[1] = NodeIndex( 0,  0, i+1); }
}
