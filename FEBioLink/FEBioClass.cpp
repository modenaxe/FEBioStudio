/*This file is part of the FEBio Studio source code and is licensed under the MIT license
listed below.

See Copyright-FEBio-Studio.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
#include "FEBioClass.h"
#include "FEBioModule.h"
#include <FECore/FECoreKernel.h>
#include <FECore/FECoreBase.h>
#include <FECore/FEModelParam.h>
#include <FECore/FEModule.h>
#include <FEBioLib/FEBioModel.h>
#include <MeshTools/FEModel.h>
#include <FEMLib/FEStepComponent.h>
#include <FEMLib/FEBoundaryCondition.h>
#include <FEMLib/FESurfaceLoad.h>
#include <FEMLib/FEBodyLoad.h>
#include <FEMLib/FEMaterial.h>
#include <FEMLib/FEDiscreteMaterial.h>
#include <FEMLib/FEAnalysisStep.h>
#include <FEMLib/FEInterface.h>
#include <FEMLib/FEModelConstraint.h>
#include <FEMLib/FEConnector.h>
#include <FEMLib/FERigidLoad.h>
#include <FEMLib/FERigidConstraint.h>
#include <FEMLib/FEInitialCondition.h>
#include <FEMLib/FEDiscreteMaterial.h>
#include <FEMLib/FEElementFormulation.h>
#include <MeshTools/FEModel.h>
#include <sstream>
using namespace FEBio;

std::map<int, const char*> idmap;
void initMap()
{
	idmap.clear();

	idmap[FEINVALID_ID             ] = "FEINVALID_ID";
	idmap[FEOBJECT_ID              ] = "FEOBJECT_ID";
	idmap[FETASK_ID                ] = "FETASK_ID";
	idmap[FESOLVER_ID              ] = "FESOLVER_ID";
	idmap[FEMATERIAL_ID            ] = "FEMATERIAL_ID";
	idmap[FEMATERIALPROP_ID        ] = "FEMATERIALPROP_ID";
	idmap[FELOAD_ID                ] = "FELOAD_ID";
	idmap[FENLCONSTRAINT_ID        ] = "FENLCONSTRAINT_ID";
	idmap[FEPLOTDATA_ID            ] = "FEPLOTDATA_ID";
	idmap[FEANALYSIS_ID            ] = "FEANALYSIS_ID";
	idmap[FESURFACEINTERFACE_ID    ] = "FESURFACEINTERFACE_ID";
	idmap[FENODELOGDATA_ID         ] = "FENODELOGDATA_ID";
	idmap[FEFACELOGDATA_ID         ] = "FEFACELOGDATA_ID";
	idmap[FEELEMLOGDATA_ID         ] = "FEELEMLOGDATA_ID";
	idmap[FEOBJLOGDATA_ID          ] = "FEOBJLOGDATA_ID";
	idmap[FEBC_ID                  ] = "FEBC_ID";
	idmap[FEGLOBALDATA_ID          ] = "FEGLOBALDATA_ID";
	idmap[FERIGIDOBJECT_ID         ] = "FERIGIDOBJECT_ID";
	idmap[FENLCLOGDATA_ID          ] = "FENLCLOGDATA_ID";
	idmap[FECALLBACK_ID            ] = "FECALLBACK_ID";
	idmap[FESOLIDDOMAIN_ID         ] = "FESOLIDDOMAIN_ID";
	idmap[FESHELLDOMAIN_ID         ] = "FESHELLDOMAIN_ID";
	idmap[FETRUSSDOMAIN_ID         ] = "FETRUSSDOMAIN_ID";
	idmap[FEDOMAIN2D_ID            ] = "FEDOMAIN2D_ID";
	idmap[FEDISCRETEDOMAIN_ID      ] = "FEDISCRETEDOMAIN_ID";
	idmap[FESURFACE_ID             ] = "FESURFACE_ID";
	idmap[FEIC_ID                  ] = "FEIC_ID";
	idmap[FENODEDATAGENERATOR_ID   ] = "FENODEDATAGENERATOR_ID";
	idmap[FEFACEDATAGENERATOR_ID   ] = "FEFACEDATAGENERATOR_ID";
	idmap[FEELEMDATAGENERATOR_ID   ] = "FEELEMDATAGENERATOR_ID";
	idmap[FELOADCONTROLLER_ID      ] = "FELOADCONTROLLER_ID";
	idmap[FEMODEL_ID               ] = "FEMODEL_ID";
	idmap[FEMODELDATA_ID           ] = "FEMODELDATA_ID";
	idmap[FESCALARGENERATOR_ID     ] = "FESCALARGENERATOR_ID";
	idmap[FEVEC3DGENERATOR_ID      ] = "FEVEC3DGENERATOR_ID";
	idmap[FEMAT3DGENERATOR_ID      ] = "FEMAT3DGENERATOR_ID";
	idmap[FEMAT3DSGENERATOR_ID     ] = "FEMAT3DSGENERATOR_ID";
	idmap[FEFUNCTION1D_ID          ] = "FEFUNCTION1D_ID";
	idmap[FELINEARSOLVER_ID        ] = "FELINEARSOLVER_ID";
	idmap[FEMESHADAPTOR_ID         ] = "FEMESHADAPTOR_ID";
	idmap[FEMESHADAPTORCRITERION_ID] = "FEMESHADAPTORCRITERION_ID";
	idmap[FERIGIDBC_ID             ] = "FERIGIDBC_ID";
	idmap[FENEWTONSTRATEGY_ID      ] = "FENEWTONSTRATEGY_ID";
	idmap[FETIMECONTROLLER_ID      ] = "FETIMECONTROLLER_ID";
	idmap[FEEIGENSOLVER_ID         ] = "FEEIGENSOLVER_ID";
	idmap[FEDATARECORD_ID          ] = "FEDATARECORD_ID";
}

// dummy model used for allocating temporary FEBio classes.
static FEBioModel* febioModel = nullptr;

static std::map<std::string, int> classIndex;

int baseClassIndex(const char* sz)
{
	if (sz == nullptr) return -1;

	int n = -1;
	auto it = classIndex.find(sz);
	if (it == classIndex.end())
	{
		n = classIndex.size();
		classIndex[sz] = n;
	}
	else n = classIndex[sz];

	return n;
}

int FEBio::GetBaseClassIndex(const std::string& baseClassName)
{
	return baseClassIndex(baseClassName.c_str());
}

std::string FEBio::GetBaseClassName(int baseClassIndex)
{
	for (auto& it : classIndex)
	{
		if (it.second == baseClassIndex) return it.first;
	}
	return std::string();
}

bool in_vector(const vector<int>& v, int n)
{
	for (int j = 0; j < v.size(); ++j)
	{
		if (v[j] == n) return true;
	}
	return false;
}

FEBioClassInfo FEBio::GetClassInfo(int classId)
{
	FECoreKernel& fecore = FECoreKernel::GetInstance();
	const FECoreFactory* fac = fecore.GetFactoryClass(classId); assert(fac);

	int modId = fac->GetModuleID();

	FEBioClassInfo ci;
	ci.classId = classId;
	ci.baseClassId = baseClassIndex(fac->GetBaseClassName());
	ci.sztype = fac->GetTypeStr();
	ci.szmod = fecore.GetModuleName(modId);

	return ci;
}

std::vector<FEBio::FEBioClassInfo> FEBio::FindAllActiveClasses(int superId, int baseClassId, unsigned int flags)
{
	FECoreKernel& fecore = FECoreKernel::GetInstance();
	return FindAllClasses(fecore.GetActiveModuleID(), superId, baseClassId, flags);
}

std::vector<FEBio::FEBioClassInfo> FEBio::FindAllClasses(int mod, int superId, int baseClassId, unsigned int flags)
{
	vector<FEBio::FEBioClassInfo> facs;

	bool includeModuleDependencies = (flags & ClassSearchFlags::IncludeModuleDependencies);
	bool includeFECoreClasses = includeModuleDependencies;// (flags & ClassSearchFlags::IncludeFECoreClasses);

	FECoreKernel& fecore = FECoreKernel::GetInstance();
	vector<int> mods;
	if ((mod != -1) && includeModuleDependencies)
	{
		mods = fecore.GetModuleDependencies(mod - 1);
	}
	if ((mod != -1) && includeFECoreClasses) 
	{
		mods.push_back(0);
	}

	for (int i = 0; i < fecore.FactoryClasses(); ++i)
	{
		const FECoreFactory* fac = fecore.GetFactoryClass(i);
		int facmod = fac->GetModuleID();

		int baseId = baseClassIndex(fac->GetBaseClassName());

		if (((superId     == -1) || (fac->GetSuperClassID() == superId)) && 
			((baseClassId == -1) || (baseId == baseClassId)))
		{
			if ((mod == -1) || (mod == facmod) || in_vector(mods, facmod))
			{
				const char* szmod = fecore.GetModuleName(fac->GetModuleID() - 1);
				FEBio::FEBioClassInfo febc = { 
					(unsigned int)i, 
					baseClassIndex(fac->GetBaseClassName()),
					fac->GetTypeStr(), 
					szmod };
				facs.push_back(febc);
			}
		}
	}

	return facs;
}

int FEBio::GetClassId(int superClassId, const std::string& typeStr)
{
	FECoreKernel& fecore = FECoreKernel::GetInstance();
	return fecore.GetFactoryIndex(superClassId, typeStr.c_str());
}

// TODO: I don't think the allocated class is ever deallocated! MEMORY LEAK!!
FECoreBase* CreateFECoreClass(int classId)
{
	// Get the kernel
	FECoreKernel& fecore = FECoreKernel::GetInstance();

	// find the factory
	const FECoreFactory* fac = fecore.GetFactoryClass(classId);
	if (fac == nullptr) return nullptr;

	// try to create the FEBio object
	assert(febioModel);
	FECoreBase* pc = fac->CreateInstance(febioModel); assert(pc);

	// all done!
	return pc;
}

FSModelComponent* CreateFSClass(int superClassID, int baseClassId, FSModel* fem)
{
	FSModelComponent* pc = nullptr;
	switch (superClassID)
	{
	case FEANALYSIS_ID  : pc = new FEBioAnalysisStep(fem); break;
	case FEFUNCTION1D_ID: pc = new FEBioFunction1D(fem); break;
	case FEMATERIAL_ID  : pc = new FEBioMaterial(); break;
	case FEBC_ID:
	{
		FEBioBoundaryCondition* pbc = new FEBioBoundaryCondition(fem);
		if      (baseClassId == FEBio::GetBaseClassIndex("FENodalBC"  )) pbc->SetMeshItemType(FE_NODE_FLAG);
		else if (baseClassId == FEBio::GetBaseClassIndex("FESurfaceBC")) pbc->SetMeshItemType(FE_FACE_FLAG);
		pc = pbc;
	}
	break;
	case FEIC_ID: pc = new FEBioInitialCondition(fem); break;
	case FESURFACEINTERFACE_ID: pc = new FEBioInterface(fem); break;
	case FELOAD_ID:
	{
		if      (baseClassId == FEBio::GetBaseClassIndex("FENodalLoad"  )) pc = new FEBioNodalLoad(fem);
		else if (baseClassId == FEBio::GetBaseClassIndex("FESurfaceLoad")) pc = new FEBioSurfaceLoad(fem);
		else if (baseClassId == FEBio::GetBaseClassIndex("FEBodyLoad"   )) pc = new FEBioBodyLoad(fem);
		else if (baseClassId == FEBio::GetBaseClassIndex("FERigidLoad"  )) pc = new FEBioRigidLoad(fem);
		else assert(false);
	}
	break;
	case FEMATERIALPROP_ID     : pc = new FEBioMaterialProperty(fem); break;
	case FELOADCONTROLLER_ID   : pc = new FEBioLoadController(fem); break;
	case FEMESHADAPTOR_ID      : pc = new FEBioMeshAdaptor(fem); break;
	case FENLCONSTRAINT_ID     : 
	{
		if      (baseClassId == FEBio::GetBaseClassIndex("FESurfaceConstraint")) pc = new FEBioSurfaceConstraint(fem);
		else if (baseClassId == FEBio::GetBaseClassIndex("FEBodyConstraint"   )) pc = new FEBioBodyConstraint(fem);
		else pc = new FEBioNLConstraint(fem); break;
	}
	break;
	case FENODEDATAGENERATOR_ID : pc = new FEBioNodeDataGenerator(fem); break;
	case FEFACEDATAGENERATOR_ID : pc = new FEBioFaceDataGenerator(fem); break;
	case FEELEMDATAGENERATOR_ID : pc = new FEBioElemDataGenerator(fem); break;
	case FESOLVER_ID           : pc = new FSGenericClass; break;
	case FERIGIDBC_ID		   : pc = new FEBioRigidConstraint(fem); break;
	case FEMESHADAPTORCRITERION_ID: pc = new FSGenericClass; break;
	case FENEWTONSTRATEGY_ID  : pc = new FSGenericClass; break;
	case FECLASS_ID           : pc = new FSGenericClass; break;
	case FETIMECONTROLLER_ID  : pc = new FSGenericClass; break;
	case FEVEC3DGENERATOR_ID  : pc = new FSGenericClass; break;
	case FEMAT3DGENERATOR_ID  : pc = new FSGenericClass; break;
	case FEMAT3DSGENERATOR_ID : pc = new FSGenericClass; break;
	default:
		assert(false);
	}
	assert(pc);

	pc->SetSuperClassID(superClassID);

	return pc;
}


bool BuildModelComponent(FSModelComponent* po, FECoreBase* feb)
{
	if (po->GetSuperClassID() != FECLASS_ID)
	{
		assert(po->GetSuperClassID() == feb->GetSuperClassID());
		po->SetTypeString(feb->GetTypeStr());

		// make sure the class ID is set on the model component
		if (po->GetClassID() <= 0)
		{
			int classId = FEBio::GetClassId(feb->GetSuperClassID(), feb->GetTypeStr());
			po->SetClassID(classId);
		}
	}

	// map the FECoreBase parameters to the FSModelComponent
	// copy the parameters from the FEBioClass to the FSObject
	FEParameterList& PL = feb->GetParameterList();

	// copy parameter groups
	ParamBlock& PB = po->GetParamBlock();
	PB.ClearParamGroups();
	for (int i = 0; i < PL.ParameterGroups(); ++i)
	{
		PB.SetActiveGroup(PL.GetParameterGroupName(i));
	}
	PB.SetActiveGroup(nullptr);

	const int params = PL.Parameters();
	FEParamIterator pi = PL.first();
	for (int i = 0; i < params; ++i, ++pi)
	{
		FEParam& param = *pi;

		PB.SetActiveGroup(param.GetParamGroup());

		if ((param.GetFlags() & FEParamFlag::FE_PARAM_HIDDEN) == 0)
		{
			int ndim = param.dim();
			int type = param.type();

			// TODO: The name needs to be copied in the FSObject class!! 
			//       This is a memory leak!!!!
			const char* szname = strdup(param.name());
			const char* szlongname = strdup(param.longName());
			Param* p = nullptr;
			switch (type)
			{
			case FEBio::FEBIO_PARAM_INT:
			{
				int n = param.value<int>();
				if (param.enums())
				{
					p = po->AddChoiceParam(n, szname, szlongname);
					p->CopyEnumNames(param.enums());
				}
				else p = po->AddIntParam(n, szname, szlongname);
			}
			break;
			case FEBio::FEBIO_PARAM_BOOL: p = po->AddBoolParam(param.value<bool>(), szname, szlongname); break;
			case FEBio::FEBIO_PARAM_DOUBLE: p = po->AddDoubleParam(param.value<double>(), szname, szlongname); break;
			case FEBio::FEBIO_PARAM_VEC3D: p = po->AddVecParam(param.value<vec3d>(), szname, szlongname); break;
			case FEBio::FEBIO_PARAM_MAT3D: p = po->AddMat3dParam(param.value<mat3d>(), szname, szlongname); break;
			case FEBio::FEBIO_PARAM_STD_STRING: p = po->AddStringParam(param.value<string>(), szname, szlongname); break;
			case FEBio::FEBIO_PARAM_STD_VECTOR_INT:
			{
				std::vector<int> val = param.value<std::vector<int> >();
				p = po->AddVectorIntParam(val, szname, szlongname);
				if (param.enums()) p->CopyEnumNames(param.enums());
			}
			break;
			case FEBio::FEBIO_PARAM_STD_VECTOR_DOUBLE:
			{
				std::vector<double> val = param.value<std::vector<double> >();
				p = po->AddVectorDoubleParam(val, szname, szlongname);
			}
			break;
			case FEBio::FEBIO_PARAM_STD_VECTOR_VEC2D:
			{
				std::vector<vec2d> val = param.value<std::vector<vec2d> >();
				p = po->AddVectorVec2dParam(val, szname, szlongname);
			}
			break;
			case FEBio::FEBIO_PARAM_DOUBLE_MAPPED:
			{
				if (ndim == 1)
				{
					FEParamDouble& v = param.value<FEParamDouble>();
					double d = 0.0;
					if (v.isConst()) d = v.constValue();
					p = po->AddDoubleParam(d, szname, szlongname)->MakeVariable(true);
				}
				else if (ndim == 3)
				{
					vec3d v(0, 0, 0);
					v.x = param.value<FEParamDouble>(0).constValue();
					v.y = param.value<FEParamDouble>(1).constValue();
					v.z = param.value<FEParamDouble>(2).constValue();
					p = po->AddVecParam(v, szname, szlongname);
				}
				else assert(false);
			}
			break;
			case FEBio::FEBIO_PARAM_VEC3D_MAPPED:
			{
				FEParamVec3& v = param.value<FEParamVec3>();
				FEVec3dValuator* val = v.valuator(); assert(val);

				FSProperty* prop = po->AddProperty(param.name(), baseClassIndex("FEVec3dValuator"));
				prop->SetSuperClassID(FEVEC3DGENERATOR_ID);
				prop->SetDefaultType("vector");

				FSModelComponent* vecProp = CreateFSClass(FEVEC3DGENERATOR_ID, -1, po->GetFSModel());

				// copy the class data
				BuildModelComponent(vecProp, val);

				prop->AddComponent(vecProp);
			}
			break;
			case FEBio::FEBIO_PARAM_MAT3D_MAPPED:
			{
				FEParamMat3d& v = param.value<FEParamMat3d>();
				FEMat3dValuator* val = v.valuator(); assert(val);

				FSProperty* prop = po->AddProperty(param.name(), baseClassIndex("FEMat3dValuator"));
				prop->SetSuperClassID(FEMAT3DGENERATOR_ID);
				prop->SetDefaultType("const");

				FSModelComponent* matProp = CreateFSClass(FEMAT3DGENERATOR_ID, -1, po->GetFSModel());

				// copy the class data
				BuildModelComponent(matProp, val);

				prop->AddComponent(matProp);
			}
			break;
			case FEBio::FEBIO_PARAM_MAT3DS_MAPPED:
			{
				FEParamMat3ds& v = param.value<FEParamMat3ds>();
				FEMat3dsValuator* val = v.valuator(); assert(val);

				FSProperty* prop = po->AddProperty(param.name(), baseClassIndex("FEMat3dsValuator"));
				prop->SetSuperClassID(FEMAT3DSGENERATOR_ID);
				prop->SetDefaultType("const");

				FSModelComponent* matProp = CreateFSClass(FEMAT3DSGENERATOR_ID, -1, po->GetFSModel());

				// copy the class data
				BuildModelComponent(matProp, val);

				prop->AddComponent(matProp);
			}
			break;
			default:
				assert(false);
			}

			// p can be null if parameters are mapped to properties (e.g. the mapped parameters)
			if (p)
			{
				p->SetFlags(param.GetFlags());
				if (param.units()) p->SetUnit(param.units());
			}
		}
	}

	// map the properties
	for (int i = 0; i < feb->PropertyClasses(); ++i)
	{
		FEProperty& prop = *feb->PropertyClass(i);

		int maxSize = (prop.IsArray() ? 0 : 1);
		int baseClassId = (prop.GetClassName() ? baseClassIndex(prop.GetClassName()) : -1);
		FSProperty* fsp = po->AddProperty(prop.GetName(), baseClassId, maxSize); assert(fsp);

		fsp->SetLongName(prop.GetLongName());

		fsp->SetSuperClassID(prop.GetSuperClassID());
		if (prop.IsRequired())
			fsp->SetFlags(fsp->GetFlags() | FSProperty::REQUIRED);
		if (prop.IsPreferred())
			fsp->SetFlags(fsp->GetFlags() | FSProperty::PREFERRED);
		
		// set the (optional) default type
		if (prop.GetDefaultType())
			fsp->SetDefaultType(prop.GetDefaultType());

		// for solvers we need to set the default type to the module name
		if (prop.GetSuperClassID() == FESOLVER_ID)
		{
			int activeMod = GetActiveModule(); assert(activeMod >= 0);
			const char* szmod = GetModuleName(activeMod); assert(szmod);
			fsp->SetDefaultType(szmod);
		}

		// handle mesh selection properties differently
		if (prop.GetSuperClassID() == FESURFACE_ID)
		{
			FSMeshSelection* pms = new FSMeshSelection(po->GetFSModel());
			pms->SetMeshItemType(FE_FACE_FLAG);
			pms->SetSuperClassID(FESURFACE_ID);
			fsp->AddComponent(pms);
		}
/*		else if (prop.GetSuperClassID() == FEITEMLIST_ID)
		{
			FSMeshSelection* pms = new FSMeshSelection(po->GetFSModel());
			if (strcmp(prop.GetName(), "node_set") == 0) pms->SetMeshItemType(FE_NODE_FLAG);

			// TODO: We need to integrate these IDs.
			fsp->SetSuperClassID(FEDOMAIN_ID);
			pms->SetSuperClassID(FEDOMAIN_ID);
			fsp->AddComponent(pms);
		}
*/		else if (prop.size() != 0)
		{
			FECoreBase* pci = prop.get(0);

			// make sure the property is either a FECLASS_ID, which is not allocated through the kernel
			// or the super IDs match.
			assert((prop.GetSuperClassID() == FECLASS_ID) || (pci->GetSuperClassID() == prop.GetSuperClassID()));

			// allocate the model component
			FSModelComponent* pmi = CreateFSClass(prop.GetSuperClassID(), -1, nullptr); assert(pmi);
			BuildModelComponent(pmi, pci);
			fsp->AddComponent(pmi);
		}
	}

	if (dynamic_cast<FEBioMaterial*>(po))
	{
		FEBioMaterial* febMat = dynamic_cast<FEBioMaterial*>(po);
		//		febMat->SetFEBioMaterial(feb);
	}
	else if (dynamic_cast<FSDomainComponent*>(po))
	{
		FSDomainComponent* pbc = dynamic_cast<FSDomainComponent*>(po);
		if (feb->FindProperty("surface"))
		{
			pbc->SetMeshItemType(FE_FACE_FLAG);
		}
	}

	return true;
}

