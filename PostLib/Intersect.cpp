#include "stdafx.h"
#include "Intersect.h"
using namespace Post;

//-----------------------------------------------------------------------------
// Find intersection of a ray with a triangle
bool Post::IntersectTriangle(const Ray& ray, const Triangle& tri, Intersection& intersection, bool evalNormal)
{
	const double tol = 0.01;

	vec3f n1 = tri.r0;
	vec3f n2 = tri.r1;
	vec3f n3 = tri.r2;

	vec3f r0 = ray.origin;
	vec3f nn = ray.direction;

	// calculate the triangle normal
	vec3f fn = tri.fn;
	if (evalNormal) { fn = (n2 - n1) ^ (n3 - n1); fn.Normalize(); }

	// find the intersection of the point with the plane
	if (fn*nn == 0.f) return false;
	double l = fn*(n1 - r0) / (fn*nn);

	//	if (l <= 0) return false;
	vec3f q = r0 + nn*l;

	// find  the natural coordinates
	vec3f e1 = n2 - n1;
	vec3f e2 = n3 - n1;

	double A[2][2] = { { e1*e1, e1*e2 }, { e2*e1, e2*e2 } };
	double D = A[0][0] * A[1][1] - A[0][1] * A[1][0];
	double Ai[2][2];
	Ai[0][0] = (A[1][1]) / D;
	Ai[1][1] = (A[0][0]) / D;
	Ai[0][1] = -A[0][1] / D;
	Ai[1][0] = -A[1][0] / D;

	vec3f E1 = e1*Ai[0][0] + e2*Ai[0][1];
	vec3f E2 = e1*Ai[1][0] + e2*Ai[1][1];

	double r = (q - n1)*E1;
	double s = (q - n1)*E2;

	intersection.point = q;
	intersection.r[0] = r;
	intersection.r[1] = s;

	return ((r >= -tol) && (s >= -tol) && (r + s <= 1.0 + tol));
}

//-----------------------------------------------------------------------------
bool Post::FastIntersectQuad(const Ray& ray, const Quad& quad, Intersection& intersect)
{
	Triangle tri[2] = {
		{ quad.r0, quad.r1, quad.r2 },
		{ quad.r2, quad.r3, quad.r0 }};

	return IntersectTriangle(ray, tri[0], intersect) || IntersectTriangle(ray, tri[1], intersect);
}

//-----------------------------------------------------------------------------
// Find intersection of a ray with a quad
bool Post::IntersectQuad(const Ray& ray, const Quad& quad, Intersection& intersect)
{
	const double tol = 0.01;

	vec3f nr = ray.origin;
	vec3f nn = ray.direction;

	vec3f y[4];
	y[0] = quad.r0 - nr;
	y[1] = quad.r1 - nr;
	y[2] = quad.r2 - nr;
	y[3] = quad.r3 - nr;

	double r = 0, s = 0, l = 0, normu;
	int niter = 0;
	const int NMAX = 10;
	do
	{
		// evaluate shape functions
		double H[4], Hr[4], Hs[4];
		H[0] = 0.25*(1 - r)*(1 - s);
		H[1] = 0.25*(1 + r)*(1 - s);
		H[2] = 0.25*(1 + r)*(1 + s);
		H[3] = 0.25*(1 - r)*(1 + s);

		Hr[0] = -0.25*(1 - s);
		Hr[1] = 0.25*(1 - s);
		Hr[2] = 0.25*(1 + s);
		Hr[3] = -0.25*(1 + s);

		Hs[0] = -0.25*(1 - r);
		Hs[1] = -0.25*(1 + r);
		Hs[2] = 0.25*(1 + r);
		Hs[3] = 0.25*(1 - r);

		// evaluate residual
		vec3f R = nn*l - y[0] * H[0] - y[1] * H[1] - y[2] * H[2] - y[3] * H[3];

		Mat3d K;
		K[0][0] = nn.x;
		K[0][1] = -y[0].x*Hr[0] - y[1].x*Hr[1] - y[2].x*Hr[2] - y[3].x*Hr[3];
		K[0][2] = -y[0].x*Hs[0] - y[1].x*Hs[1] - y[2].x*Hs[2] - y[3].x*Hs[3];

		K[1][0] = nn.y;
		K[1][1] = -y[0].y*Hr[0] - y[1].y*Hr[1] - y[2].y*Hr[2] - y[3].y*Hr[3];
		K[1][2] = -y[0].y*Hs[0] - y[1].y*Hs[1] - y[2].y*Hs[2] - y[3].y*Hs[3];

		K[2][0] = nn.z;
		K[2][1] = -y[0].z*Hr[0] - y[1].z*Hr[1] - y[2].z*Hr[2] - y[3].z*Hr[3];
		K[2][2] = -y[0].z*Hs[0] - y[1].z*Hs[1] - y[2].z*Hs[2] - y[3].z*Hs[3];

		K.Invert();

		vec3f du = K*R;
		l -= du.x;
		r -= du.y;
		s -= du.z;

		normu = du.y*du.y + du.z*du.z;
		niter++;
	} 
	while ((normu > 1e-6) && (niter < NMAX));

	if (niter < NMAX)
	{
		intersect.point = nr + nn*l;
		intersect.r[0] = r;
		intersect.r[1] = s;
	
		bool b = ((r + tol >= -1) && (r - tol <= 1) && (s + tol >= -1) && (s - tol <= 1));
		return b;
	}
	else return false;
}

//-----------------------------------------------------------------------------
bool Post::FindFaceIntersection(const Ray& ray, const FEMeshBase& mesh, const FEFace& face, Intersection& q)
{
	q.m_index = -1;

	vec3f rn[10];
	mesh.FaceNodePosition(face, rn);

	bool bfound = false;
	switch (face.m_ntype)
	{
	case FE_FACE_TRI3:
	case FE_FACE_TRI6:
	case FE_FACE_TRI7:
	case FE_FACE_TRI10:
	{
		Triangle tri = { rn[0], rn[1], rn[2] };
		bfound = IntersectTriangle(ray, tri, q);
	}
	break;
	case FE_FACE_QUAD4:
	case FE_FACE_QUAD8:
	case FE_FACE_QUAD9:
	{
		Quad quad = { rn[0], rn[1], rn[2], rn[3] };
		bfound = FastIntersectQuad(ray, quad, q);
	}
	break;
	}

	return bfound;
}
