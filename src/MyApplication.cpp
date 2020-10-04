#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Math/Color.h>

#define ADD_IMGUI 1

#if ADD_IMGUI
#include <Magnum/ImGuiIntegration/Context.hpp>
#endif

using namespace Magnum;


class MyApplication : public Platform::Application
{
  public:
    explicit MyApplication(const Arguments& arguments);

  private:
#if ADD_IMGUI
    ImGuiIntegration::Context _imgui{NoCreate};
#endif
    void drawEvent() override;
};


MyApplication::MyApplication(const Arguments& arguments)
  : Platform::Application{arguments}
{
  /* TODO: Add your initialization code here */
  using namespace Math::Literals;
  GL::Renderer::setClearColor(0xa5c9ea_rgbf);
#if ADD_IMGUI
  _imgui.newFrame();
#endif
}


void MyApplication::drawEvent()
{
  GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

  /* TODO: Add your drawing code here */

  swapBuffers();
}


MAGNUM_APPLICATION_MAIN(MyApplication)
