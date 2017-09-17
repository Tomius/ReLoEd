// Copyright (c), Tamas Csala

#include <Silice3D/core/scene.hpp>

#include "fps_display.hpp"
#include "cdlod/cdlod_terrain_settings.hpp"

FpsDisplay::FpsDisplay(Silice3D::GameObject* parent)
    : Silice3D::GameObject(parent) {
  glm::vec2 window_size = scene()->engine()->window_size();

  fps_ = AddComponent<Silice3D::Label>(
           "FPS:", glm::vec2{0.98f, 0.04f} * window_size, 1.8f, glm::vec4(1));
  fps_->set_horizontal_alignment(Silice3D::HorizontalAlignment::kRight);

  geom_nodes_ = AddComponent<Silice3D::Label>(
             "Geometry nodes:", glm::vec2{0.98f, 0.08f} * window_size, 1.5f, glm::vec4(1));
  geom_nodes_->set_horizontal_alignment(Silice3D::HorizontalAlignment::kRight);

  triangle_count_ = AddComponent<Silice3D::Label>(
             "Triangles count:", glm::vec2{0.98f, 0.105f} * window_size, 1.5f, glm::vec4(1));
  triangle_count_->set_horizontal_alignment(Silice3D::HorizontalAlignment::kRight);

  triangle_per_sec_ = AddComponent<Silice3D::Label>(
             "Triangles per sec:", glm::vec2{0.98f, 0.13f} * window_size, 1.5f, glm::vec4(1));
  triangle_per_sec_->set_horizontal_alignment(Silice3D::HorizontalAlignment::kRight);

  texture_nodes_ = AddComponent<Silice3D::Label>(
             "Texture nodes:", glm::vec2{0.98f, 0.155f} * window_size, 1.5f, glm::vec4(1));
  texture_nodes_->set_horizontal_alignment(Silice3D::HorizontalAlignment::kRight);

  memory_usage_ = AddComponent<Silice3D::Label>(
             "GPU memory usage:", glm::vec2{0.98f, 0.18f} * window_size, 1.5f, glm::vec4(1));
  memory_usage_->set_horizontal_alignment(Silice3D::HorizontalAlignment::kRight);
}

FpsDisplay::~FpsDisplay() {
  std::cout << "FPS: "
    << min_fps_ << " min, "
    << sum_frame_num_ / sum_time_ << " avg, "
    << max_fps_ << " max" << std::endl;

  std::cout << "Triangles per frame: "
    << min_triangles_ << "K min, "
    << sum_triangle_num_ / sum_calls_ << "K avg, "
    << max_triangles_ << "K max" << std::endl;

  std::cout << "GPU Memory usage: "
    << min_memu_ << "MB min, "
    << sum_mem_usage_ / sum_calls_ << "MB avg, "
    << max_memu_ << "MB max" << std::endl;
}

void FpsDisplay::Update() {
  sum_time_ += scene_->camera_time().dt();
  if (sum_time_ < 0.0) {
    return;
  }
  accum_time_ += scene_->camera_time().dt();
  accum_calls_++;
  sum_calls_++;

  double fps = accum_calls_ / accum_time_;
  size_t geom_nodes_count = CdlodTerrainSettings::geom_nodes_count;
  size_t triangle_count = (geom_nodes_count
        << (2*(CdlodTerrainSettings::kNodeDimensionExp-1))) / 1000;
  size_t triangles_per_sec = triangle_count * fps / 1000;
  size_t gpu_mem_usage = CdlodTerrainSettings::texture_nodes_count
                         *(262*262*2 + 260*260*3)/1024/1024;

  sum_frame_num_ += 1;
  min_fps_ = std::min(min_fps_, fps);
  max_fps_ = std::max(max_fps_, fps);
  sum_triangle_num_ += triangle_count;
  min_triangles_ = std::min<double>(min_triangles_, triangle_count);
  max_triangles_ = std::max<double>(max_triangles_, triangle_count);
  sum_mem_usage_ += gpu_mem_usage;
  min_memu_ = std::min<double>(min_memu_, gpu_mem_usage);
  max_memu_ = std::max<double>(max_memu_, gpu_mem_usage);

  if (accum_time_ > kRefreshInterval) {
    fps_->set_text("FPS: " + std::to_string(static_cast<int>(fps)));

    geom_nodes_->set_text("Geometry nodes: " +
      std::to_string(geom_nodes_count));

    triangle_count_->set_text("Triangles count: " +
      std::to_string(triangle_count) + "K");

    triangle_per_sec_->set_text("Triangles per sec: " +
      std::to_string(triangles_per_sec) + "M");

    texture_nodes_->set_text("Texture nodes: " +
      std::to_string(CdlodTerrainSettings::texture_nodes_count));

    memory_usage_->set_text("GPU memory usage: " +
      std::to_string(gpu_mem_usage) + "MB");

    accum_time_ = accum_calls_ = 0;
  }
}

