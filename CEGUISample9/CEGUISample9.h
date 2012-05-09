#ifndef _CEGUISAMPLE9_
#define _CEGUISAMPLE9_

#include "../CEGUIBase/CEGUIBaseApp.h"

class MyEffect : public CEGUI::RenderEffect
{
public:
	MyEffect();

	int getPassCount() const;
	void performPreRenderFunctions(const int pass);
	void performPostRenderFunctions();
	virtual bool realiseGeometry(CEGUI::RenderingWindow& window,CEGUI::GeometryBuffer& geometry);
	virtual bool update(const float elapsed,CEGUI::RenderingWindow& window);
    
protected:
	static const float tess_x;
	static const float tess_y;
	static const int buffsize = (8 * 8 * 6);

	bool initialised;

	float lastX,lastY;
	float dragX,dragY;
	float elasX,elasY;

	CEGUI::Vertex vb[buffsize];
};

class CEGUISample9App : public CEGUIBaseApp
{
public:
	void createGUI();

protected:
	void createListContent();
	void initDemoEventWiring();

	bool handleSlider(const CEGUI::EventArgs& e);
	bool handleRadio(const CEGUI::EventArgs& e);
	bool handleCheck(const CEGUI::EventArgs& e);
};

class MyListItem : public CEGUI::ListboxTextItem
{
public:
	MyListItem(const CEGUI::String& text) : ListboxTextItem(text)
	{
		setSelectionBrushImage("TaharezLook","MultiListSelectionBrush");
	}
};

#endif