bool FEBio::BuildModelComponent(FSModelComponent* po)
{
	// create the FEBio class
	int classId = po->GetClassID();
	FECoreBase* feb = CreateFECoreClass(classId);
	if (feb == nullptr) return false;
	return BuildModelComponent(po, feb);
}

vector<FEBio::FEBioModule>	FEBio::GetAllModules()
{
	// Get the kernel
	FECoreKernel& fecore = FECoreKernel::GetInstance();

	vector<FEBio::FEBioModule> mods;
	for (int i = 0; i < fecore.Modules(); ++i)
	{
		FEBio::FEBioModule mod;
		mod.m_szname = fecore.GetModuleName(i);
		mod.m_szdesc = fecore.GetModuleDescription(i);
		mod.m_id = i + 1;
		mods.push_back(mod);
	}

	return mods;
}

const char* FEBio::GetModuleName(int moduleId)
{
	FECoreKernel& fecore = FECoreKernel::GetInstance();
	const char* szmodName = fecore.GetModuleNameFromId(moduleId); assert(szmodName);
	return szmodName;
}

int FEBio::GetModuleId(const std::string& moduleName)
{
	vector<FEBio::FEBioModule> modules = GetAllModules();
	for (int i = 0; i < modules.size(); ++i)
	{
		FEBioModule& mod = modules[i];
		string modName(mod.m_szname);
		if (modName == moduleName) return mod.m_id;
	}
	return -1;
}

