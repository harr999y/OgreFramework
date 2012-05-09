#include "CEGUISample9.h"
#include <cstdlib>

const float MyEffect::tess_x = 8.0f;
const float MyEffect::tess_y = 8.0f;

MyEffect::MyEffect() : initialised(false),dragX(0.f),dragY(0.f),elasX(0.f),elasY(0.f),lastX(0.0f),lastY(0.0f)
{}

int MyEffect::getPassCount() const
{
	return 1;
}

void MyEffect::performPreRenderFunctions(const int )//pass)
{}

void MyEffect::performPostRenderFunctions()
{}

bool MyEffect::realiseGeometry(CEGUI::RenderingWindow &window, CEGUI::GeometryBuffer &geometry)
{
	using namespace CEGUI;
	Texture& tex = window.getTextureTarget().getTexture();

	static const CEGUI::colour c(1.0f,1.0f,1.0f,1.0f);

	const float qw = window.getSize().d_width / tess_x;
	const float qh = window.getSize().d_height / tess_y;
	const float tcx = qw * tex.getTexelScaling().d_x;
	const float tcy = (window.getTextureTarget().isRenderingInverted() ? -qh : qh) * tex.getTexelScaling().d_y;
	for(int j = 0;j<tess_y;++j)
	{
		for(int i= 0;i<tess_x;++i)
		{
			int idx = (j*tess_x+i)*6;
            float top_adj = dragX * ((1.0f / tess_x) * j);
            float bot_adj = dragX * ((1.0f / tess_x) * (j+1));
            top_adj = ((top_adj*top_adj) / 3) * (dragX < 0 ? -1 : 1);
            bot_adj = ((bot_adj*bot_adj) / 3) * (dragX < 0 ? -1 : 1);

            float lef_adj = dragY * ((1.0f / tess_y) * i);
            float rig_adj = dragY * ((1.0f / tess_y) * (i+1));
            lef_adj = ((lef_adj*lef_adj) / 3) * (dragY < 0 ? -1 : 1);
            rig_adj = ((rig_adj*rig_adj) / 3) * (dragY < 0 ? -1 : 1);

            // vertex 0
            vb[idx + 0].position   = Vector3(i * qw - top_adj, j * qh - lef_adj, 0.0f);
            vb[idx + 0].colour_val = c;
            vb[idx + 0].tex_coords = Vector2(i * tcx, j*tcy);

            // vertex 1
            vb[idx + 1].position   = Vector3(i * qw - bot_adj, j * qh + qh - lef_adj, 0.0f);
            vb[idx + 1].colour_val = c;
            vb[idx + 1].tex_coords = Vector2(i*tcx, j*tcy+tcy);

            // vertex 2
            vb[idx + 2].position   = Vector3(i * qw + qw - bot_adj, j * qh + qh - rig_adj, 0.0f);
            vb[idx + 2].colour_val = c;
            vb[idx + 2].tex_coords = Vector2(i*tcx+tcx, j*tcy+tcy);

            // vertex 3
            vb[idx + 3].position   = Vector3(i * qw + qw - bot_adj, j * qh + qh - rig_adj, 0.0f);
            vb[idx + 3].colour_val = c;
            vb[idx + 3].tex_coords = Vector2(i*tcx+tcx, j*tcy+tcy);

            // vertex 4
            vb[idx + 4].position   = Vector3(i * qw + qw - top_adj, j * qh - rig_adj, 0.0f);
            vb[idx + 4].colour_val = c;
            vb[idx + 4].tex_coords = Vector2(i*tcx+tcx, j*tcy);

            // vertex 5
            vb[idx + 5].position   = Vector3(i * qw - top_adj, j * qh - lef_adj, 0.0f);
            vb[idx + 5].colour_val = c;
            vb[idx + 5].tex_coords = Vector2(i * tcx, j*tcy);
		}
	}

	geometry.setActiveTexture(&tex);
	geometry.appendGeometry(vb,buffsize);
	return false;
}

