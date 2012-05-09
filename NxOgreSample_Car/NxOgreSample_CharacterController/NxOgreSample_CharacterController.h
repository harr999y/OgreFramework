#include "../CEGUIBase/CEGUIBaseApp.h"

class NxOgreSample_CCApp : public CEGUIBaseApp
{
public:
	void createScene();
	void createPhysX();
	void setupScene();
protected:
	Critter::AnimatedCharacter * mCC;
	
};