void FEBio::SetActiveModule(int moduleID)
{
	// create a new model
	delete febioModel; febioModel = new FEBioModel;

	FECoreKernel& fecore = FECoreKernel::GetInstance();
	fecore.SetActiveModule(moduleID);

	fecore.GetActiveModule()->InitModel(febioModel);
}

int FEBio::GetActiveModule()
{
	FECoreKernel& fecore = FECoreKernel::GetInstance();
	return fecore.GetActiveModuleID();
}

void FEBio::InitFSModel(FSModel& fem)
{
	// make sure we have a FEBioModel
	if (febioModel == nullptr) { assert(false); return; }

	// clear the current list of variables
	fem.ClearVariables();

	// copy all variables
	DOFS& dofs = febioModel->GetDOFS();
	for (int i = 0; i < dofs.Variables(); ++i)
	{
		std::string varName = dofs.GetVariableName(i);
		FEDOFVariable* dofi = fem.AddVariable(varName);
		int ndof = dofs.GetVariableSize(i);
		for (int j = 0; j < ndof; ++j)
		{
			const char* szdof = dofs.GetDOFName(i, j);
			if (ndof == 1)
			{
				dofi->AddDOF(varName, szdof);
			}
			else
			{
				std::stringstream ss;
				ss << szdof << "-" << varName;
				dofi->AddDOF(ss.str(), szdof);
			}
		}
	}
}

