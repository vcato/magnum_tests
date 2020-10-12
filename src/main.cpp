#include <iostream>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>

#ifdef CORRADE_TARGET_ANDROID
#include <Magnum/Platform/AndroidApplication.h>
#elif defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Magnum/Platform/EmscriptenApplication.h>
#else
#include <Magnum/Platform/Sdl2Application.h>
#endif

#define ENABLE_DEBUG_OUTPUT 0


#include <Magnum/Shaders/Phong.h>

namespace Math = Magnum::Math;
namespace Platform = Magnum::Platform;
namespace ImGuiIntegration = Magnum::ImGuiIntegration;
namespace Shaders = Magnum::Shaders;
namespace GL = Magnum::GL;
using std::ostream;
using Color4 = Magnum::Color4;
using Float = Magnum::Float;
using Double = Magnum::Double;
using Magnum::NoCreate;
using Magnum::Vector2;
using Magnum::Color3;
using namespace Math::Literals;

#if ENABLE_DEBUG_OUTPUT
using std::cerr;


static ostream& operator<<(ostream&stream, const Magnum::Vector2i &v)
{
  stream << "Vector2i(" << v.x() << "," << v.y() << ")";
  return stream;
}
#endif


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
    Shaders::Phong _shader;
    bool _toggle = false;
    bool _mouse_is_down = false;
    Magnum::Vector2i _mouse_down_pos;
    Magnum::Math::Rad<float> _mouse_down_rot_x;
    Magnum::Math::Rad<float> _mouse_down_rot_y;
    Magnum::Math::Rad<float> _rot_x{0};
    Magnum::Math::Rad<float> _rot_y{0};
};


MyApplication::MyApplication(const Arguments& arguments)
  : Platform::Application{
    arguments,
      Configuration{}
    .setTitle("Magnum ImGui Example")
      .setWindowFlags(Configuration::WindowFlag::Resizable)
  }
{
  _imgui =
    ImGuiIntegration::Context(
      /*size*/Vector2{windowSize()}/dpiScaling(),
      windowSize(),
      framebufferSize()
    );

  /* Set up proper blending to be used by ImGui. There's a great chance
     you'll need this exact behavior for the rest of your scene. If not, set
     this only for the drawFrame() call. */
  GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
    GL::Renderer::BlendEquation::Add);
  GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
    GL::Renderer::BlendFunction::OneMinusSourceAlpha);

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
    struct TriangleVertex {
      Magnum::Vector3 position;
      Magnum::Vector3 normal;
      Color3 color;
    };

    Vector2 corner = {-0.5, 0.5};

    const TriangleVertex data[] = {
      {{corner.x()  , corner.y()  , 0}, {0,0,1}, 0xff0000_rgbf},
      {{corner.x()+1, corner.y()-1, 0}, {0,0,1}, 0x00ff00_rgbf},
      {{corner.x()+1, corner.y()  , 0}, {0,0,1}, 0x0000ff_rgbf},
    };

    GL::Buffer buffer;
    buffer.setData(data);

    GL::Mesh mesh;
    mesh.setCount(3)
      .addVertexBuffer(
        std::move(buffer),
        /*offset*/0,
        /*attributes*/
        Shaders::Phong::Position(),
        Shaders::Phong::Normal(),
        Shaders::Phong::Color3()
      );

    using Matrix = Magnum::Matrix4;

    Matrix transformation_matrix =
      Matrix::translation(Magnum::Vector3::zAxis(-5));

    transformation_matrix =
      transformation_matrix *
      Matrix::rotationX(_rot_x) *
      Matrix::rotationY(_rot_y);

    Matrix projection_matrix =
      Matrix::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

    _shader.setDiffuseColor(0x0000ff_rgbf);
    _shader.setTransformationMatrix(transformation_matrix);
    _shader.setNormalMatrix(transformation_matrix.normalMatrix());
    _shader.setProjectionMatrix(projection_matrix);
    _shader.draw(mesh);
  }

  _imgui.newFrame();

  /* Enable text input, if needed */
  if (ImGui::GetIO().WantTextInput && !isTextInputActive())
    startTextInput();
  else if (!ImGui::GetIO().WantTextInput && isTextInputActive())
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

  _imgui.relayout(
    Vector2{event.windowSize()}/event.dpiScaling(),
    event.windowSize(),
    event.framebufferSize()
  );
}


void MyApplication::keyPressEvent(KeyEvent& event)
{
  if (_imgui.handleKeyPressEvent(event)) return;
}


void MyApplication::keyReleaseEvent(KeyEvent& event)
{
  if (_imgui.handleKeyReleaseEvent(event)) return;
}


#if 0
static Magnum::Vector2
viewPos(
  const Magnum::Vector2i &window_pos,
  const Magnum::Vector2i &window_size
)
{
  float x = 2*(window_pos.x() / float(window_size.x())) - 1;
  float y = 1 - 2*(window_pos.y() / float(window_size.y()));
  return Magnum::Vector2(x,y);
}
#endif


static MyApplication::MouseEvent::Button mouseButton()
{
  return MyApplication::MouseEvent::Button::Left;
}


void MyApplication::mousePressEvent(MouseEvent& event)
{
  if (_imgui.handleMousePressEvent(event)) {
    return;
  }

  if (event.button() == mouseButton()) {
    _mouse_is_down = true;
    _mouse_down_pos = event.position();
    _mouse_down_rot_x = _rot_x;
    _mouse_down_rot_y = _rot_y;
  }
}


void MyApplication::mouseReleaseEvent(MouseEvent& event)
{
  if (_imgui.handleMouseReleaseEvent(event)) return;

  if (event.button() == mouseButton()) {
    _mouse_is_down = false;
  }
}


void MyApplication::mouseMoveEvent(MouseMoveEvent& event)
{
  if (_imgui.handleMouseMoveEvent(event)) {
    return;
  }

  if (_mouse_is_down) {
    Magnum::Vector2i delta = event.position() - _mouse_down_pos;
    using Deg = Magnum::Math::Deg<float>;
    using Rad = Magnum::Math::Rad<float>;
    _rot_x = _mouse_down_rot_x + Rad(Deg(delta.y()));
    _rot_y = _mouse_down_rot_y + Rad(Deg(delta.x()));
  }
}


void MyApplication::mouseScrollEvent(MouseScrollEvent& event)
{
  if (_imgui.handleMouseScrollEvent(event)) {
    /* Prevent scrolling the page */
    event.setAccepted();
    return;
  }
}

void MyApplication::textInputEvent(TextInputEvent& event)
{
  if (_imgui.handleTextInputEvent(event)) return;
}


MAGNUM_APPLICATION_MAIN(MyApplication)
