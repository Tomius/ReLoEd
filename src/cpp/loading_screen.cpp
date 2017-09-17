// Copyright (c), Tamas Csala

#include <glad/glad.h>
#include <oglwrap/oglwrap.h>
#include <Silice3D/gui/label.hpp>

void ShowLoadingScreen(Silice3D::Scene* scene) {
  glm::vec2 window_size = scene->engine()->window_size();

  Silice3D::Label label{
    nullptr, "Land of Earth", glm::vec2{0.5, 0.44} * window_size, 5
  };
  label.set_vertical_alignment(Silice3D::VerticalAlignment::kCenter);
  label.set_horizontal_alignment(Silice3D::HorizontalAlignment::kCenter);
  // label.ScreenResized(window_size.x, window_size.y);

  Silice3D::Label label2{
    nullptr, "Loading please wait...", glm::vec2{0.5, 0.54} * window_size, 1.5
  };
  label2.set_vertical_alignment(Silice3D::VerticalAlignment::kCenter);
  label2.set_horizontal_alignment(Silice3D::HorizontalAlignment::kCenter);
  // label2.ScreenResized(window_size.x, window_size.y);

  gl::TemporarySet capabilities{{{gl::kBlend, true},
                                 {gl::kCullFace, false},
                                 {gl::kDepthTest, false}}};
  gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);
  label.Render2D();
  label2.Render2D();
}
