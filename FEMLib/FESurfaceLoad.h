#pragma once
#include "FEBoundaryCondition.h"

//=============================================================================
// Base class for prescribed pressure loads.
//
class FESurfaceLoad : public FEPrescribedBC
{
public:
	FESurfaceLoad(int ntype, FEModel* ps) : FEPrescribedBC(ntype, ps) {}
	FESurfaceLoad(int ntype, FEModel* ps, FEItemListBuilder* pi, int nstep) : FEPrescribedBC(ntype, ps, pi, nstep){}
};

//-----------------------------------------------------------------------------
// Surface pressure boundary conditions
//
class FEPressureLoad : public FESurfaceLoad
{
public:
	enum { LOAD, NTYPE };

public:
	FEPressureLoad(FEModel* ps, FEItemListBuilder* pi = 0, int nstep = 0);

	FELoadCurve* GetLoadCurve() { return GetParamLC(LOAD); }

	void SetLinearFlag(bool b) { SetBoolValue(NTYPE, b); }
	bool GetLinearFlag() { return GetBoolValue(NTYPE); }

	void SetLoad(double f) { SetFloatValue(LOAD, f); }
	double GetLoad() { return GetFloatValue(LOAD); }

	// used only for reading parameters for old file formats
	void LoadParam(const Param& p);
};

//-----------------------------------------------------------------------------
// Fluid flux surface boundary load
//
class FEFluidFlux : public FESurfaceLoad
{
public:
	enum { LOAD, NTYPE, NFLUX };

public:
	FEFluidFlux(FEModel* ps, FEItemListBuilder* pi = 0, int nstep = 0);

	FELoadCurve* GetLoadCurve() { return GetParamLC(LOAD); }

	void SetLoad(double f) { SetFloatValue(LOAD, f); }
	double GetLoad() { return GetFloatValue(LOAD); }

	void SetLinearFlag(bool b) { SetBoolValue(NTYPE, b); }
	bool GetLinearFlag() { return GetBoolValue(NTYPE); }

	void SetMixtureFlag(bool b) { SetBoolValue(NFLUX, b); }
	bool GetMixtureFlag() { return GetBoolValue(NFLUX); }

	// used only for reading parameters for old file formats
	void LoadParam(const Param& p);
};


//-----------------------------------------------------------------------------
// Mixture Normal Traction surface boundary load
//
class FEBPNormalTraction : public FESurfaceLoad
{
public:
	enum { LOAD, NTYPE, NTRAC };

public:
	FEBPNormalTraction(FEModel* ps, FEItemListBuilder* pi = 0, int nstep = 0);

	FELoadCurve* GetLoadCurve() { return GetParamLC(LOAD); }

	void SetLoad(double f) { SetFloatValue(LOAD, f); }
	double GetLoad() { return GetFloatValue(LOAD); }

	void SetLinearFlag(bool b) { SetBoolValue(NTYPE, b); }
	bool GetLinearFlag() { return GetBoolValue(NTYPE); }

	void SetMixtureFlag(bool b) { SetBoolValue(NTRAC, b); }
	bool GetMixtureFlag() { return GetBoolValue(NTRAC); }

	// used only for reading parameters for old file formats
	void LoadParam(const Param& p);
};

//-----------------------------------------------------------------------------
// Solute flux surface boundary load
//
class FESoluteFlux : public FESurfaceLoad
{
public:
	enum { LOAD, NTYPE, BC };

public:
	FESoluteFlux(FEModel* ps, FEItemListBuilder* pi = 0, int nstep = 0);

	FELoadCurve* GetLoadCurve() { return GetParamLC(LOAD); }

	void SetLoad(double f) { SetFloatValue(LOAD, f); }
	double GetLoad() { return GetFloatValue(LOAD); }

	void SetLinearFlag(bool b) { SetBoolValue(NTYPE, b); }
	bool GetLinearFlag() { return GetBoolValue(NTYPE); }