class FBSLogStream : public LogStream
{
public:
	FBSLogStream(FEBioOutputHandler* outputHandler) : m_outputHandler(outputHandler) {}

	void print(const char* sz) override
	{
		if (m_outputHandler) m_outputHandler->write(sz);
	}

private:
	FEBioOutputHandler* m_outputHandler;
};

static bool terminateRun = false;

bool interrup_cb(FEModel* fem, unsigned int nwhen, void* pd)
{
	if (terminateRun)
	{
		terminateRun = false;
        throw std::exception();
	}
	return true;
}

void FEBio::TerminateRun()
{
	terminateRun = true;
}

bool FEBio::runModel(const std::string& fileName, FEBioOutputHandler* outputHandler)
{
	terminateRun = false;

	FEBioModel fem;

	// attach the output handler
	if (outputHandler)
	{
		fem.GetLogFile().SetLogStream(new FBSLogStream(outputHandler));
	}

	// attach a callback to interrupt
	fem.AddCallback(interrup_cb, CB_ALWAYS, nullptr);

	try {

		// try to read the input file
		if (fem.Input(fileName.c_str()) == false)
		{
			return false;
		}

		// do model initialization
		if (fem.Init() == false)
		{
			return false;
		}

		// solve the model
		return fem.Solve();
	}
	catch (...)
	{

	}

	return false;
}

