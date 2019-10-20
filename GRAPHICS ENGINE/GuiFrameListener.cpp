//mem probs without this next one
#include <OgreNoMemoryMacros.h>
#include <CEGUI/CEGUIImageset.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUILogger.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/elements/CEGUIPushButton.h>
#include "OgreCEGUIRenderer.h"
#include "OgreCEGUIResourceProvider.h"
//regular mem handler
#include <OgreMemoryMacros.h>

#include "ExampleApplication.h"

class GuiFrameListener : public FrameListener, public KeyListener
{
private:
  CEGUI::Renderer* mGUIRenderer;
public:
  GuiFrameListener(RenderWindow* win, Camera* cam, CEGUI::Renderer* renderer)
  {
	  renderer = mGUIRenderer;
  }
};