	int GetBC() { return GetIntValue(BC); }
	void SetBC(int n) { SetIntValue(BC, n); }

	// used only for reading parameters for old file formats
	void LoadParam(const Param& p);
};

//-----------------------------------------------------------------------------
// Heat flux surface boundary load
//
class FEHeatFlux : public FESurfaceLoad
{
public:
	enum { FLUX };

public:
	FEHeatFlux(FEModel* ps, FEItemListBuilder* pi = 0, int nstep = 0);

	FELoadCurve* GetLoadCurve() { return GetParamLC(FLUX); }

	void SetLoad(double f) { SetFloatValue(FLUX, f); }
	double GetLoad() { return GetFloatValue(FLUX); }

	// used only for reading parameters for old file formats
	void LoadParam(const Param& p);
};

//-----------------------------------------------------------------------------
// Convective Heat flux surface boundary load
//
class FEConvectiveHeatFlux : public FESurfaceLoad
{
public:
	enum { HC, TREF };

public:
	FEConvectiveHeatFlux(FEModel* ps, FEItemListBuilder* pi = 0, int nstep = 0);

	double GetCoefficient() { return GetFloatValue(HC); }
	double GetTemperature() { return GetFloatValue(TREF); }

	void SetCoefficient(double hc) { SetFloatValue(HC, hc); }
	void SetTemperature(double Ta) { SetFloatValue(TREF, Ta); }

	FELoadCurve* GetLoadCurve() { return GetParamLC(TREF); }

	// used only for reading parameters for old file formats
	void LoadParam(const Param& p);
};

//-----------------------------------------------------------------------------

class FESurfaceTraction : public FESurfaceLoad
{
public:
	enum { LOAD, TRACTION };

public:
	FESurfaceTraction(FEModel* ps, FEItemListBuilder* pi = 0, int nstep = 0);

	FELoadCurve* GetLoadCurve() { return GetParamLC(LOAD); }

	void SetScale(double f) { SetFloatValue(LOAD, f); }
	double GetScale() { return GetFloatValue(LOAD); }

	void SetTraction(const vec3d& t) { SetVecValue(TRACTION, t); }
	vec3d GetTraction() { return GetVecValue(TRACTION); }

	// used only for reading parameters for old file formats
	void LoadParam(const Param& p);
};

//-----------------------------------------------------------------------------

class FEFluidTraction : public FESurfaceLoad
{
public:
	enum { LOAD, TRACTION };

public:
	FEFluidTraction(FEModel* ps, FEItemListBuilder* pi = 0, int nstep = 0);

	FELoadCurve* GetLoadCurve();

	void SetScale(double s);
	double GetScale();

	void SetTraction(const vec3d& t);
	vec3d GetTraction();

	// used only for reading parameters for old file formats
	void LoadParam(const Param& p);
};

//-----------------------------------------------------------------------------

class FEFluidVelocity : public FESurfaceLoad
{
public:
    enum { LOAD };
    
public:
    FEFluidVelocity(FEModel* ps);
    FEFluidVelocity(FEModel* ps, FEItemListBuilder* pi, vec3d t, int nstep = 0);
    
    FELoadCurve* GetLoadCurve() { return GetParamLC(LOAD); }
    
    void SetLoad(vec3d t) { SetVecValue(LOAD, t); }
    vec3d GetLoad() { return GetVecValue(LOAD); }
};

//-----------------------------------------------------------------------------

class FEFluidNormalVelocity : public FESurfaceLoad
{
public:
    enum {LOAD, BP, BPARAB};
    
public:
    FEFluidNormalVelocity(FEModel* ps);
    FEFluidNormalVelocity(FEModel* ps, FEItemListBuilder* pi, double vn, bool bp, bool bparab, int nstep = 0);
    
    FELoadCurve* GetLoadCurve() { return GetParamLC(LOAD); }
    
    void SetLoad(double f) { SetFloatValue(LOAD, f); }
    double GetLoad() { return GetFloatValue(LOAD); }
    