const char* FEBio::GetSuperClassString(int superClassID)
{
	if (idmap.empty()) initMap();
	if (idmap.find(superClassID) != idmap.end())
		return idmap[superClassID];
	else
		return nullptr;
}

std::map<int, const char*> FEBio::GetSuperClassMap()
{
	if (idmap.empty()) initMap();
	return idmap;
}

vec3d FEBio::GetMaterialFiber(void* vec3dvaluator, const vec3d& p)
{
	FECoreBase* pc = (FECoreBase*)vec3dvaluator;
	FEVec3dValuator* val = dynamic_cast<FEVec3dValuator*>(pc); assert(val);
	if (val == nullptr) return vec3d(0,0,0);
	FEMaterialPoint mp;
	mp.m_r0 = mp.m_rt = p;
	vec3d v = (*val)(mp);
	v.unit();
	return v; 
}

void FEBio::DeleteClass(void* p)
{
	FECoreBase* pc = (FECoreBase*)p;
	delete pc;
}

// Call this to initialize default properties
bool FEBio::InitDefaultProps(FSModelComponent* pc)
{
	for (int i = 0; i < pc->Properties(); ++i)
	{
		FSProperty& prop = pc->GetProperty(i);
		if ((prop.IsRequired() || prop.IsPreferred()) && (prop.GetComponent() == nullptr))
		{
			vector<FEBio::FEBioClassInfo> fci = FEBio::FindAllActiveClasses(prop.GetSuperClassID(), -1, FEBio::ClassSearchFlags::IncludeFECoreClasses | FEBio::ClassSearchFlags::IncludeModuleDependencies);
			if ((fci.size() > 0) && (prop.GetDefaultType().empty() == false))
			{
				FSModel* fem = pc->GetFSModel();
				FSModelComponent* psc = FEBio::CreateClass(prop.GetSuperClassID(), prop.GetDefaultType().c_str(), fem);
				assert(psc);
				if (psc)
				{
					prop.AddComponent(psc);
					bool b = InitDefaultProps(psc);
					if (b == false) { assert(false); return false; }
				}
				else return false;
			}
		}
	}
	return true;
}

