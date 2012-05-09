#include "OgreSample22.h"

ThingRenderable::ThingRenderable(float radius, size_t count, float qsize) : mRadius(radius),mCount(count),mQSize(qsize)
{
	mBox = Ogre::AxisAlignedBox(-radius,-radius,-radius,radius,radius,radius);
	initialise();
	fillBuffer();
}

ThingRenderable::~ThingRenderable()
{
	delete mRenderOp.indexData;
	delete mRenderOp.vertexData;
}

void ThingRenderable::addTime(float t)
{
	for (size_t x=0;x<mCount;x++)
	{
		Quaternion dest = things[x] * orbits[x];
		things[x] = things[x] + t * (dest - things[x]);
		things[x].normalise();
	}
	fillBuffer();
}

float randFloat()
{
	return ((float)rand() / RAND_MAX) * 2.0f -1.0f;
}

void ThingRenderable::initialise()
{
	Vector3 ax,ay,az;
	size_t x;
	Quaternion q;
	things.clear();
	orbits.clear();
	for (x=0;x<mCount;x++)
	{
		ax = Vector3(randFloat(), randFloat(), randFloat());
		ay = Vector3(randFloat(), randFloat(), randFloat());
		az = ax.crossProduct(ay);
		ay = az.crossProduct(ax);
		ax.normalise(); ay.normalise(); az.normalise();
		q.FromAxes(ax, ay, az);
		//std::cerr << ax.dotProduct(ay) << " " << ay.dotProduct(az) << " " << az.dotProduct(ax) << std::endl;
		things.push_back(q);

		ax = Vector3(randFloat(), randFloat(), randFloat());
		ay = Vector3(randFloat(), randFloat(), randFloat());
		az = ax.crossProduct(ay);
		ay = az.crossProduct(ax);
		ax.normalise(); ay.normalise(); az.normalise();
		q.FromAxes(ax, ay, az);
		orbits.push_back(q);
	}

	size_t nvertices = mCount * 4;
	IndexData * idata = new Ogre::IndexData();
	VertexData * vdata = new Ogre::VertexData();
	unsigned short *faces = new unsigned short[mCount*6];
	for(x=0; x<mCount; x++) 
	{
		faces[x*6+0] = x*4+0;
		faces[x*6+1] = x*4+1;
		faces[x*6+2] = x*4+2;
		faces[x*6+3] = x*4+0;
		faces[x*6+4] = x*4+2;
		faces[x*6+5] = x*4+3;
	}

	vdata->vertexStart = 0;
	vdata->vertexCount = nvertices;

	VertexDeclaration * decl = vdata->vertexDeclaration;
	VertexBufferBinding * bind = vdata->vertexBufferBinding;

	size_t offset = 0;
	decl->addElement(0,offset,VET_FLOAT3,VES_POSITION);
	offset += VertexElement::getTypeSize(VET_FLOAT3);

	vbuf = HardwareBufferManager::getSingleton().createVertexBuffer(offset,nvertices,HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	bind->setBinding(0,vbuf);
	
	HardwareIndexBufferSharedPtr ibuf = HardwareBufferManager::getSingleton().createIndexBuffer(HardwareIndexBuffer::IT_16BIT,mCount * 6,HardwareIndexBuffer::HBU_STATIC_WRITE_ONLY);

	idata->indexBuffer = ibuf;
	idata->indexCount = mCount * 6;
	idata->indexStart = 0;
	ibuf->writeData(0,ibuf->getSizeInBytes(),faces,true);

	delete [] faces;
	mRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
	mRenderOp.indexData = idata;
	mRenderOp.vertexData = vdata;
	mRenderOp.useIndexes = true;
}

void ThingRenderable::fillBuffer()
{
	float * vIdx = static_cast<float *> (vbuf->lock(HardwareBuffer::HBL_DISCARD));
	size_t elemsize = 1 * 3;
	size_t planesize = 4 * elemsize;
	for (size_t x=0;x<mCount;x++)
	{
		Vector3 ax,ay,az;
		things[x].ToAxes(ax,ay,az);
		Vector3 pos = az * mRadius; 
		ax *= mQSize;
		ay *= mQSize;
		Vector3 pos1 = pos - ax - ay;
		Vector3 pos2 = pos + ax - ay;
		Vector3 pos3 = pos + ax + ay;
		Vector3 pos4 = pos - ax + ay;
		vIdx[x*planesize + 0*elemsize + 0] = pos1.x;
		vIdx[x*planesize + 0*elemsize + 1] = pos1.y;
		vIdx[x*planesize + 0*elemsize + 2] = pos1.z;
		vIdx[x*planesize + 1*elemsize + 0] = pos2.x;
		vIdx[x*planesize + 1*elemsize + 1] = pos2.y;
		vIdx[x*planesize + 1*elemsize + 2] = pos2.z;
		vIdx[x*planesize + 2*elemsize + 0] = pos3.x;
		vIdx[x*planesize + 2*elemsize + 1] = pos3.y;
		vIdx[x*planesize + 2*elemsize + 2] = pos3.z;
		vIdx[x*planesize + 3*elemsize + 0] = pos4.x;
		vIdx[x*planesize + 3*elemsize + 1] = pos4.y;
		vIdx[x*planesize + 3*elemsize + 2] = pos4.z;		
	}
	vbuf->unlock();
}

Ogre::Real ThingRenderable::getBoundingRadius() const
{
	return mRadius;
}

Real ThingRenderable::getSquaredViewDepth(const Ogre::Camera *cam) const
{
	Vector3 min,max,mid,dist;
	min = mBox.getMinimum();
	max = mBox.getMaximum();
	mid = ((min - max) * 0.5f) + min;
	dist = cam->getDerivedPosition() - mid;
	return dist.squaredLength();
}

VolumeRenderable::VolumeRenderable(size_t nSlices, float size, const Ogre::String &texture) : mSlices(nSlices),mSize(size),mTexture(texture)
{
	mRadius = sqrtf(size * size * 3) / 2.0f;
	mBox = AxisAlignedBox(-size,-size,-size,size,size,size);
	setCastShadows(false);
	initialise();
}

VolumeRenderable::~VolumeRenderable()
{
	MaterialManager::getSingleton().remove(mTexture);
	delete mRenderOp.indexData;
	delete mRenderOp.vertexData;
}

void VolumeRenderable::_notifyCurrentCamera(Ogre::Camera *cam)
{
	MovableObject::_notifyCurrentCamera(cam);
	Vector3 zVec = getParentNode()->_getDerivedPosition() - cam->getDerivedPosition();
	zVec.normalise();
	Vector3 fixedAxis = cam->getDerivedOrientation() * Vector3::UNIT_Y ;

	Vector3 xVec = fixedAxis.crossProduct( zVec );
	xVec.normalise();

	Vector3 yVec = zVec.crossProduct( xVec );
	yVec.normalise();

	Quaternion oriQuat;
	oriQuat.FromAxes( xVec, yVec, zVec );

	oriQuat.ToRotationMatrix(mFakeOrientation);

	Matrix3 tempMat;
	Quaternion q = getParentNode()->_getDerivedOrientation().UnitInverse() * oriQuat ;
	q.ToRotationMatrix(tempMat);

	Matrix4 rotMat = Matrix4::IDENTITY;
	rotMat = tempMat;
	rotMat.setTrans(Vector3(0.5f, 0.5f, 0.5f));
	mUnit->setTextureTransform(rotMat);
}

void VolumeRenderable::getWorldTransforms( Matrix4* xform ) const
{
	Matrix4 destMatrix(Matrix4::IDENTITY); // this initialisation is needed

	const Vector3 &position = getParentNode()->_getDerivedPosition();
	const Vector3 &scale = getParentNode()->_getDerivedScale();
	Matrix3 scale3x3(Matrix3::ZERO);
	scale3x3[0][0] = scale.x;
	scale3x3[1][1] = scale.y;
	scale3x3[2][2] = scale.z;

	destMatrix = mFakeOrientation * scale3x3;
	destMatrix.setTrans(position);

	*xform = destMatrix;
}

void VolumeRenderable::initialise()
{
	// Create geometry
	size_t nvertices = mSlices*4; // n+1 planes
	size_t elemsize = 3*3;
	size_t dsize = elemsize*nvertices;
	size_t x;

	Ogre::IndexData *idata = new Ogre::IndexData();
	Ogre::VertexData *vdata = new Ogre::VertexData();

	// Create  structures
	float *vertices = new float[dsize];

	float coords[4][2] = {
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f}
	};
	for(x=0; x<mSlices; x++) 
	{
		for(size_t y=0; y<4; y++)
		{
			float xcoord = coords[y][0]-0.5;
			float ycoord = coords[y][1]-0.5;
			float zcoord = -((float)x/(float)(mSlices-1)  - 0.5f);
			// 1.0f .. a/(a+1)
			// coordinate
			vertices[x*4*elemsize+y*elemsize+0] = xcoord*(mSize/2.0f);
			vertices[x*4*elemsize+y*elemsize+1] = ycoord*(mSize/2.0f);
			vertices[x*4*elemsize+y*elemsize+2] = zcoord*(mSize/2.0f);
			// normal
			vertices[x*4*elemsize+y*elemsize+3] = 0.0f;
			vertices[x*4*elemsize+y*elemsize+4] = 0.0f;
			vertices[x*4*elemsize+y*elemsize+5] = 1.0f;
			// tex
			vertices[x*4*elemsize+y*elemsize+6] = xcoord*sqrtf(3.0f);
			vertices[x*4*elemsize+y*elemsize+7] = ycoord*sqrtf(3.0f);
			vertices[x*4*elemsize+y*elemsize+8] = zcoord*sqrtf(3.0f);
		} 
	}
	unsigned short *faces = new unsigned short[mSlices*6];
	for(x=0; x<mSlices; x++) 
	{
		faces[x*6+0] = x*4+0;
		faces[x*6+1] = x*4+1;
		faces[x*6+2] = x*4+2;
		faces[x*6+3] = x*4+1;
		faces[x*6+4] = x*4+2;
		faces[x*6+5] = x*4+3;
	}
	// Setup buffers
	vdata->vertexStart = 0;
	vdata->vertexCount = nvertices;

	VertexDeclaration* decl = vdata->vertexDeclaration;
	VertexBufferBinding* bind = vdata->vertexBufferBinding;

	size_t offset = 0;
	decl->addElement(0,offset,VET_FLOAT3,VES_POSITION);
	offset += VertexElement::getTypeSize(VET_FLOAT3);
	decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
	offset += VertexElement::getTypeSize(VET_FLOAT3);
	decl->addElement(0, offset, VET_FLOAT3, VES_TEXTURE_COORDINATES);
	offset += VertexElement::getTypeSize(VET_FLOAT3);

	HardwareVertexBufferSharedPtr vbuf = 
		HardwareBufferManager::getSingleton().createVertexBuffer(
		offset, nvertices, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	bind->setBinding(0, vbuf);

	vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);

	HardwareIndexBufferSharedPtr ibuf = HardwareBufferManager::getSingleton().
		createIndexBuffer(
		HardwareIndexBuffer::IT_16BIT, 
		mSlices*6, 
		HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	idata->indexBuffer = ibuf;
	idata->indexCount = mSlices*6;
	idata->indexStart = 0;
	ibuf->writeData(0, ibuf->getSizeInBytes(), faces, true);

	// Delete temporary buffers
	delete [] vertices;
	delete [] faces;

	// Now make the render operation
	mRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
	mRenderOp.indexData = idata;
	mRenderOp.vertexData = vdata;
	mRenderOp.useIndexes = true;

	if (!ResourceGroupManager::getSingleton().resourceGroupExists("VolumeRenderable"))
	{
		ResourceGroupManager::getSingleton().createResourceGroup("VolumeRenderable");
	}

	MaterialPtr material = MaterialManager::getSingleton().create(mTexture,"VolumeRenderable",false,0);
	material->removeAllTechniques();

	Technique * technique = material->createTechnique();
	Pass * pass = technique->createPass();
	TextureUnitState * textureUnit = pass->createTextureUnitState();

	pass->setSceneBlending(SBT_TRANSPARENT_ALPHA);
	pass->setDepthWriteEnabled(false);
	pass->setCullingMode(CULL_NONE);
	pass->setLightingEnabled(false);

	textureUnit->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
	textureUnit->setTextureName(mTexture,TEX_TYPE_3D);
	textureUnit->setTextureFiltering(TFO_TRILINEAR);

	mUnit = textureUnit;
	m_pMaterial = material;
}

Ogre::Real VolumeRenderable::getBoundingRadius() const
{
	return mRadius;
}
Ogre::Real VolumeRenderable::getSquaredViewDepth(const Ogre::Camera* cam) const
{
	Ogre::Vector3 min, max, mid, dist;

	min = mBox.getMinimum();
	max = mBox.getMaximum();
	mid = ((min - max) * 0.5) + min;
	dist = cam->getDerivedPosition() - mid;

	return dist.squaredLength();
}



void OgreSample22App::createScene()
{
	mTex = TextureManager::getSingleton().createManual("DynaTex","General",TEX_TYPE_3D,64,64,64,0,PF_A8R8G8B8);
	mSceneMgr->setAmbientLight(ColourValue(0.6, 0.6, 0.6));
	mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox", 50 );

	//mRoot->getRenderSystem()->clearFrameBuffer(FBT_COLOUR, ColourValue(255,255,255,0));

	// Create a light
	Light* l = mSceneMgr->createLight("MainLight");
	l->setDiffuseColour(0.75, 0.75, 0.80);
	l->setSpecularColour(0.9, 0.9, 1);
	l->setPosition(-100,80,50);
	mSceneMgr->getRootSceneNode()->attachObject(l);

	// Create volume renderable
	mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0,0,0));      
	mVRend = new VolumeRenderable(32,750.f,"DynaTex");
	mNode->attachObject(mVRend);

	/*mTRend = new ThingRenderable(90.0f,32,7.5f);
	mTRend->setMaterial("Examples/VTDarkStuff");
	mNode->attachObject(mTRend);*/

	global_real = 0.4f;
	global_imag = 0.6f;
	global_theta = 0.0f;
	generate();
}