    void SetBP(bool b) { SetBoolValue(BP, b); }
    bool GetBP() { return GetBoolValue(BP); }
    
    void SetBParab(bool b) { SetBoolValue(BPARAB, b); }
    bool GetBParab() { return GetBoolValue(BPARAB); }
};

//-----------------------------------------------------------------------------

class FEFluidRotationalVelocity : public FESurfaceLoad
{
public:
    enum {LOAD, AXIS, ORIGIN };
    
public:
    FEFluidRotationalVelocity(FEModel* ps);
    FEFluidRotationalVelocity(FEModel* ps, FEItemListBuilder* pi, double w, vec3d n, vec3d p, int nstep = 0);
    
    FELoadCurve* GetLoadCurve() { return GetParamLC(LOAD); }
    
    void SetLoad(double w) { SetFloatValue(LOAD, w); }
    double GetLoad() { return GetFloatValue(LOAD); }
    
    void SetAxis(vec3d n) { SetVecValue(AXIS, n); }
    vec3d GetAxis() { return GetVecValue(AXIS); }
    
    void SetOrigin(vec3d p) { SetVecValue(ORIGIN, p); }
    vec3d GetOrigin() { return GetVecValue(ORIGIN); }
};

//-----------------------------------------------------------------------------

class FEFluidFlowResistance : public FESurfaceLoad
{
public:
    enum {LOAD, PO };
    
public:
    FEFluidFlowResistance(FEModel* ps);
    FEFluidFlowResistance(FEModel* ps, FEItemListBuilder* pi, double b, double p, int nstep = 0);
    
    FELoadCurve* GetLoadCurve() { return GetParamLC(LOAD); }
    
    void SetLoad(double f) { SetFloatValue(LOAD, f); }
    double GetLoad() { return GetFloatValue(LOAD); }
    
    FELoadCurve* GetPOLoadCurve() { return GetParamLC(PO); }
    
    void SetPO(double f) { SetFloatValue(PO, f); }
    double GetPO() { return GetFloatValue(PO); }
    
};

//-----------------------------------------------------------------------------

class FEFluidBackflowStabilization : public FESurfaceLoad
{
public:
    enum { LOAD };
    
public:
    FEFluidBackflowStabilization(FEModel* ps);
    FEFluidBackflowStabilization(FEModel* ps, FEItemListBuilder* pi, double b, int nstep = 0);
    
    FELoadCurve* GetLoadCurve() { return GetParamLC(LOAD); }
    
    void SetLoad(double f) { SetFloatValue(LOAD, f); }
    double GetLoad() { return GetFloatValue(LOAD); }
    
};

//-----------------------------------------------------------------------------

class FEFluidTangentialStabilization : public FESurfaceLoad
{
public:
    enum { LOAD };
    
public:
    FEFluidTangentialStabilization(FEModel* ps);
    FEFluidTangentialStabilization(FEModel* ps, FEItemListBuilder* pi, double b, int nstep = 0);
    
    FELoadCurve* GetLoadCurve() { return GetParamLC(LOAD); }
    
    void SetLoad(double f) { SetFloatValue(LOAD, f); }
    double GetLoad() { return GetFloatValue(LOAD); }
    
};

//-----------------------------------------------------------------------------

class FEFSITraction : public FESurfaceLoad
{
public:
    FEFSITraction(FEModel* ps);
    FEFSITraction(FEModel* ps, FEItemListBuilder* pi, int nstep = 0);
    FELoadCurve* GetLoadCurve() { return nullptr; }

};

//-----------------------------------------------------------------------------
// concentration flux for reaction-diffusion problems
class FEConcentrationFlux : public FESurfaceLoad
{
	enum { SOL_ID, FLUX };

public:
	FEConcentrationFlux(FEModel* ps);
	
	FELoadCurve* GetLoadCurve();

	void SetFlux(double f);
	double GetFlux();

	int GetSoluteID();
	void SetSoluteID(int n);
};
