// Copyright (c), Tamas Csala

#ifndef LOE_FPS_DISPLAY_H_
#define LOE_FPS_DISPLAY_H_

#include <Silice3D/core/game_object.hpp>
#include <Silice3D/gui/label.hpp>

class FpsDisplay : public Silice3D::GameObject {
 public:
  FpsDisplay(Silice3D::GameObject* parent);
  ~FpsDisplay();

 private:
  Silice3D::Label *fps_;
  Silice3D::Label *geom_nodes_, *triangle_count_, *triangle_per_sec_;
  Silice3D::Label *texture_nodes_, *memory_usage_;

  constexpr static const float kRefreshInterval = 0.1;
  double sum_frame_num_ = 0, min_fps_ = 1.0/0.0, max_fps_ = 0;
  double sum_triangle_num_ = 0, min_triangles_ = 1.0/0.0, max_triangles_ = 0;
  double sum_mem_usage_ = 0, min_memu_ = 1.0/0.0, max_memu_ = 0;
  double sum_time_ = -0.1, sum_calls_ = 0, accum_time_ = 0, accum_calls_ = 0;

  virtual void Update() override;
  virtual void ScreenResized(size_t width, size_t height) override;
};

#endif