bool MyEffect::update(const float elapsed, CEGUI::RenderingWindow &window)
{
	using namespace CEGUI;

	if(!initialised)
	{
		initialised = true;
		lastX = window.getPosition().d_x;
		lastY = window.getPosition().d_y;
		return true;
	}

	const Vector2 pos(window.getPosition());

	if(pos.d_x != lastX)
	{
		dragX += (pos.d_x - lastX) * 0.2;
		elasX = 0.05f;
		lastX = pos.d_x;

		if(dragX > 30)
			dragX = 30;
		else if(dragX < -30)
			dragX = -30;
	}

	if(pos.d_y != lastY)
	{
        dragY += (pos.d_y - lastY) * 0.2f;
        elasY = 0.05f;
        lastY = pos.d_y;

        if (dragY > 30)
            dragY = 30;
        else if (dragY < -30)
            dragY = -30;
	}

	if((dragX != 0) || (dragY != 0))
	{
		if(dragX < 0)
		{
			dragX += (elasX * 800 * elapsed);
			elasX +=0.075 * elapsed;
			if(dragX > 0)
				dragX = 0;
		}
        else
        {
            dragX -= (elasX * 800 * elapsed);
            elasX += 0.075 * elapsed;
            if (dragX < 0)
                dragX = 0;
        }

        if (dragY < 0)
        {
            dragY += elasY * 800 * elapsed;
            elasY += 0.075 * elapsed;
            if (dragY >0)
                dragY = 0;
        }
        else
        {
            dragY -= elasY * 800 * elapsed;
            elasY += 0.075 * elapsed;
            if (dragY < 0)
                dragY = 0;
        }

		System::getSingleton().signalRedraw();
		return false;
	}
	return true;
}

void CEGUISample9App::createGUI()
{
	using namespace CEGUI;

	RenderEffectManager::getSingleton().addEffect <MyEffect> ("WobblyWindow");

	WindowFactoryManager::getSingleton().addFalagardWindowMapping("TaharezLook/WobblyFrameWindow","CEGUI/FrameWindow","TaharezLook/FrameWindow","Falagard/FrameWindow","WobblyWindow");

	WindowManager& winMgr = WindowManager::getSingleton();
	SchemeManager::getSingleton().create("TaharezLook.scheme");
    System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
    FontManager::getSingleton().create("DejaVuSans-10.font");

    // load an image to use as a background
    ImagesetManager::getSingleton().createFromImageFile("BackgroundImage", "GPN-2000-001437.tga");

    // here we will use a StaticImage as the root, then we can use it to place a background image
    Window* background = winMgr.createWindow("TaharezLook/StaticImage", "background_wnd");
    // set position and size
    background->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0)));
    background->setSize(UVector2(cegui_reldim(1), cegui_reldim( 1)));
    // disable frame and standard background
    background->setProperty("FrameEnabled", "false");
    background->setProperty("BackgroundEnabled", "false");
    // set the background image
    background->setProperty("Image", "set:BackgroundImage image:full_image");
    // install this as the root GUI sheet
    System::getSingleton().setGUISheet(background);

    // load the windows for Demo7 from the layout file.
    Window* sheet = winMgr.loadWindowLayout("Demo7Windows.layout");
    // attach this to the 'real' root
    background->addChildWindow(sheet);

	createListContent();
	initDemoEventWiring();
}

void CEGUISample9App::createListContent()
{
    using namespace CEGUI;

    WindowManager& winMgr = WindowManager::getSingleton();

	Combobox * cbobox = static_cast<Combobox*>(winMgr.getWindow("Demo7/Window2/Combobox"));
    // add items to the combobox list
    cbobox->addItem(new MyListItem("Combobox Item 1"));
    cbobox->addItem(new MyListItem("Combobox Item 2"));
    cbobox->addItem(new MyListItem("Combobox Item 3"));
    cbobox->addItem(new MyListItem("Combobox Item 4"));
    cbobox->addItem(new MyListItem("Combobox Item 5"));
    cbobox->addItem(new MyListItem("Combobox Item 6"));
    cbobox->addItem(new MyListItem("Combobox Item 7"));
    cbobox->addItem(new MyListItem("Combobox Item 8"));
    cbobox->addItem(new MyListItem("Combobox Item 9"));
    cbobox->addItem(new MyListItem("Combobox Item 10"));

    //
    // Multi-Column List setup
    //
    MultiColumnList* mclbox = static_cast<MultiColumnList*>(winMgr.getWindow("Demo7/Window2/MultiColumnList"));
    // Add some empty rows to the MCL
    mclbox->addRow();
    mclbox->addRow();
    mclbox->addRow();
    mclbox->addRow();
    mclbox->addRow();

    // Set first row item texts for the MCL
    mclbox->setItem(new MyListItem("Laggers World"), 0, 0);
    mclbox->setItem(new MyListItem("yourgame.some-server.com"), 1, 0);
    mclbox->setItem(new MyListItem("[colour='FFFF0000']1000ms"), 2, 0);

    // Set second row item texts for the MCL
    mclbox->setItem(new MyListItem("Super-Server"), 0, 1);
    mclbox->setItem(new MyListItem("whizzy.fakenames.net"), 1, 1);
    mclbox->setItem(new MyListItem("[colour='FF00FF00']8ms"), 2, 1);

    // Set third row item texts for the MCL
    mclbox->setItem(new MyListItem("Cray-Z-Eds"), 0, 2);
    mclbox->setItem(new MyListItem("crayzeds.notarealserver.co.uk"), 1, 2);
    mclbox->setItem(new MyListItem("[colour='FF00FF00']43ms"), 2, 2);

    // Set fourth row item texts for the MCL
    mclbox->setItem(new MyListItem("Fake IPs"), 0, 3);
    mclbox->setItem(new MyListItem("123.320.42.242"), 1, 3);
    mclbox->setItem(new MyListItem("[colour='FFFFFF00']63ms"), 2, 3);

    // Set fifth row item texts for the MCL
    mclbox->setItem(new MyListItem("Yet Another Game Server"), 0, 4);
    mclbox->setItem(new MyListItem("abc.abcdefghijklmn.org"), 1, 4);
    mclbox->setItem(new MyListItem("[colour='FFFF6600']284ms"), 2, 4);

    mclbox->setProperty("Font", "fkp-16");
}

