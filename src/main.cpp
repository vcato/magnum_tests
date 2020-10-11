#include <iostream>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Shaders/VertexColor.h>

#ifdef CORRADE_TARGET_ANDROID
#include <Magnum/Platform/AndroidApplication.h>
#elif defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Magnum/Platform/EmscriptenApplication.h>
#else
#include <Magnum/Platform/Sdl2Application.h>
#endif

namespace Math = Magnum::Math;
namespace Platform = Magnum::Platform;
namespace ImGuiIntegration = Magnum::ImGuiIntegration;
namespace Shaders = Magnum::Shaders;
namespace GL = Magnum::GL;
using Color4 = Magnum::Color4;
using Float = Magnum::Float;
using Double = Magnum::Double;
using Magnum::NoCreate;
using Magnum::Vector2;
using Magnum::Color3;
using namespace Math::Literals;
using std::cerr;


class MyApplication: public Platform::Application
{
    public:
        explicit MyApplication(const Arguments& arguments);

        void drawEvent() override;

        void viewportEvent(ViewportEvent& event) override;

        void keyPressEvent(KeyEvent& event) override;
        void keyReleaseEvent(KeyEvent& event) override;

        void mousePressEvent(MouseEvent& event) override;
        void mouseReleaseEvent(MouseEvent& event) override;
        void mouseMoveEvent(MouseMoveEvent& event) override;
        void mouseScrollEvent(MouseScrollEvent& event) override;
        void textInputEvent(TextInputEvent& event) override;

    private:
        ImGuiIntegration::Context _imgui{NoCreate};

        bool _showAnotherWindow = false;
        GL::Mesh _mesh;
        Shaders::VertexColor2D _shader;
        bool _toggle = false;
};


MyApplication::MyApplication(const Arguments& arguments)
: Platform::Application{
    arguments,
    Configuration{}
      .setTitle("Magnum ImGui Example")
      .setWindowFlags(Configuration::WindowFlag::Resizable)
  }
{
    _imgui = ImGuiIntegration::Context(Vector2{windowSize()}/dpiScaling(),
        windowSize(), framebufferSize());

    /* Set up proper blending to be used by ImGui. There's a great chance
       you'll need this exact behavior for the rest of your scene. If not, set
       this only for the drawFrame() call. */
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
        GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    struct TriangleVertex {
      Vector2 position;
      Color3 color;
    };

    const TriangleVertex data[] = {
      {{-0.5, 0.5}, 0xff0000_rgbf},
      {{ 0.5,-0.5}, 0x00ff00_rgbf},
      {{ 0.5, 0.5}, 0x0000ff_rgbf},
    };

    GL::Buffer buffer;
    buffer.setData(data);

    _mesh.setCount(3)
      .addVertexBuffer(
        std::move(buffer),
        /*offset*/0,
        /*attributes*/
        Shaders::VertexColor2D::Position(),
        Shaders::VertexColor2D::Color3()
      );

#if !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_ANDROID)
    /* Have some sane speed, please */
    setMinimalLoopPeriod(16);
#endif
}


void MyApplication::drawEvent()
{
    GL::defaultFramebuffer.clear(
      GL::FramebufferClear::Color |
      GL::FramebufferClear::Depth
    );

    if (_toggle) {
      _shader.draw(_mesh);
    }

    _imgui.newFrame();

    /* Enable text input, if needed */
    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    ImGui::Begin("Another Window", &_showAnotherWindow);
    ImGui::Text("Hello, world3!");
    ImGui::Checkbox("Toggle", &_toggle);
    ImGui::End();

    /* Update application cursor */
    _imgui.updateApplicationCursor(*this);

    /* Set appropriate states. If you only draw ImGui, it is sufficient to
       just enable blending and scissor test in the constructor. */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);

    _imgui.drawFrame();

    /* Reset state. Only needed if you want to draw something else with
       different state after. */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    swapBuffers();
    redraw();
}


void MyApplication::viewportEvent(ViewportEvent& event)
{
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    _imgui.relayout(Vector2{event.windowSize()}/event.dpiScaling(),
        event.windowSize(), event.framebufferSize());
}


void MyApplication::keyPressEvent(KeyEvent& event)
{
    if(_imgui.handleKeyPressEvent(event)) return;
}


void MyApplication::keyReleaseEvent(KeyEvent& event)
{
    if(_imgui.handleKeyReleaseEvent(event)) return;
}


void MyApplication::mousePressEvent(MouseEvent& event)
{
    if(_imgui.handleMousePressEvent(event)) return;
}


void MyApplication::mouseReleaseEvent(MouseEvent& event)
{
    if(_imgui.handleMouseReleaseEvent(event)) return;
}


void MyApplication::mouseMoveEvent(MouseMoveEvent& event)
{
    if(_imgui.handleMouseMoveEvent(event)) return;
}


void MyApplication::mouseScrollEvent(MouseScrollEvent& event)
{
    if(_imgui.handleMouseScrollEvent(event)) {
        /* Prevent scrolling the page */
        event.setAccepted();
        return;
    }
}

void MyApplication::textInputEvent(TextInputEvent& event)
{
    if(_imgui.handleTextInputEvent(event)) return;
}


MAGNUM_APPLICATION_MAIN(MyApplication)