bool BuildModelComponent(int superClassId, const std::string& typeStr, FSModelComponent* po)
{
	int classId = FEBio::GetClassId(superClassId, typeStr); assert(classId > 0);
	po->SetSuperClassID(superClassId);
	po->SetClassID(classId);
	po->SetTypeString(typeStr);
	bool ret = FEBio::BuildModelComponent(po);
	return ret;
}

bool FEBio::BuildModelComponent(FSModelComponent* pc, const std::string& typeStr)
{
	return BuildModelComponent(pc->GetSuperClassID(), typeStr, pc);
}

void FEBio::UpdateFEBioMaterial(FEBioMaterial* pm)
{
/*	FEBioClass* febClass = pm->GetFEBioMaterial();

	// first map the parameters to the FEBioClass
	map_parameters(febClass, pm);

	// then write the parameters to the FEBio class
	febClass->UpdateData();
*/
}

void FEBio::UpdateFEBioDiscreteMaterial(FEBioDiscreteMaterial* pm)
{
/*	FEBioClass* febClass = pm->GetFEBioMaterial();

	// first map the parameters to the FEBioClass
	map_parameters(febClass, pm);

	// then write the parameters to the FEBio class
	febClass->UpdateData();
*/
}

template <class T> T* CreateModelComponent(int superClassID, const std::string& typeStr, FSModel* fem)
{
	T* mc = new T(fem);
	if (BuildModelComponent(superClassID, typeStr, mc) == false)
	{
		assert(false);
		delete mc;
		return nullptr;
	}
	return mc;
}

