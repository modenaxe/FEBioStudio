#include "stdafx.h"
#include "GModifier.h"
#include <GeomLib/GObject.h>
#include <MeshLib/FENodeFaceList.h>
#include <MeshLib/FENodeNodeList.h>
#include <MeshLib/MeshTools.h>

//-----------------------------------------------------------------------------
GWrapModifier::GWrapModifier()
{
	m_po = 0;
	AddIntParam(-1, "target ID", "target ID");
	AddIntParam(0, "method", "method")->SetEnumNames("Closest point \0Closest surface point\0");
	AddIntParam(1, "steps", "steps");
}

//-----------------------------------------------------------------------------
void GWrapModifier::Apply(GObject* po)
{
	// make sure there is a target
	if (m_po == 0) return;

	FEMesh* pm = po->GetFEMesh();

	int i, j, k;

	// get the object ID
	int id = GetIntValue(TRG_ID);

	// find the object
	if (m_po == 0) 
	{
		SetIntValue(TRG_ID, -1);
		return;
	}

	// create the displacement vector
	vector<vec3d> DS;
	DS.assign(pm->Nodes(), vec3d(0,0,0));

	// tag all external nodes
	vector<int> tag; tag.assign(pm->Nodes(), 0);
	for (i=0; i<pm->Faces(); ++i)
	{
		FEFace& f = pm->Face(i);
		int nf = f.Nodes();
		for (j=0; j<nf; ++j) tag[f.n[j]] = 1;
	}

	// get the number of steps of the normal projection
	int nsteps = GetIntValue(NSTEPS);
	if (nsteps < 1) nsteps = 1;

	// get the method of application
	int ntype = GetIntValue(METHOD);
	switch (ntype)
	{
	case 0: // normal projection
		NormalProjection(pm, DS, tag, nsteps); break;
	case 1:	// closest point
		ClosestPoint(pm, DS, tag); break;
	default:
		assert(false);
	}

	// see if there are any exterior nodes
	int next = 0;
	for (i=0; i<pm->Nodes(); ++i) if (tag[i] == 0) next++;

	if (next > 0)
	{
		// calculate the average displacement
		vec3d d;
		int n = 0;
		for (i=0; i<pm->Nodes(); ++i)
		{
			if (tag[i] == 1)
			{
				d += DS[i];
				n++;
			}
		}
		if (n != 0) d /= (double) n;

		// assign the average displacement as initial value
		for (i=0; i<pm->Nodes(); ++i)
		{
			if (tag[i] == 0) DS[i] = d;
		}

		// create Node-Node list
		FENodeNodeList NNL(pm);

		// apply smoothing
		int niter = 1;
		for (n=0; n<niter; ++n)
		{
			for (i=0; i<pm->Nodes(); ++i)
			{
				if (tag[i] == 0)
				{
					vec3d d;
					int nval = NNL.Valence(i);
					for (k=0; k<nval; ++k)
					{
						int nn = NNL.Node(i, k);
						d += DS[nn];
					}
					if (nval != 0) d /= (double) nval;
					DS[i] = d;
				}
			}
		}
	}

	// apply the displacement
	for (i=0; i<pm->Nodes(); ++i)
	{
		FENode& n = pm->Node(i);
		n.r += DS[i];
	}

	// update the mesh
	pm->Update();
}

