#include "FEAnalysisStep.h"
#include <MeshTools/FEModel.h>
#include <MeshTools/FERigidConstraint.h>
#include <MeshTools/FEProject.h>
#include <FEMLib/FEInitialCondition.h>
#include <FEMLib/FESurfaceLoad.h>
#include <FEMLib/FEMKernel.h>

int FEStep::m_ncount = 0;

//-----------------------------------------------------------------------------
// FEStep
//-----------------------------------------------------------------------------

FEStep::FEStep(FEModel* ps, int ntype) : m_ntype(ntype), m_pfem(ps)
{
	m_nID = ++m_ncount;
	m_sztype = "(undefined)";
}

//-----------------------------------------------------------------------------
FEStep::~FEStep()
{
	RemoveAllBCs();
	RemoveAllLoads();
	RemoveAllICs();
	RemoveAllInterfaces();
	RemoveAllConstraints();
	RemoveAllLinearConstraints();
	RemoveAllConnectors();
}

//-----------------------------------------------------------------------------
void FEStep::ResetCounter()
{
	m_ncount = 0;
}

//-----------------------------------------------------------------------------
void FEStep::DecreaseCounter()
{
	m_ncount--;
	assert(m_ncount >= 0);
}

//-----------------------------------------------------------------------------

void FEStep::SetID(int nid)
{
	if (nid > m_ncount) m_ncount = nid;
	m_nID = nid;
}

//-----------------------------------------------------------------------------
void FEStep::AddBC(FEBoundaryCondition* pbc)
{ 
	m_BC.push_back(pbc); 
	pbc->SetStep(GetID());
}

//-----------------------------------------------------------------------------
void FEStep::InsertBC(int n, FEBoundaryCondition* pbc)
{ 
	m_BC.insert(m_BC.begin() + n, pbc); 
	pbc->SetStep(GetID());
}