void CEGUISample9App::initDemoEventWiring()
{
	using namespace CEGUI;

    // Subscribe handler that processes changes to the slider position.
    WindowManager::getSingleton().getWindow("Demo7/Window1/Slider1")->
        subscribeEvent(Slider::EventValueChanged, Event::Subscriber(&CEGUISample9App::handleSlider, this));

    // Subscribe handler that processes changes to the checkbox selection state.
    WindowManager::getSingleton().getWindow("Demo7/Window1/Checkbox")->
        subscribeEvent(Checkbox::EventCheckStateChanged, Event::Subscriber(&CEGUISample9App::handleCheck, this));

    // Subscribe handler that processes changes to the radio button selection state.
    WindowManager::getSingleton().getWindow("Demo7/Window1/Radio1")->
        subscribeEvent(RadioButton::EventSelectStateChanged, Event::Subscriber(&CEGUISample9App::handleRadio, this));

    // Subscribe handler that processes changes to the radio button selection state.
    WindowManager::getSingleton().getWindow("Demo7/Window1/Radio2")->
        subscribeEvent(RadioButton::EventSelectStateChanged, Event::Subscriber(&CEGUISample9App::handleRadio, this));

    // Subscribe handler that processes changes to the radio button selection state.
    WindowManager::getSingleton().getWindow("Demo7/Window1/Radio3")->
        subscribeEvent(RadioButton::EventSelectStateChanged, Event::Subscriber(&CEGUISample9App::handleRadio, this));
}

bool CEGUISample9App::handleSlider(const CEGUI::EventArgs &e)
{
	using namespace CEGUI;

	float val = static_cast<Slider*>(static_cast<const WindowEventArgs&>(e).window)->getCurrentValue();

	static_cast<ProgressBar*>(WindowManager::getSingleton().getWindow("Demo7/Window2/Progbar1"))->setProgress(val);

	static_cast<ProgressBar*>(WindowManager::getSingleton().getWindow("Demo7/Window2/Progbar2"))->setProgress(1.0f - val);

	WindowManager::getSingleton().getWindow("root")->setAlpha(val);

	return true;
}

bool CEGUISample9App::handleRadio(const CEGUI::EventArgs &e)
{
	using namespace CEGUI;
	CEGUI::uint id = static_cast<RadioButton*>(static_cast<const WindowEventArgs&>(e).window)->getSelectedButtonInGroup()->getID();
	Window* img = WindowManager::getSingleton().getWindow("Demo7/Window2/Image1");
    // set an image into the StaticImage according to the ID of the selected radio button.
    switch (id)
    {
    case 0:
        img->setProperty("Image", "set:BackgroundImage image:full_image");
        break;

    case 1:
        img->setProperty("Image", "set:TaharezLook image:MouseArrow");
        break;

    default:
        img->setProperty("Image", "");
        break;
    }

    // event was handled
    return true;
}

bool CEGUISample9App::handleCheck(const CEGUI::EventArgs& e)
{
    using namespace CEGUI;

    // show or hide the FrameWindow containing the multi-line editbox according to the state of the
    // checkbox widget
    WindowManager::getSingleton().getWindow("Demo7/Window3")->
        setVisible(static_cast<Checkbox*>(static_cast<const WindowEventArgs&>(e).window)->isSelected());

    // event was handled.
    return true;
}
