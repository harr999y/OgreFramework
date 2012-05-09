#include <stdio.h>
#include "MyCharacterController.h"

#define COLLIDABLE_MASK	(1<<GROUP_COLLIDABLE_NON_PUSHABLE)

MyCharacterController::MyCharacterController(NxControllerManager* manager, NxScene *scene, NxVec3 pos, NxVec3 dim)
{
	mManager = manager;
	mClosestPoint.zero();

	NxBoxControllerDesc desc;
	desc.position.x = pos.x;
	desc.position.y = pos.y;
	desc.position.z = pos.z;
	desc.extents = dim;
	desc.skinWidth = 0.01f;
	desc.slopeLimit = cosf(NxMath::degToRad(45.0f));
	desc.stepOffset = 0.5f;
	desc.upDirection = NX_Y;
	desc.callback = &mReport;
	mController = mManager->createController(scene, desc);
}

MyCharacterController::MyCharacterController(NxControllerManager* manager, NxScene *scene, NxVec3 pos, NxReal radius, NxReal height)
{
	mManager = manager;
	mClosestPoint.zero();

	NxCapsuleControllerDesc desc;
	desc.position.x = pos.x;
	desc.position.y = pos.y;
	desc.position.z = pos.z;
	desc.height = height;
	desc.radius = radius;
	desc.skinWidth = 0.01f;
	desc.slopeLimit = cosf(NxMath::degToRad(45.0f));
	desc.stepOffset = 0.5f;
	desc.upDirection = NX_Y;
	desc.climbingMode = CLIMB_EASY;
	desc.callback = &mReport;
	mController = mManager->createController(scene, desc);
}

MyCharacterController::~MyCharacterController()
{
// 	if(mManager)
// 	{
// 		mManager->releaseController(*mController);
// 	}
}

void MyCharacterController::Move(const NxVec3 &disp, NxU32 &flag)
{
	mController->move(disp, COLLIDABLE_MASK, 0.001, flag);
}

NxActor* MyCharacterController::GetCharacterActor()
{
	return mController->getActor();
}

bool MyCharacterController::SetCharacterPos(NxVec3 pos)
{
	return mController->setPosition(NxExtendedVec3(pos.x, pos.y, pos.z));
}

void MyCharacterController::UpdateCharacterExtents(NxScene *scene, NxVec3 offset)
{
	switch (mController->getType())
	{
	case NX_CONTROLLER_BOX:
		{
			NxBoxController* boxCtrl = ((NxBoxController*) mController);
			NxExtendedVec3 pos = boxCtrl->getPosition(); 
			NxVec3 dim = boxCtrl->getExtents() + offset; // expanded extent
			if (dim.x <= 0.0f || dim.y <= 0.0f || dim.z <= 0.0f)
				break;
			NxBounds3 worldBounds;
			worldBounds.setCenterExtents(NxVec3(pos.x, pos.y + offset.y, pos.z), dim); // expand upward
			
			boxCtrl->setCollision(false); // avoid checking overlap with ourselves
			if (scene->checkOverlapAABB(worldBounds))
				printf("\n>> Can NOT change the size of the box controller!");
			else
			{
				boxCtrl->setExtents(dim);
				boxCtrl->setPosition(pos + NxExtendedVec3(0, offset.y, 0));
			}
			boxCtrl->setCollision(true);
			break;
		}
	case NX_CONTROLLER_CAPSULE:
		{
			NxCapsuleController* capsuleCtrl = ((NxCapsuleController*) mController);
			NxF32 height = capsuleCtrl->getHeight() + offset.y; // height offset = offset.y
			NxF32 radius = capsuleCtrl->getRadius() + offset.y/2; // radius offset = offset.y/2
			NxExtendedVec3 pos = capsuleCtrl->getPosition();
			pos.y += offset.y; // radius offset + 1/2 * height offset
			if (height <= 0.0f || radius <= 0.0f)
				break;
			NxCapsule worldCapsule;
			worldCapsule.p0 = NxVec3(pos.x, pos.y-height/2, pos.z);
			worldCapsule.p1 = NxVec3(pos.x, pos.y+height/2, pos.z);
			worldCapsule.radius = radius;
			capsuleCtrl->setCollision(false);
			if (scene->checkOverlapCapsule(worldCapsule))
			{
				printf("\n>> Can NOT change the size of the capsule controller!");
			}
			else
			{
				capsuleCtrl->setHeight(height);
				capsuleCtrl->setRadius(radius);
				capsuleCtrl->setPosition(pos);
			}
			capsuleCtrl->setCollision(true);
			break;
		}
		break;
	}
}