//-----------------------------------------------------------------------------
void GWrapModifier::ClosestPoint(FEMesh *pm, vector<vec3d>& DS, vector<int>& tag)
{
	// get the target mesh
	FEMesh* ptrg = m_po->GetFEMesh();

	// create the node-face list for the target mesh
	FENodeFaceList NFL(ptrg);
	NFL.Build();

	// get the source object
	GObject* ps = pm->GetGObject();

/*	for (int i=0; i<pm->Nodes(); ++i)
	{
		if (tag[i] == 1)
		{
			FENode& n = pm->Node(i);

			vec3d nr = ps->Transform().LocalToGlobal(n.r);
			nr = m_po->Transform().GlobalToLocal(nr);

			// find the closest node
			int imin = 0;
			vec3d dr = NFL.Node(0).r - nr;
			double dmin = dr*dr, d;
			for (int j=1; j<NFL.Nodes(); ++j)
			{
				dr = NFL.Node(j).r - nr;
				d = dr*dr;
				if (d < dmin)
				{
					imin = j;
					dmin = d;
				}
			}

			// project to faces
			int nval = NFL.Valence(imin);
			dmin = 1e99;
			for (int j=0; j<nval; ++j)
			{
				FEFace& f = *NFL.Face(imin, j);
				double r = 0, s = 0;
				vec3d q = ProjectToFace(*ptrg, nr, f, r, s);

				vec3d dr = nr - q;
				if (dr*dr < dmin)
				{
					q = m_po->Transform().LocalToGlobal(q);
					DS[i] = ps->Transform().GlobalToLocal(q) - n.r;
					dmin = dr*dr;
				}
			}
		}
	}
*/
}

//-----------------------------------------------------------------------------
void GWrapModifier::NormalProjection(FEMesh *pm, vector<vec3d>& DS, vector<int>& tag, int nsteps)
{
	int i, j;

	// get the target mesh
	FEMesh* ptrg = m_po->GetFEMesh();

	// get the source object
	GObject* ps = pm->GetGObject();

	// store the original positions of the mesh
	vector<vec3d> r0; r0.resize(pm->Nodes());
	for (i=0; i<pm->Nodes(); ++i) r0[i] = pm->Node(i).r;

	// calculate node normals
	for (int ns = 0; ns < nsteps; ++ns)
	{
		vector<vec3d> N; 
		N.assign(pm->Nodes(), vec3d(0,0,0));
		for (i=0; i<pm->Nodes(); ++i) pm->Node(i).m_ntag = 0;
		for (i=0; i<pm->Faces(); ++i)
		{
			FEFace& f = pm->Face(i);
			for (j=0; j<f.Nodes(); ++j) 
			{
				N[f.n[j]] += f.m_nn[j];
				pm->Node(f.n[j]).m_ntag = 1;
			}
		}
		for (i=0; i<pm->Nodes(); ++i)
		{
			if (pm->Node(i).m_ntag == 1)
			{
				vec3d r = ps->Transform().LocalToGlobalNormal(N[i]);
				N[i] = m_po->Transform().GlobalToLocalNormal(r);
				N[i].Normalize();
			}
		}

		// loop over all nodes
		vec3d q;
		double g;
		for (i=0; i<pm->Nodes(); ++i)
		{
			if (tag[i] == 1)
			{	
				FENode& node = pm->Node(i);

				// get the normal
				vec3d n = N[i];

				// get the global position
				vec3d r = ps->Transform().LocalToGlobal(node.r);
				r = m_po->Transform().GlobalToLocal(r);

				// loop over all faces of the target mesh
				double dmin = 1e99, d;
				for (j=0; j<ptrg->Faces(); ++j)
				{
					FEFace& f = ptrg->Face(j);

					// find the intersection of the ray with this face
					if (FindIntersection(*ptrg, f, r, n, q, g))
					{	
						d = (q-r)*(q-r);
						if (d < dmin)
						{
							q = m_po->Transform().LocalToGlobal(q);
							DS[i] = ps->Transform().GlobalToLocal(q) - node.r;
							dmin = d;
						}
					}
				}
			}
		}

		for (i=0; i<pm->Nodes(); ++i)
		{
			if (tag[i] == 1)
			{
				DS[i] /= (nsteps - ns);
				pm->Node(i).r += DS[i];
			}
		}
		if (ns != nsteps - 1) pm->UpdateNormals();
	}

	// restore original nodal position
	for (i=0; i<pm->Nodes(); ++i)
	{
		FENode& n = pm->Node(i);
		DS[i] = n.r - r0[i];
		n.r = r0[i];
	}
}
