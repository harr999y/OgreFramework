#ifndef _OGRESAMPLE22_
#define _OGRESAMPLE22_

#include "../CEGUIBase/CEGUIBaseApp.h"
#include <OgrePrerequisites.h>
#include <OgreSimpleRenderable.h>
#include <OgreQuaternion.h>
#include <OgreVector3.h>
#include <OgreTexture.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreHardwareVertexBuffer.h>
#include <OgreHardwareIndexBuffer.h>
#include <OgreTextureManager.h>
#include <OgreLogManager.h>
#include <sstream>
#include <OgreCamera.h>

using namespace Ogre;

//Julia
#include <cmath>

struct Quat
{
	float r,i,j,k;
};

inline void qadd (Quat & a,const Quat & b)
{
	a.r += b.r;
	a.i += b.i;
	a.j += b.j;
	a.k += b.k;
}

inline void qmult(Quat &c, const Quat &a, const Quat &b) 
{
	c.r = a.r*b.r - a.i*b.i - a.j*b.j - a.k*b.k;
	c.i = a.r*b.i + a.i*b.r + a.j*b.k - a.k*b.j;
	c.j = a.r*b.j + a.j*b.r + a.k*b.i - a.i*b.k;
	c.k = a.r*b.k + a.k*b.r + a.i*b.j - a.j*b.i;
}

inline void qsqr(Quat &b, const Quat &a) 
{
	b.r = a.r*a.r - a.i*a.i - a.j*a.j - a.k*a.k;
	b.i = 2.0f*a.r*a.i;
	b.j = 2.0f*a.r*a.j;
	b.k = 2.0f*a.r*a.k;
}

/**
 * Implicit function that evaluates the Julia set.
 */
class Julia {
private:
	float global_real, global_imag, global_theta;
	Quat oc,c,eio,emio;
public:
	Julia(float global_real, float global_imag, float global_theta):
	  global_real(global_real), global_imag(global_imag), global_theta(global_theta) 
	  {

		  oc.r = global_real;
		  oc.i = global_imag;
		  oc.j = oc.k = 0.0;

		  eio.r = cos(global_theta);
		  eio.i = sin(global_theta);
		  eio.j = 0.0;
		  eio.k = 0.0;

		  emio.r = cos(-global_theta);
		  emio.i = sin(-global_theta);
		  emio.j = 0.0;
		  emio.k = 0.0;

		  /***
		  *** multiply eio*c only once at the beginning of iteration
		  *** (since q |-> sqrt(eio*(q-eio*c)))
		  *** q -> e-io*q^2 - eio*c
		  ***/

		  qmult(c, eio,oc);
	  }

	inline float eval(float x, float y, float z) {
		Quat q, temp;
		int i;
	
		q.r = x;
		q.i = y;
		q.j = z;
		q.k = 0.0;
	
		for (i = 30; i > 0; i--) {
			qsqr(temp, q);
			qmult(q, emio, temp);
			qadd(q, c);
	
			if (q.r*q.r + q.i*q.i + q.j*q.j + q.k*q.k > 8.0)
				break;
		}
	
		return((float)i);
	}
};

class ThingRenderable : public Ogre::SimpleRenderable
{
public:
	ThingRenderable(float radius,size_t count,float qsize);
	~ThingRenderable();

	Real getBoundingRadius() const;
	Real getSquaredViewDepth(const Camera*) const;
	void addTime(float t);

protected:
	void initialise();
	void fillBuffer();

	Ogre::HardwareVertexBufferSharedPtr vbuf;
	float mRadius;
	size_t mCount;
	float mQSize;
	Ogre::vector <Ogre::Quaternion>::type things;
	Ogre::vector <Ogre::Quaternion>::type orbits;
};

class VolumeRenderable : public SimpleRenderable
{
protected:
	void initialise();

	size_t mSlices;
	float mSize;
	float mRadius;
	Ogre::Matrix3 mFakeOrientation;
	String mTexture;
	TextureUnitState * mUnit;

public:
	VolumeRenderable(size_t nSlices,float size,const String & texture);
	~VolumeRenderable();

	void _notifyCurrentCamera(Camera* cam);
	void getWorldTransforms( Matrix4* xform ) const;
	Real getBoundingRadius() const;
	Real getSquaredViewDepth(const Camera* cam) const;
};

class OgreSample22App : public CEGUIBaseApp
{
public:
	TexturePtr mTex;
	SimpleRenderable * mTRend;
	SimpleRenderable * mVRend;
	Overlay * mOverlay;
	SceneNode * mNode;
	float global_real,global_imag,global_theta;

	void createScene();
	void frameRenderingQueued(const FrameEvent& evt);
	void destroyScene();
	void generate();
};

#endif