template <class T> T* CreateModelComponent(int superClassID, const std::string& typeStr)
{
	T* mc = new T;
	if (BuildModelComponent(superClassID, typeStr, mc) == false)
	{
		assert(false);
		delete mc;
		return nullptr;
	}
	return mc;
}


FSStep* FEBio::CreateStep(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioAnalysisStep>(FEANALYSIS_ID, typeStr, fem);
}

FSMaterial* FEBio::CreateMaterial(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioMaterial>(FEMATERIAL_ID, typeStr);
}

FSMaterialProperty* FEBio::CreateMaterialProperty(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioMaterialProperty>(FEMATERIALPROP_ID, typeStr, fem);
}

FSDiscreteMaterial* FEBio::CreateDiscreteMaterial(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioDiscreteMaterial>(FEDISCRETEMATERIAL_ID, typeStr);
}

FSBoundaryCondition* FEBio::CreateBoundaryCondition(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioBoundaryCondition>(FEBC_ID, typeStr, fem);
}

FSNodalLoad* FEBio::CreateNodalLoad(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioNodalLoad>(FELOAD_ID, typeStr, fem);
}

FSSurfaceLoad* FEBio::CreateSurfaceLoad(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioSurfaceLoad>(FELOAD_ID, typeStr, fem);
}