//-----------------------------------------------------------------------------
int FEStep::RemoveBC(FEBoundaryCondition* pbc)
{
	for (int i=0; i<(int) m_BC.size(); ++i) 
	{
		if (m_BC[i] == pbc)
		{
			m_BC.erase(m_BC.begin()+i);
			return i;
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------
void FEStep::RemoveAllBCs()
{
	for (int i=0; i<m_BC.size(); ++i) delete m_BC[i];
	m_BC.clear();
}

//-----------------------------------------------------------------------------
void FEStep::AddLoad(FEBoundaryCondition* pfc)
{ 
	m_FC.push_back(pfc); 
	pfc->SetStep(GetID()); 
}

//-----------------------------------------------------------------------------
void FEStep::InsertLoad(int n, FEBoundaryCondition* pfc)
{ 
	m_FC.insert(m_FC.begin() + n, pfc); 
	pfc->SetStep(GetID());
}

//-----------------------------------------------------------------------------
int FEStep::RemoveLoad(FEBoundaryCondition* pfc)
{
	for (int i=0; i<(int) m_FC.size(); ++i) 
	{
		if (m_FC[i] == pfc)
		{
			m_FC.erase(m_FC.begin()+i);
			return i;
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------
void FEStep::RemoveAllLoads()
{
	for (int i = 0; i<m_FC.size(); ++i) delete m_FC[i];
	m_FC.clear();
}

//-----------------------------------------------------------------------------
void FEStep::AddIC(FEInitialCondition* pic)
{
	m_IC.push_back(pic); 
	pic->SetStep(GetID());
}

void FEStep::InsertIC(int n, FEInitialCondition* pic)
{ 
	m_IC.insert(m_IC.begin() + n, pic); 
	pic->SetStep(GetID());
}

//-----------------------------------------------------------------------------
int FEStep::RemoveIC(FEInitialCondition* pic)
{
	for (int i = 0; i<(int)m_IC.size(); ++i)
	{
		if (m_IC[i] == pic)
		{
			m_IC.erase(m_IC.begin() + i);
			return i;
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------
void FEStep::RemoveAllICs()
{
	for (int i = 0; i<m_IC.size(); ++i) delete m_IC[i];
	m_IC.clear();
}

//-----------------------------------------------------------------------------
void FEStep::AddInterface(FEInterface* pi)
{ 
	m_Int.push_back(pi); 
	pi->SetStep(GetID());
}

//-----------------------------------------------------------------------------
void FEStep::InsertInterface(int n, FEInterface* pi)
{ 
	m_Int.insert(m_Int.begin() + n, pi); 
	pi->SetStep(GetID());
}

//-----------------------------------------------------------------------------
int FEStep::RemoveInterface(FEInterface* pi)
{
	for (int i=0; i<(int) m_Int.size(); ++i) 
	{
		if (m_Int[i] == pi)
		{
			m_Int.erase(m_Int.begin()+i);
			return i;
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------
void FEStep::RemoveAllInterfaces()
{
	for (int i = 0; i<m_Int.size(); ++i) delete m_Int[i];
	m_Int.clear();
}

//-----------------------------------------------------------------------------
int FEStep::RCs(int ntype)
{
	int nc = 0;
	for (int i = 0; i<(int)m_RC.size(); ++i)
	{
		if (m_RC[i]->Type() == ntype) nc++;
	}
	return nc;
}

//-----------------------------------------------------------------------------
void FEStep::AddRC(FERigidConstraint* prc)
{ 
	m_RC.push_back(prc); 
	prc->SetStep(GetID());
}

//-----------------------------------------------------------------------------
void FEStep::InsertRC(int n, FERigidConstraint* prc)
{ 
	m_RC.insert(m_RC.begin() + n, prc); 
	prc->SetStep(GetID());
}

//-----------------------------------------------------------------------------
int FEStep::RemoveRC(FERigidConstraint* prc)
{
	for (int i=0; i<(int) m_RC.size(); ++i) 
	{
		if (m_RC[i] == prc)
		{
			m_RC.erase(m_RC.begin()+i);
			return i;
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------
void FEStep::RemoveAllConstraints()
{
	for (int i = 0; i<m_RC.size(); ++i) delete m_RC[i];
	m_RC.clear();
}

//-----------------------------------------------------------------------------
void FEStep::RemoveAllLinearConstraints()
{
	for (int i = 0; i<m_LC.size(); ++i) delete m_LC[i];
	m_LC.clear();
}

//-----------------------------------------------------------------------------
void FEStep::AddConnector(FEConnector* pi)
{
	m_CN.push_back(pi); 
	pi->SetStep(GetID());
}

//-----------------------------------------------------------------------------
void FEStep::InsertConnector(int n, FEConnector* pi)
{ 
	m_CN.insert(m_CN.begin() + n, pi); 
	pi->SetStep(GetID());
}

//-----------------------------------------------------------------------------
int FEStep::RemoveConnector(FEConnector* pi)
{
    for (int i=0; i<(int) m_CN.size(); ++i)
    {
        if (m_CN[i] == pi)
        {
            m_CN.erase(m_CN.begin()+i);
            return i;
        }
    }
    return -1;
}

//-----------------------------------------------------------------------------
void FEStep::RemoveAllConnectors()
{
	for (int i = 0; i<m_CN.size(); ++i) delete m_CN[i];
	m_CN.clear();
}

//-----------------------------------------------------------------------------
void FEStep::Save(OArchive &ar)
{
	// write the name
	ar.WriteChunk(CID_STEP_NAME, GetName());

	// write the step
	ar.WriteChunk(CID_STEP_ID, m_nID);

	// save the boundary conditions
	int nbc = BCs();
	if (nbc > 0)
	{
		ar.BeginChunk(CID_BC_SECTION);
		{
			for (int i=0; i<nbc; ++i)
			{
				FEBoundaryCondition* pb = BC(i);
				int ntype = pb->Type();
				ar.BeginChunk(ntype);
				{
					pb->Save(ar);
				}
				ar.EndChunk();
			}
		}
		ar.EndChunk();
	}

	// save the loads
	int nfc = Loads();
	if (nfc > 0)
	{
		ar.BeginChunk(CID_FC_SECTION);
		{
			for (int i=0; i<nfc; ++i)
			{
				FEBoundaryCondition* pb = Load(i);
				int ntype = pb->Type();
				ar.BeginChunk(ntype);
				{
					pb->Save(ar);
				}
				ar.EndChunk();
			}
		}
		ar.EndChunk();
	}

	// save initial conditions
	int nic = ICs();
	if (nic > 0)
	{
		ar.BeginChunk(CID_IC_SECTION);
		{
			for (int i=0; i<nic; ++i)
			{
				FEInitialCondition* pi = IC(i);
				int ntype = pi->Type();
				ar.BeginChunk(ntype);
				{
					pi->Save(ar);
				}
				ar.EndChunk();
			}
		}
		ar.EndChunk();
	}

	// save the interfaces
	int nintfs = Interfaces();
	if (nintfs > 0)
	{
		ar.BeginChunk(CID_INTERFACE_SECTION);
		{
			for (int i=0; i<nintfs; ++i)
			{
				FEInterface* pi = Interface(i);
				int ntype = pi->Type();
				ar.BeginChunk(ntype);
				{
					pi->Save(ar);
				}
				ar.EndChunk();
			}
		}
		ar.EndChunk();
	}

	// save the rigid constriants
	int nrc = RCs();
	if (nrc > 0)
	{
		ar.BeginChunk(CID_RC_SECTION);
		{
			for (int i=0; i<nrc; ++i)
			{
				FERigidConstraint* pr = RC(i);
				int ntype = pr->Type();
				ar.BeginChunk(ntype);
				{
					pr->Save(ar);
				}
				ar.EndChunk();
			}
		}
		ar.EndChunk();
	}
    
    // save the connectors
    int ncnct = Connectors();
    if (ncnct > 0)
    {
        ar.BeginChunk(CID_CONNECTOR_SECTION);
        {
            for (int i=0; i<ncnct; ++i)
            {
                FEConnector* pi = Connector(i);
                int ntype = pi->Type();
                ar.BeginChunk(ntype);
                {
                    pi->Save(ar);
                }
                ar.EndChunk();
            }
        }
        ar.EndChunk();
    }
}

//-----------------------------------------------------------------------------
void FEStep::Load(IArchive &ar)
{
	TRACE("FEStep::Load");

	while (IO_OK == ar.OpenChunk())
	{
		int nid = ar.GetChunkID();
		switch (nid)
		{
		case CID_STEP_NAME      : { string name; ar.read(name); SetName(name); } break;
		case CID_STEP_ID        : { int nid; ar.read(nid); SetID(nid); } break;
		case CID_BC_SECTION: // boundary conditions
			{
				while (IO_OK == ar.OpenChunk())
				{
					int ntype = ar.GetChunkID();

					FEBoundaryCondition* pb = 0;
					switch (ntype)
					{
					case FE_FIXED_DISPLACEMENT		 : pb = new FEFixedDisplacement         (m_pfem); break;
					case FE_FIXED_ROTATION           : pb = new FEFixedRotation             (m_pfem); break;
					case FE_FIXED_FLUID_PRESSURE	 : pb = new FEFixedFluidPressure        (m_pfem); break;
					case FE_FIXED_TEMPERATURE        : pb = new FEFixedTemperature          (m_pfem); break;
					case FE_FIXED_CONCENTRATION      : pb = new FEFixedConcentration        (m_pfem); break;
                    case FE_FIXED_FLUID_VELOCITY     : pb = new FEFixedFluidVelocity        (m_pfem); break;
                    case FE_FIXED_DILATATION         : pb = new FEFixedFluidDilatation      (m_pfem); break;
					case FE_PRESCRIBED_DISPLACEMENT	 : pb = new FEPrescribedDisplacement    (m_pfem); break;
					case FE_PRESCRIBED_ROTATION      : pb = new FEPrescribedRotation        (m_pfem); break;
					case FE_PRESCRIBED_FLUID_PRESSURE: pb = new FEPrescribedFluidPressure   (m_pfem); break;
					case FE_PRESCRIBED_TEMPERATURE   : pb = new FEPrescribedTemperature     (m_pfem); break;
					case FE_PRESCRIBED_CONCENTRATION : pb = new FEPrescribedConcentration   (m_pfem); break;
                    case FE_PRESCRIBED_FLUID_VELOCITY: pb = new FEPrescribedFluidVelocity   (m_pfem); break;
                    case FE_PRESCRIBED_DILATATION    : pb = new FEPrescribedFluidDilatation (m_pfem); break;
					case FE_FIXED_SHELL_DISPLACEMENT : pb = new FEFixedShellDisplacement    (m_pfem); break;
					default:
						if (ar.Version() < 0x00020000)
						{
							// initial conditions used to be grouped with the boundary conditions, but
							// now have their own section
							switch(ntype)
							{
							case FE_NODAL_VELOCITIES		 : pb = new FENodalVelocities       (m_pfem); break;
							case FE_NODAL_SHELL_VELOCITIES	 : pb = new FENodalShellVelocities  (m_pfem); break;
                            case FE_INIT_FLUID_PRESSURE      : pb = new FEInitFluidPressure     (m_pfem); break;
                            case FE_INIT_SHELL_FLUID_PRESSURE: pb = new FEInitShellFluidPressure(m_pfem); break;
							case FE_INIT_CONCENTRATION       : pb = new FEInitConcentration     (m_pfem); break;
                            case FE_INIT_SHELL_CONCENTRATION : pb = new FEInitShellConcentration(m_pfem); break;
							case FE_INIT_TEMPERATURE         : pb = new FEInitTemperature       (m_pfem); break;
							default:
								throw ReadError("error parsing CID_BC_SECTION in FEStep::Load");
							}
						}
						else
						{
							throw ReadError("error parsing CID_BC_SECTION in FEStep::Load");
						}
					}

					pb->Load(ar);

					if (ar.Version() < 0x0001000F)
					{
						// In older version, the rotational dofs were actually stored in the FEFixedDisplacement
						// but now, they are stored in FEFixedRotation
						FEFixedDisplacement* pbc = dynamic_cast<FEFixedDisplacement*>(pb);
						if (pbc)
						{
							int bc = pbc->GetBC();
							if (bc >= 8)
							{
								bc = (bc>>3)&7;
								FEFixedRotation* prc = new FEFixedRotation(m_pfem);
								prc->SetName(pbc->GetName());
								prc->SetBC(bc);
								prc->SetItemList(pb->GetItemList()->Copy());
								AddBC(prc);
							}
						}

						FEPrescribedDisplacement* pdc = dynamic_cast<FEPrescribedDisplacement*>(pb);
						if (pdc)
						{
							int bc = pdc->GetDOF();
							if (bc > 2)
							{
								bc -= 3;
								FEPrescribedRotation* prc = new FEPrescribedRotation(m_pfem);
								prc->SetName(pdc->GetName());
								prc->SetDOF(bc);
								prc->SetItemList(pdc->GetItemList()->Copy());
								delete pdc;
								pb = prc;
							}
						}
					}

					// add BC
					if (ar.Version() < 0x00020000)
					{
						if (dynamic_cast<FEInitialCondition*>(pb)) AddIC(dynamic_cast<FEInitialCondition*>(pb));
						else AddBC(pb);
					}
					else AddBC(pb);

					ar.CloseChunk();
				}
			}
			break;
		case CID_FC_SECTION: // loads
			{
				while (IO_OK == ar.OpenChunk())
				{
					int ntype = ar.GetChunkID();

					FEBoundaryCondition* pl = 0;

					if (ntype == FE_NODAL_LOAD) pl = new FENodalLoad(m_pfem);
					else
					{
						// see if it's a surface load
						pl = fecore_new<FEBoundaryCondition>(m_pfem, FE_SURFACE_LOAD, ntype);
						if (pl == 0)
						{
							// could be a body load
							pl = fecore_new<FEBoundaryCondition>(m_pfem, FE_BODY_LOAD, ntype);
						}
					}

					// make sure we have something
					if (pl == 0)
					{
						throw ReadError("error parsing CID_FC_SECTION FEStep::Load");
					}

					// read BC data
					pl->Load(ar);

					// add BC
					AddLoad(pl);

					ar.CloseChunk();
				}
			}
			break;
		case CID_IC_SECTION: // initial conditions
			{
				while (IO_OK == ar.OpenChunk())
				{
					int ntype = ar.GetChunkID();

					FEInitialCondition* pi = 0;
					switch (ntype)
					{
					case FE_INIT_FLUID_PRESSURE      : pi = new FEInitFluidPressure     (m_pfem); break;
                    case FE_INIT_SHELL_FLUID_PRESSURE: pi = new FEInitShellFluidPressure(m_pfem); break;
                    case FE_INIT_CONCENTRATION       : pi = new FEInitConcentration     (m_pfem); break;
                    case FE_INIT_SHELL_CONCENTRATION : pi = new FEInitShellConcentration(m_pfem); break;
					case FE_INIT_TEMPERATURE         : pi = new FEInitTemperature       (m_pfem); break;
					case FE_NODAL_VELOCITIES         : pi = new FENodalVelocities       (m_pfem); break;
					case FE_NODAL_SHELL_VELOCITIES   : pi = new FENodalShellVelocities  (m_pfem); break;
					default:
						throw ReadError("error parsing CID_IC_SECTION FEStep::Load");
					}

					// read data
					pi->Load(ar);

					// Add IC
					AddIC(pi);

					ar.CloseChunk();
				}
			}
			break;
		case CID_INTERFACE_SECTION: // interfaces
			{
				while (IO_OK == ar.OpenChunk())
				{
					int ntype = ar.GetChunkID();

					FEInterface* pi = 0;

					// check obsolete interfaces first
					if      (ntype == FE_SLIDING_INTERFACE   ) pi = new FESlidingInterface(m_pfem);
					else if (ntype == FE_SPRINGTIED_INTERFACE) pi = new FESpringTiedInterface(m_pfem);
					else pi = fecore_new<FEInterface>(m_pfem, FE_INTERFACE, ntype);

					// make sure we were able to allocate an interface
					if (pi == 0)
					{
						throw ReadError("error parsing unknown CID_INTERFACE_SECTION FEStep::Load");
					}

					// load the interface data
					pi->Load(ar);

					// add interface to step
					AddInterface(pi);

					ar.CloseChunk();
				}
			}
			break;
		case CID_RC_SECTION: // rigid constraints
			{
				while (IO_OK == ar.OpenChunk())
				{
					int ntype = ar.GetChunkID();

					if (ar.Version() < 0x00020000)
					{
						FERigidConstraintOld* rc_old = new FERigidConstraintOld(ntype, GetID());

						// read RC data
						rc_old->Load(ar);

						vector<FERigidConstraint*> rc = convertOldToNewRigidConstraint(m_pfem, rc_old);

						// add RCs
						for (int i=0; i<(int) rc.size(); ++i) AddRC(rc[i]);
					}
					else
					{
						FERigidConstraint* rc = 0;
						switch (ntype)
						{
						case FE_RIGID_FIXED            : rc = new FERigidFixed          (m_pfem, GetID()); break;
						case FE_RIGID_DISPLACEMENT     : rc = new FERigidDisplacement   (m_pfem, GetID()); break;
						case FE_RIGID_FORCE            : rc = new FERigidForce          (m_pfem, GetID()); break;
						case FE_RIGID_INIT_VELOCITY    : rc = new FERigidVelocity       (m_pfem, GetID()); break;
						case FE_RIGID_INIT_ANG_VELOCITY: rc = new FERigidAngularVelocity(m_pfem, GetID()); break;
						default:
							assert(false);
						}

						if (rc)
						{
							rc->Load(ar);
							AddRC(rc);
						}
					}

					ar.CloseChunk();
				}
			}
			break;
        case CID_CONNECTOR_SECTION: // connectors
            {
                while (IO_OK == ar.OpenChunk())
                {
                    int ntype = ar.GetChunkID();
                    
                    FEConnector* pi = 0;
                    switch (ntype)
                    {
                        case FE_RC_SPHERICAL_JOINT		: pi = new FERigidSphericalJoint    (m_pfem); break;
                        case FE_RC_REVOLUTE_JOINT		: pi = new FERigidRevoluteJoint     (m_pfem); break;
                        case FE_RC_PRISMATIC_JOINT		: pi = new FERigidPrismaticJoint    (m_pfem); break;
                        case FE_RC_CYLINDRICAL_JOINT	: pi = new FERigidCylindricalJoint  (m_pfem); break;
                        case FE_RC_PLANAR_JOINT         : pi = new FERigidPlanarJoint       (m_pfem); break;
                        case FE_RC_RIGID_LOCK           : pi = new FERigidLock              (m_pfem); break;
                        case FE_RC_SPRING               : pi = new FERigidSpring            (m_pfem); break;
                        case FE_RC_DAMPER               : pi = new FERigidDamper            (m_pfem); break;
                        case FE_RC_ANGULAR_DAMPER       : pi = new FERigidAngularDamper     (m_pfem); break;
                        case FE_RC_CONTRACTILE_FORCE    : pi = new FERigidContractileForce  (m_pfem); break;
                        default:
                            throw ReadError("error parsing unknown CID_CONNECTOR_SECTION FEStep::Load");
                    }
                    
                    // load the interface data
                    pi->Load(ar);
                    
                    // add interface to step
                    AddConnector(pi);
                    
                    ar.CloseChunk();
                }
            }
            break;
		}
		ar.CloseChunk();
	}
}

//-----------------------------------------------------------------------------
// FEInitialStep
//-----------------------------------------------------------------------------

FEInitialStep::FEInitialStep(FEModel* ps) : FEStep(ps, FE_STEP_INITIAL)
{ 
	SetName("Initial"); 
	SetTypeString("Initial");

	SetID(0);
	DecreaseCounter();
}

void FEInitialStep::Save(OArchive &ar)
{
	// save the step data
	ar.BeginChunk(CID_STEP_DATA);
	{
		FEStep::Save(ar);
	}
	ar.EndChunk();
}

void FEInitialStep::Load(IArchive &ar)
{
	TRACE("FEInitialStep::Load");

	while (IO_OK == ar.OpenChunk())
	{
		int nid = ar.GetChunkID();
		switch (nid)
		{
		case CID_STEP_DATA      : FEStep::Load(ar); break;
		}
		ar.CloseChunk();
	}	
}

//-----------------------------------------------------------------------------
// FEAnalysisStep
//-----------------------------------------------------------------------------

void STEP_SETTINGS::Defaults()
{
	sztitle[0] = 0;

	// time settings
	ntime = 10;
	dt = 0.1;
	bauto = true;
	bmust = false;
	mxback = 5;
	iteopt = 10;
	dtmin = 0.01;
	dtmax = 0.1;
	ncut = 0;	// constant cutback
	tfinal = 0;
	bdivref = true;
	brefstep = true;

	// solver settings
	mthsol = 0;
	bminbw = false;
	nmatfmt = 0;	// = default matrix storage
	ilimit = 10;
	maxref = 15;

	// analysis settings
	nanalysis = 0;

	// time integration parameters
	override_rhoi = false;		// use rhoi instead of these parameters
    alpha = 1;
    beta = 0.25;
    gamma = 0.50;

	// output optios
	plot_stride = 1;

	// constants
//	Rc = 0; //8.314e-6;
//	Ta = 0; //298;
}

FEAnalysisStep::FEAnalysisStep(FEModel* ps, int ntype) : FEStep(ps, ntype)
{
	// set default options
	m_ops.Defaults();

	// reset must point curve
	LOADPOINT pt0(0,0), pt1(1,1);
	m_MP.Clear();
	m_MP.SetType(FELoadCurve::LC_STEP);
	m_MP.Add(pt0);
	m_MP.Add(pt1);
}

vector<string> FEAnalysisStep::GetAnalysisStrings() const
{
	vector<string> s;
	s.push_back("static");
	s.push_back("dynamic");
	return s;
}

void FEAnalysisStep::Save(OArchive &ar)
{
	// save the step settings
	ar.BeginChunk(CID_STEP_SETTINGS);
	{
		STEP_SETTINGS& o = m_ops;
		ar.WriteChunk(CID_STEP_TITLE     , o.sztitle);
		ar.WriteChunk(CID_STEP_NTIME     , o.ntime);
		ar.WriteChunk(CID_STEP_DT        , o.dt);
		ar.WriteChunk(CID_STEP_BAUTO     , o.bauto);
		ar.WriteChunk(CID_STEP_BMUST     , o.bmust);
		ar.WriteChunk(CID_STEP_MXBACK    , o.mxback);
		ar.WriteChunk(CID_STEP_ITEOPT    , o.iteopt);
		ar.WriteChunk(CID_STEP_DTMIN     , o.dtmin);
		ar.WriteChunk(CID_STEP_DTMAX     , o.dtmax);
		ar.WriteChunk(CID_STEP_MTHSOL    , o.mthsol);
		ar.WriteChunk(CID_STEP_BMINBW    , o.bminbw);
		ar.WriteChunk(CID_STEP_MATFMT	 , o.nmatfmt);
		ar.WriteChunk(CID_STEP_ILIMIT    , o.ilimit);
		ar.WriteChunk(CID_STEP_MAXREF    , o.maxref);
		ar.WriteChunk(CID_STEP_DIVREF	 , o.bdivref);
		ar.WriteChunk(CID_STEP_REFSTEP   , o.brefstep);
//		ar.WriteChunk(CID_STEP_DTOL      , o.dtol);
//		ar.WriteChunk(CID_STEP_ETOL      , o.etol);
//		ar.WriteChunk(CID_STEP_RTOL      , o.rtol);
//		ar.WriteChunk(CID_STEP_PTOL		 , o.ptol);
//		ar.WriteChunk(CID_STEP_CTOL		 , o.ctol);
//		ar.WriteChunk(CID_STEP_VTOL      , o.vtol);
//		ar.WriteChunk(CID_STEP_TOLLS     , o.tolls);
		ar.WriteChunk(CID_STEP_ANALYSIS  , o.nanalysis);
//		ar.WriteChunk(CID_STEP_PRES_STIFF, o.nprstf);
//		ar.WriteChunk(CID_STEP_SYMM_PORO , o.bsymmporo);
		ar.WriteChunk(CID_STEP_CUTBACK   , o.ncut);
//		ar.WriteChunk(CID_STEP_MINRES    , o.minres);
		ar.WriteChunk(CID_STEP_PLOTSTRIDE, o.plot_stride);
	}
	ar.EndChunk();

	// save the step parameters
	ar.BeginChunk(CID_STEP_PARAMS);
	{
		ParamContainer::Save(ar);
	}
	ar.EndChunk();

	// save the must point
	ar.BeginChunk(CID_STEP_MUST_POINT);
	{
		m_MP.Save(ar);
	}
	ar.EndChunk();

	// save the step data
	ar.BeginChunk(CID_STEP_DATA);
	{
		FEStep::Save(ar);
	}
	ar.EndChunk();
}

void FEAnalysisStep::Load(IArchive &ar)
{
	TRACE("FEAnalysisStep::Load");

	while (IO_OK == ar.OpenChunk())
	{
		int nid = ar.GetChunkID();
		switch (nid)
		{
		case CID_STEP_PARAMS:
		{
			ParamContainer::Load(ar);
		}
		break;
		case CID_STEP_SETTINGS:
			{
				STEP_SETTINGS& o = m_ops;
				while (IO_OK == ar.OpenChunk())
				{
					int nid = ar.GetChunkID();
					switch (nid)
					{
					case CID_STEP_TITLE     : ar.read(o.sztitle  ); break;
					case CID_STEP_NTIME     : ar.read(o.ntime    ); break;
					case CID_STEP_DT        : ar.read(o.dt       ); break;
					case CID_STEP_BAUTO     : ar.read(o.bauto    ); break;
					case CID_STEP_BMUST     : ar.read(o.bmust    ); break;
					case CID_STEP_MXBACK    : ar.read(o.mxback   ); break;
					case CID_STEP_ITEOPT    : ar.read(o.iteopt   ); break;
					case CID_STEP_DTMIN     : ar.read(o.dtmin    ); break;
					case CID_STEP_DTMAX     : ar.read(o.dtmax    ); break;
					case CID_STEP_MTHSOL    : ar.read(o.mthsol   ); break;
					case CID_STEP_BMINBW    : ar.read(o.bminbw   ); break;
					case CID_STEP_MATFMT    : ar.read(o.nmatfmt  ); break;
					case CID_STEP_ILIMIT    : ar.read(o.ilimit   ); break;
					case CID_STEP_MAXREF    : ar.read(o.maxref   ); break;
					case CID_STEP_DIVREF	: ar.read(o.bdivref  ); break;
					case CID_STEP_REFSTEP	: ar.read(o.brefstep ); break;
//					case CID_STEP_DTOL      : ar.read(o.dtol     ); break;
//					case CID_STEP_ETOL      : ar.read(o.etol     ); break;
//					case CID_STEP_RTOL      : ar.read(o.rtol     ); break;
//					case CID_STEP_PTOL      : ar.read(o.ptol     ); break;
//					case CID_STEP_CTOL      : ar.read(o.ctol     ); break;
//					case CID_STEP_VTOL      : ar.read(o.vtol     ); break;
//					case CID_STEP_TOLLS     : ar.read(o.tolls    ); break;
					case CID_STEP_ANALYSIS  : ar.read(o.nanalysis); break;
//					case CID_STEP_PSTIFFNESS: ar.read(o.bpress_stiff); break;
//					case CID_STEP_PRES_STIFF: ar.read(o.nprstf   ); break;
//					case CID_STEP_SYMM_PORO : ar.read(o.bsymmporo); break;
					case CID_STEP_CUTBACK   : ar.read(o.ncut     ); break;
//					case CID_STEP_MINRES    : ar.read(o.minres   ); break;
					case CID_STEP_PLOTSTRIDE: ar.read(o.plot_stride); break;

					}
					ar.CloseChunk();

					// for old versions, biphasic and biphasic-solute problems
					// only knew transient analyses, but the analysis parameter was not 
					// used, so it may contain an incorrect value now. Therefore
					// we need to fix it.
					if (ar.Version() < 0x00010008)
					{
						if ((GetType() == FE_STEP_BIPHASIC) || 
							(GetType() == FE_STEP_BIPHASIC_SOLUTE) ||
							(GetType() == FE_STEP_MULTIPHASIC)) o.nanalysis = FE_DYNAMIC;
					}
				}
			}
			break;
		case CID_STEP_MUST_POINT: m_MP.Load(ar); break;
		case CID_STEP_DATA      : FEStep::Load(ar); break;
		}
		ar.CloseChunk();
	}	
}

//-----------------------------------------------------------------------------
FENonLinearMechanics::FENonLinearMechanics(FEModel* ps) : FEAnalysisStep(ps, FE_STEP_MECHANICS)
{
	SetTypeString("Structural Mechanics");

	AddDoubleParam(0.001, "dtol", "Displacement tolerance");
	AddDoubleParam(0.01 , "etol", "Energy tolerance");
	AddDoubleParam(0    , "rtol", "Residual tolerance");
	AddDoubleParam(0.9  , "lstol", "Line search tolerance");
	AddDoubleParam(1e-20, "min_residual", "Minumum residual");
	AddChoiceParam(0, "qnmethod", "Quasi-Newton method")->SetEnumNames("BFGS\0Broyden\0");
	AddDoubleParam(0, "rhoi", "Spectral radius");
}

double FENonLinearMechanics::GetDisplacementTolerance() { return GetParam(MP_DTOL).GetFloatValue(); }
double FENonLinearMechanics::GetEnergyTolerance() { return GetParam(MP_ETOL).GetFloatValue(); }
double FENonLinearMechanics::GetResidualTolerance() { return GetParam(MP_RTOL).GetFloatValue(); }
double FENonLinearMechanics::GetLineSearchTolerance() { return GetParam(MP_LSTOL).GetFloatValue(); }

void FENonLinearMechanics::SetDisplacementTolerance(double dtol) { GetParam(MP_DTOL).SetFloatValue(dtol); }
void FENonLinearMechanics::SetEnergyTolerance(double etol) { GetParam(MP_ETOL).SetFloatValue(etol); }
void FENonLinearMechanics::SetResidualTolerance(double rtol) { GetParam(MP_RTOL).SetFloatValue(rtol); }
void FENonLinearMechanics::SetLineSearchTolerance(double lstol) { GetParam(MP_LSTOL).SetFloatValue(lstol); }

//-----------------------------------------------------------------------------
FEHeatTransfer::FEHeatTransfer(FEModel* ps) : FEAnalysisStep(ps, FE_STEP_HEAT_TRANSFER)
{
	SetTypeString("Heat Transfer");
}

vector<string> FEHeatTransfer::GetAnalysisStrings() const
{
	vector<string> s;
	s.push_back("steady-state");
	s.push_back("transient");
	return s;
}

//-----------------------------------------------------------------------------
FENonLinearBiphasic::FENonLinearBiphasic(FEModel* ps) : FEAnalysisStep(ps, FE_STEP_BIPHASIC)
{
	SetTypeString("Biphasic");

	AddDoubleParam(0.001, "dtol", "Displacement tolerance");
	AddDoubleParam(0.01 , "etol", "Energy tolerance");
	AddDoubleParam(0    , "rtol", "Residual tolerance");
	AddDoubleParam(0.01 , "ptol", "Pressure tolerance");
	AddDoubleParam(0.9  , "lstol", "Line search tolerance");
	AddDoubleParam(1e-20, "min_residual", "Minumum residual");
	AddChoiceParam(0, "qnmethod", "Quasi-Newton method")->SetEnumNames("BFGS\0Broyden\0");

	m_ops.nanalysis = 1; // set transient analysis
}

vector<string> FENonLinearBiphasic::GetAnalysisStrings() const
{
	vector<string> s;
	s.push_back("steady-state");
	s.push_back("transient");
	return s;
}

//-----------------------------------------------------------------------------
FEBiphasicSolutes::FEBiphasicSolutes(FEModel* ps) : FEAnalysisStep(ps, FE_STEP_BIPHASIC_SOLUTE)
{
	SetTypeString("Biphasic-solute");

	AddDoubleParam(0.001, "dtol", "Displacement tolerance");
	AddDoubleParam(0.01 , "etol", "Energy tolerance");
	AddDoubleParam(0    , "rtol", "Residual tolerance");
	AddDoubleParam(0.01 , "ptol", "Pressure tolerance");
	AddDoubleParam(0.01 , "ctol", "Concentration tolerance");
	AddDoubleParam(0.9  , "lstol", "Line search tolerance");
	AddDoubleParam(1e-20, "min_residual", "Minumum residual");
	AddChoiceParam(0, "qnmethod", "Quasi-Newton method")->SetEnumNames("BFGS\0Broyden\0");

	m_ops.nanalysis = 1; // set transient analysis
}

vector<string> FEBiphasicSolutes::GetAnalysisStrings() const
{
	vector<string> s;
	s.push_back("steady-state");
	s.push_back("transient");
	return s;
}

//-----------------------------------------------------------------------------
FEMultiphasicAnalysis::FEMultiphasicAnalysis(FEModel* ps) : FEAnalysisStep(ps, FE_STEP_MULTIPHASIC)
{
	SetTypeString("Multiphasic");

	AddDoubleParam(0.001, "dtol", "Displacement tolerance");
	AddDoubleParam(0.01 , "etol", "Energy tolerance");
	AddDoubleParam(0    , "rtol", "Residual tolerance");
	AddDoubleParam(0.01 , "ptol", "Pressure tolerance");
	AddDoubleParam(0.01 , "ctol", "Concentration tolerance");
	AddDoubleParam(0.9  , "lstol", "Line search tolerance");
	AddDoubleParam(1e-20, "min_residual", "Minumum residual");
	AddChoiceParam(0, "qnmethod", "Quasi-Newton method")->SetEnumNames("BFGS\0Broyden\0");

	m_ops.nanalysis = 1; // set transient analysis
    m_ops.nmatfmt = 0;   // set non-symmetric flag
}

vector<string> FEMultiphasicAnalysis::GetAnalysisStrings() const
{
	vector<string> s;
	s.push_back("steady-state");
	s.push_back("transient");
	return s;
}

//-----------------------------------------------------------------------------
FEFluidAnalysis::FEFluidAnalysis(FEModel* ps) : FEAnalysisStep(ps, FE_STEP_FLUID)
{
    SetTypeString("Fluid");

	AddDoubleParam(0.001, "vtol", "Fluid velocity tolerance");
    AddDoubleParam(0.001, "ftol", "Fluid dilatation tolerance");
	AddDoubleParam(0.01 , "etol", "Energy tolerance");
	AddDoubleParam(0    , "rtol", "Residual tolerance");
	AddDoubleParam(0.9  , "lstol", "Line search tolerance");
	AddDoubleParam(1e-20, "min_residual", "Minumum residual");
    AddDoubleParam(0    , "rhoi", "Spectral radius");
	AddChoiceParam(1    , "qnmethod", "Quasi-Newton method")->SetEnumNames("BFGS\0Broyden\0");
	AddChoiceParam(1    , "equation_scheme", "Equation Scheme");

    m_ops.nanalysis = 1; // set dynamic analysis
    m_ops.nmatfmt = 0;   // set non-symmetric flag
}

//-----------------------------------------------------------------------------
FEFluidFSIAnalysis::FEFluidFSIAnalysis(FEModel* ps) : FEAnalysisStep(ps, FE_STEP_FLUID_FSI)
{
    SetTypeString("Fluid-FSI");
    
    AddDoubleParam(0.001, "dtol", "Solid displacement tolerance");
    AddDoubleParam(0.001, "vtol", "Fluid velocity tolerance");
    AddDoubleParam(0.001, "ftol", "Fluid dilatation tolerance");
    AddDoubleParam(0.01 , "etol", "Energy tolerance");
    AddDoubleParam(0    , "rtol", "Residual tolerance");
    AddDoubleParam(0.9  , "lstol", "Line search tolerance");
	AddDoubleParam(1e-20, "min_residual", "Minumum residual");
    AddDoubleParam(0    , "rhoi", "Spectral radius");
    AddChoiceParam(1    , "qnmethod", "Quasi-Newton method")->SetEnumNames("BFGS\0Broyden\0");
    
    m_ops.nanalysis = 1; // set dynamic analysis
    m_ops.nmatfmt = 0;   // set non-symmetric flag
}

//-----------------------------------------------------------------------------
FEReactionDiffusionAnalysis::FEReactionDiffusionAnalysis(FEModel* ps) : FEAnalysisStep(ps, FE_STEP_REACTION_DIFFUSION)
{
	SetTypeString("Reaction diffusion");

	AddDoubleParam(0.001, "Ctol", "Concentration tolerance");
	AddDoubleParam(0.01 , "Rtol", "Residual tolerance");
	AddDoubleParam(0.0  , "Stol", "SBS concentration tolerance");
	AddDoubleParam(0.5  , "alpha", "Time integration parameter");
	AddDoubleParam(1e-20, "min_residual", "Minumum residual");

	m_ops.nanalysis = 1; // set dynamic analysis
	m_ops.nmatfmt = 0;   // set non-symmetric flag
}


vector<string> FEReactionDiffusionAnalysis::GetAnalysisStrings() const
{
	vector<string> s;
	s.push_back("steady-state");
	s.push_back("transient");
	return s;
}
