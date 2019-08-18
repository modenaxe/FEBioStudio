#pragma once
#include <MeshTools/FEObject.h>
#include <MeshLib/box.h>

namespace Post {
	class CImageModel;
	class C3DImage;
}

class CGLView;

class GImageObject : public FEObject
{
public:
	GImageObject();
	~GImageObject();

	// load the image data
	bool LoadImageData(const std::string& fileName, int nx, int ny, int nz, BOX box);

	// set the physical dimensions of the image
	void SetBox(BOX box);

	// get the bounding box
	BOX GetBox() const;

	// update the image object
	void Update();

	// render the image data
	void Render(CGLView* view);

	// get the image model
	Post::CImageModel* GetImageModel();

private:
	Post::CImageModel*		m_im;		// the actual image data
};