FSBodyLoad* FEBio::CreateBodyLoad(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioBodyLoad>(FELOAD_ID, typeStr, fem);
}

FSPairedInterface* FEBio::CreatePairedInterface(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioInterface>(FESURFACEINTERFACE_ID, typeStr, fem);
}

FSModelConstraint* FEBio::CreateNLConstraint(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioNLConstraint>(FENLCONSTRAINT_ID, typeStr, fem);
}

FSSurfaceConstraint* FEBio::CreateSurfaceConstraint(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioSurfaceConstraint>(FENLCONSTRAINT_ID, typeStr, fem);
}

FSRigidConstraint* FEBio::CreateRigidConstraint(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioRigidConstraint>(FERIGIDBC_ID, typeStr, fem);
}

FSRigidConnector* FEBio::CreateRigidConnector(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioRigidConnector>(FENLCONSTRAINT_ID, typeStr, fem);
}

FSRigidLoad* FEBio::CreateRigidLoad(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioRigidLoad>(FELOAD_ID, typeStr, fem);
}

FSInitialCondition* FEBio::CreateInitialCondition(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioInitialCondition>(FEIC_ID, typeStr, fem);
}

FSLoadController* FEBio::CreateLoadController(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioLoadController>(FELOADCONTROLLER_ID, typeStr, fem);
}

FSFunction1D* FEBio::CreateFunction1D(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEBioFunction1D>(FEFUNCTION1D_ID, typeStr, fem);
}

FSGenericClass* FEBio::CreateGenericClass(const std::string& typeStr, FSModel* fem)
{
	if (typeStr.empty())
	{
		FSGenericClass* pc = new FSGenericClass;
		pc->SetSuperClassID(FECLASS_ID);
	}
	else return CreateModelComponent<FSGenericClass>(FECLASS_ID, typeStr);
}

FEShellFormulation* FEBio::CreateShellFormulation(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FEShellFormulation>(FESHELLDOMAIN_ID, typeStr, fem);
}

FESolidFormulation* FEBio::CreateSolidFormulation(const std::string& typeStr, FSModel* fem)
{
	return CreateModelComponent<FESolidFormulation>(FESOLIDDOMAIN_ID, typeStr, fem);
}

FSModelComponent* FEBio::CreateClass(int superClassID, const std::string& typeStr, FSModel* fem)
{
	switch (superClassID)
	{
	case FEMATERIAL_ID        : return CreateMaterial        (typeStr, fem); break;
	case FEMATERIALPROP_ID    : return CreateMaterialProperty(typeStr, fem); break;
	case FEDISCRETEMATERIAL_ID: return CreateDiscreteMaterial(typeStr, fem); break;
	case FECLASS_ID           : return CreateGenericClass    (typeStr, fem); break;
	case FELOADCONTROLLER_ID  : return CreateLoadController  (typeStr, fem); break;
	case FEFUNCTION1D_ID      : return CreateFunction1D      (typeStr, fem); break;
	case FESOLVER_ID          :
	case FENEWTONSTRATEGY_ID  :
	case FETIMECONTROLLER_ID  :
	case FEVEC3DGENERATOR_ID  :
	case FEMAT3DGENERATOR_ID  :
	case FEMAT3DSGENERATOR_ID :
	{
		FSGenericClass* pc = new FSGenericClass;
		BuildModelComponent(superClassID, typeStr, pc);
		return pc;
	}
	break;
	default:
		assert(false);
	}
	return nullptr;
}

FSModelComponent* FEBio::CreateClass(int classId, FSModel* fsm)
{
	FECoreKernel& fecore = FECoreKernel::GetInstance();

	const FECoreFactory* fac = fecore.GetFactoryClass(classId); assert(fac);
	int superClassID = fac->GetSuperClassID();

	int baseClassId = FEBio::GetBaseClassIndex(fac->GetBaseClassName());

	// create the FS model class
	FSModelComponent* pc = CreateFSClass(superClassID, baseClassId, fsm);
	pc->SetClassID(classId);
	pc->SetSuperClassID(superClassID);
	pc->SetTypeString(fac->GetTypeStr());

	// build the model component
	BuildModelComponent(pc);

	return pc;
}
