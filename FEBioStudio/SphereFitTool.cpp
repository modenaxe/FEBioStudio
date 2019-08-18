#include "stdafx.h"
#include "SphereFitTool.h"
#include <QWidget>
#include <QBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
#include "Document.h"
#include <MeshTools/SphereFit.h>

// constructor
CSphereFitTool::CSphereFitTool() : CBasicTool("Sphere Fit", HAS_APPLY_BUTTON)
{
	m_bsel = false;
	m_x = m_y = m_z = 0.0;
	m_R = 0.0;
	m_obj = 0;
	addBoolProperty(&m_bsel, "selection only");
	addDoubleProperty(&m_x, "x")->setFlags(CProperty::Visible);
	addDoubleProperty(&m_y, "y")->setFlags(CProperty::Visible);
	addDoubleProperty(&m_z, "z")->setFlags(CProperty::Visible);
	addDoubleProperty(&m_R, "R")->setFlags(CProperty::Visible);
	addDoubleProperty(&m_obj, "obj")->setFlags(CProperty::Visible);

	SetApplyButtonText("Fit");
}

bool CSphereFitTool::OnApply()
{
	// get the nodal coordinates (surface only)
	CDocument* doc = GetDocument();
	if (doc && doc->IsValid())
	{
		GObject* po = doc->GetActiveObject();
		if ((po == 0) || (po->GetFEMesh() == 0)) 
		{
			SetErrorString("You must select an object that has a mesh.");
			return false;
		}

		FEMesh& mesh = *po->GetFEMesh();

		int N = mesh.Nodes();
		int F = mesh.Faces();
		for (int i=0; i<N; ++i) mesh.Node(i).m_ntag = 0;
		for (int i=0; i<F; ++i)
		{
			FEFace& f = mesh.Face(i);
			if ((m_bsel == false) || (f.IsSelected()))
			{
				int nf = f.Nodes();
				for (int j=0; j<nf; ++j) mesh.Node(f.n[j]).m_ntag = 1;
			}
		}

		vector<vec3d> y;
		for (int i=0; i<N; ++i)
		{
			if (mesh.Node(i).m_ntag == 1) y.push_back(po->Transform().LocalToGlobal(mesh.Node(i).r));
		}

		// find the best fit sphere
		SphereFit fit;
		fit.Fit(y, 50);
		vec3d sc = fit.m_rc;
		double R = fit.m_R;

		// calculate the objective function
		double objs = fit.ObjFunc(y);

		// update GUI
		m_x = sc.x;
		m_y = sc.y;
		m_z = sc.z;
		m_R = R;
		m_obj = objs;

		// set this position as the 3D cursor
		doc->Set3DCursor(sc);
	}

	return true;
}