void OgreSample22App::frameRenderingQueued(const FrameEvent& evt)
{
	//static_cast<ThingRenderable *>(mTRend)->addTime(evt.timeSinceLastFrame * 0.05f);
}

void OgreSample22App::generate()
{
	Julia julia(global_real,global_imag,global_theta);
	const float scale = 2.5f;
	const float vcut = 29.0f;
	const float vscale = 1.0f / vcut;

	HardwarePixelBufferSharedPtr buffer = mTex->getBuffer(0,0);
	StringStream d;
	d << "HardwarePixelBuffer " << buffer->getWidth() << " " << buffer->getHeight() << " " << buffer->getDepth();
	LogManager::getSingleton().logMessage(d.str());

	buffer->lock(HardwareBuffer::HBL_NORMAL);
	const PixelBox & pb = buffer->getCurrentLock();
	d.str("");
	d << "PixelBox " << pb.getWidth() << " " << pb.getHeight() << " " << pb.getDepth() << " " << pb.rowPitch << " " << pb.slicePitch << " " << pb.data << " " << PixelUtil::getFormatName(pb.format);
	LogManager::getSingleton().logMessage(d.str());

	uint32 * pbptr = static_cast<uint32*>(pb.data);
	for (size_t z=pb.front;z<pb.back;z++)
	{
		for (size_t y=pb.top;y<pb.bottom;y++)
		{
			for (size_t x=pb.left;x<pb.right;x++)
			{
				if (z==pb.front || z==(pb.back-1) || y==pb.top || y==(pb.bottom-1) || x==pb.left || x==(pb.right-1))
				{
					pbptr[x] = 0;
				}
				else
				{
					float val = julia.eval(((float)x/pb.getWidth()-0.5f) * scale, 
						((float)y/pb.getHeight()-0.5f) * scale, 
						((float)z/pb.getDepth()-0.5f) * scale);
					if(val > vcut)
						val = vcut;
					PixelUtil::packColour((float)x / pb.getWidth(),(float)y / pb.getHeight(),(float)z / pb.getDepth(),(1.0f-(val*vscale))*0.7f,PF_A8R8G8B8,&pbptr[x]);
				}
			}
			pbptr += pb.rowPitch;
		}
		pbptr += pb.getSliceSkip();
	}
	buffer->unlock();
}

void OgreSample22App::destroyScene()
{
	TextureManager::getSingleton().remove("DynaTex");
	delete mVRend;
	//delete mTRend;
}