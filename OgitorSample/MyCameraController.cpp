#include "MyCameraController.h"

void MyCameraController::setup()
{
	SceneManager * sMgr = mCamera->getSceneManager();
	mCameraPivot = sMgr->getRootSceneNode()->createChildSceneNode();
	mCameraOriginal  = mCameraPivot->createChildSceneNode(Vector3(0, 1.3, -20));
	mCameraNode = sMgr->getRootSceneNode()->createChildSceneNode();

	mCameraNode->attachObject(mCamera);

	mCameraNode->setFixedYawAxis(true);
	mCameraOriginal->setFixedYawAxis(true);
	mCameraPivot->setFixedYawAxis(true);
}

void MyCameraController::update(float deltaTime)
{
	mCameraPivot->setPosition(mBodyNode->getPosition() + Vector3(0, 0.6, 0));
	mCameraOriginal->_update(true,true);
	mCameraOriginal->lookAt(mCameraPivot->getPosition(),Node::TS_WORLD);

	Vector3 vo = mCameraOriginal->_getDerivedPosition();
	Vector3 vb = mCameraPivot->_getDerivedPosition();
	Ogre::Ray ray;
	NxVec3 vc(vb.x,vb.y,vb.z);
	NxVec3 vecDir = NxVec3(vo.x - vb.x,vo.y - vb.y,vo.z - vb.z);
	float length = vecDir.normalize();
	NxRay nxRay(vc, vecDir);
	NxRaycastHit hit;
	NxShape * shape = mNxScene->raycastClosestShape(nxRay, NX_ALL_SHAPES, hit);

	if (shape && hit.distance < length)
	{
		Vector3 hitWorld(hit.worldImpact.x,hit.worldImpact.y,hit.worldImpact.z);

		//为了防止穿透
		float transZ = -0.8f;
		//float transY = (-0.8f) * vo.y / (NxMath::sqrt(vo.x * vo.x + vo.z * vo.z));  //这个还是算了，不加上去。。。
		hitWorld += mCameraOriginal->_getDerivedOrientation() * Vector3(0,0,transZ);
		Vector3 hitLocal = -mCameraPivot->convertWorldToLocalPosition(hitWorld);
		mCameraNode->setPosition(hitWorld);
		mCameraNode->lookAt(mCameraPivot->getPosition(),Node::TS_WORLD);

		//为了防止在角落里时摄像机反向
		if (hitLocal.z < 0)
		{
			mCameraNode->setPosition(mCameraPivot->getPosition());// + Vector3(0,1.1,0));  //这个效果和魔兽世界还有暗黑血统相似，但他们是不是这么写的就不得而知了~ 呵呵
			Vector3 v = mCameraPivot->getOrientation() * Vector3(0,0,3);
			mCameraNode->lookAt(mCameraPivot->getPosition() + v,Node::TS_WORLD);
		}
		bViewRestore = true;
	}
	else
	{
		if (bViewRestore)
		{
			Vector3 transPos = vo - mCameraNode->getPosition();
			Vector3 localPos = mCameraPivot->convertWorldToLocalPosition(mCameraNode->getPosition());
			Vector3 originalPos = mCameraOriginal->getPosition();
			if (localPos.z  > originalPos.z)
			{
				localPos.z -= 25 * deltaTime;
			}
			if (localPos.z <= originalPos.z)
			{
				localPos.z = originalPos.z;
				bViewRestore = false;
			}
			else
				originalPos.z = localPos.z;

			mCameraNode->setPosition(mCameraPivot->convertLocalToWorldPosition(originalPos));
		}
		else
			mCameraNode->setPosition(vo);

		mCameraNode->lookAt(mCameraPivot->getPosition(),Node::TS_WORLD);
	}

	Vector3 cameraNodePos = mCameraNode->getPosition();
	float height = mTerrainGroup->getHeightAtWorldPosition(cameraNodePos.x,0,cameraNodePos.z) + 0.5f;
	if (cameraNodePos.y < height)
	{
		cameraNodePos.y = height;
		mCameraNode->setPosition(cameraNodePos);
	}
}

void MyCameraController::updateGoal(float pitchAngle,float yawAngle,float deltaZoom)
{
	mCameraPivot->yaw(Degree(yawAngle),Node::TS_LOCAL);
	//mBodyNode->yaw(Degree(deltaYaw),Node::TS_LOCAL);

	//if(mPivotPitch + pitchAngle <= 60 && mPivotPitch + pitchAngle >= -40)
	{
		mCameraPivot->pitch(Degree(pitchAngle), Node::TS_LOCAL);
		mPivotPitch += pitchAngle;
	}

	Real dist = mBodyNode->_getDerivedPosition().distance(mCameraOriginal->_getDerivedPosition());
	Real distChange = deltaZoom * dist;

	// bound the zoom
	if (!bViewRestore && dist + distChange >= 6 && dist + distChange <= 28)
	{
		mCameraOriginal->translate(0, 0, distChange, Node::TS_LOCAL);
	}
}
