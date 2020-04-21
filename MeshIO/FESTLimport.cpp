#include "FESTLimport.h"
#include <GeomLib/GSurfaceMeshObject.h>
#include <MeshTools/GModel.h>

//-----------------------------------------------------------------------------
FESTLimport::FESTLimport()
{
}

//-----------------------------------------------------------------------------
FESTLimport::~FESTLimport(void)
{
}

//-----------------------------------------------------------------------------
// read a line from the input file
bool FESTLimport::read_line(char* szline, const char* sz)
{
	m_nline++;

	int l = (int)strlen(sz);
	if (fgets(szline, 255, m_fp) == 0) return false;

	// remove leading white space
	int n = 0;
	while (isspace(szline[n])) ++n;
	if (n != 0)
	{
		char* ch = szline + n;
		n = 0;
		while (*ch) szline[n++] = *ch++;
	}
	
	if (strncmp(szline, sz, l) != 0) return false;

	return true;
}

//-----------------------------------------------------------------------------
// Load an STL model
bool FESTLimport::Load(FEProject& prj, const char* szfile)
{
	FEModel& fem = prj.GetFEModel();
	m_pfem = &fem;
	m_nline = 0;

	// try to open the file
	if (Open(szfile, "rt") == false) return errf("Failed opening file %s.", szfile);

	// read the first line
	char szline[256] = {0};
	if (read_line(szline, "solid ") == false) return errf("First line must be solid definition.");

	// clear the list
	m_Face.clear();

	// read all the triangles
	int nc = 0;
	FACET face;
	do
	{
		// read the facet line
		if (read_line(szline, "facet normal") == false)
		{
			// check for the endsolid tag
			if (strncmp(szline, "endsolid", 8) == 0) break; 
			else return errf("Error encountered at line %d", m_nline);
		}

		// read the outer loop line
		if (read_line(szline, "outer loop") == false) return errf("Error encountered at line %d", m_nline);

		// read the vertex data
		float x, y, z;
		for (int i=0; i<3; ++i)
		{
			if (read_line(szline, "vertex ") == false) return errf("Error encountered at line %d", m_nline);
			sscanf(szline, "vertex %g%g%g", &x, &y, &z);

			face.r[i].x = x;
			face.r[i].y = y;
			face.r[i].z = z;
		}

		// read the endloop tag
		if (read_line(szline, "endloop") == false) return errf("Error encountered at line %d", m_nline);

		// read the endfacet tag
		if (read_line(szline, "endfacet") == false) return errf("Error encountered at line %d", m_nline);

		// add the facet to the list
		m_Face.push_back(face);
	}
	while (1);

	// close the file
	Close();

	// build the nodes
	build_mesh();

	return true;
}

//-----------------------------------------------------------------------------
// Build the FE model
void FESTLimport::build_mesh()
{
	int i;

	// number of facets
	int NF = (int)m_Face.size();

	// find the bounding box of the model
	BOX& b = m_box = BoundingBox();
	double h = 0.01*b.GetMaxExtent();
	b.Inflate(h,h,h);
	vec3d r0(b.x0, b.y0, b.z0);
	vec3d r1(b.x1, b.y1, b.z1);

	// create the box partition
	int NB = m_NB = (int) pow((double) (NF*3), 0.17) + 1;
	m_BL.resize(NB*NB*NB);
	int n = 0;
	for (i=0; i<NB; ++i)
		for (int j=0; j<NB; ++j)
			for (int k=0; k<NB; ++k)
			{
				OBOX& bn = m_BL[n++];
				bn.r0.x = r0.x + i*(r1.x - r0.x)/NB;
				bn.r0.y = r0.x + j*(r1.x - r0.x)/NB;
				bn.r0.z = r0.x + k*(r1.x - r0.x)/NB;

				bn.r1.x = r0.x + (i+1)*(r1.x - r0.x)/NB;
				bn.r1.y = r0.x + (j+1)*(r1.x - r0.x)/NB;
				bn.r1.z = r0.x + (k+1)*(r1.x - r0.x)/NB;
			}

	// reserve space for nodes
	m_Node.reserve(NF*3);
	
	// create the nodes
	list<FACET>::iterator pf = m_Face.begin();
	int nid = 0;
	for (i=0; i<NF; ++i, ++pf)
	{
		FACET& f = *pf;
		for (int j=0; j<3; ++j) f.n[j] = find_node(f.r[j]);

		// make sure all three nodes are distinct
		int* n = f.n;
		if ((n[0] == n[1]) || (n[0]==n[2]) || (n[1]==n[2])) f.nid = -1;
		else f.nid = nid++;
	}
	int NN = (int)m_Node.size();

	// create the mesh
	FESurfaceMesh* pm = new FESurfaceMesh;
	pm->Create(NN, 0, NF);

	// create nodes
	for (i=0; i<NN; ++i)
	{
		vec3d& ri = m_Node[i].r;
		FENode& node = pm->Node(i);
		node.pos(ri);
	}

	// create elements
	list<FACET>::iterator is = m_Face.begin();
	for (i=0; i<NF; ++i, ++is)
	{
		FACET& f = *is;
		int n = f.nid;
		if (n >= 0)
		{
			FEFace& face = pm->Face(n);
			face.SetType(FE_FACE_TRI3);
			face.m_gid = 0;
			face.n[0] = f.n[0];
			face.n[1] = f.n[1];
			face.n[2] = f.n[2];
		}
	}

	// update the mesh
	pm->RebuildMesh();
	GSurfaceMeshObject* po = new GSurfaceMeshObject(pm);

	static int nc = 1;
	char sz[256];
	sprintf(sz, "STL-Object%02d", nc++);
	po->SetName(sz);

	// add the object to the model
	m_pfem->GetModel().AddObject(po);
}

//-----------------------------------------------------------------------------
int FESTLimport::FindBox(vec3d& r)
{
	BOX& b = m_box;
	int i = (int)(m_NB*(r.x - b.x0)/(b.x1 - b.x0));
	int j = (int)(m_NB*(r.y - b.y0)/(b.y1 - b.y0));
	int k = (int)(m_NB*(r.z - b.z0)/(b.z1 - b.z0));
	return k + j*(m_NB + i*m_NB);
}

//-----------------------------------------------------------------------------
int FESTLimport::find_node(vec3d& r, const double eps)
{
	// get the box in which this node lies
	int nb = FindBox(r);
	assert((nb >= 0)&&(nb<(int)m_BL.size()));
	OBOX& b = m_BL[nb];

	// see if this node is already in this box
	int N = (int)b.NL.size();
	for (int i=0; i<N; ++i)
	{
		NODE& ni = m_Node[b.NL[i]];
		vec3d& ri = ni.r;
		if ((ri - r)*(ri - r) < eps) return ni.n;
	}

	N = (int)m_Node.size();
	NODE nd;
	nd.r = r;
	nd.n = N;
	m_Node.push_back(nd);
	b.NL.push_back(nd.n);
	return nd.n;
}

//-----------------------------------------------------------------------------
BOX FESTLimport::BoundingBox()
{
	list<FACET>::iterator pf = m_Face.begin();
	vec3d r = pf->r[0];
	BOX b(r, r);
	for (pf = m_Face.begin(); pf != m_Face.end(); ++pf)
	{
		vec3d* r = pf->r;
		b += r[0];
		b += r[1];
		b += r[2];
	}
	return b;
}
