#ifndef SCENES_MAIN_SCENE_H_
#define SCENES_MAIN_SCENE_H_

#include <Silice3D/core/scene.hpp>
#include <Silice3D/camera/third_personal_camera.hpp>
#include <Silice3D/camera/free_fly_camera.hpp>

#include "loading_screen.hpp"
#include "skybox.hpp"
#include "terrain.hpp"
#include "fps_display.hpp"
#include "scattering.hpp"

class MainScene : public Silice3D::Scene {
 public:
  MainScene(Silice3D::GameEngine* engine, GLFWwindow* window)
      : Scene(engine, window) {
    #if !ENGINE_NO_FULLSCREEN
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    #endif

    ShowLoadingScreen(scene());
    glfwSwapBuffers(window);

    AddComponent<Skybox>();
    AddComponent<Terrain>();

    int radius = CdlodTerrainSettings::kSphereRadius;
    tp_camera_ = AddComponent<Silice3D::ThirdPersonalCamera>(
        M_PI/3, 2, 3*radius, glm::vec3(-2*radius, 0, 0),
        5, 1, 0.005, 4, radius, radius);
    set_camera(tp_camera_);

    AddComponent<Scattering>();
    AddComponent<FpsDisplay>();
  }

  private:
    Silice3D::FreeFlyCamera* free_fly_camera_ = nullptr;
    Silice3D::ThirdPersonalCamera* tp_camera_ = nullptr;

  virtual void KeyAction(int key, int scancode, int action, int mods) override {
    int radius = CdlodTerrainSettings::kSphereRadius;
    if (action == GLFW_PRESS) {
      if (key == GLFW_KEY_SPACE) {
        if (free_fly_camera_) {
          glm::dvec3 pos = free_fly_camera_->transform().pos();
          double current_dist = glm::length(pos) - radius;
          if (current_dist < 100) {
            current_dist = radius + 100;
            pos = current_dist * glm::normalize(pos);
          }

          tp_camera_ = AddComponent<Silice3D::ThirdPersonalCamera>(
              M_PI/3, 2, 3*radius, pos,
              5, 1, 0.005, 4, radius, radius);
          RemoveComponent(free_fly_camera_);
          free_fly_camera_ = nullptr;
          set_camera(tp_camera_);
        } else {
          free_fly_camera_ = AddComponent<Silice3D::FreeFlyCamera>(
            M_PI/3, 2, 3*radius, tp_camera_->transform().pos(), glm::vec3(), 10, 5);
          glm::vec3 up{glm::normalize(free_fly_camera_->transform().pos())};
          free_fly_camera_->transform().set_forward(-up + glm::vec3{0, 0.25, 0});
          free_fly_camera_->transform().set_up(up);
          RemoveComponent(tp_camera_);
          tp_camera_ = nullptr;
          set_camera(free_fly_camera_);
        }
      } else if (key == GLFW_KEY_KP_9) {
        RemoveComponent(camera());
        tp_camera_ = nullptr;

        double old_radius = 27501;
        double radius = CdlodTerrainSettings::kSphereRadius;
        double scale = radius / old_radius;
        free_fly_camera_ = AddComponent<Silice3D::FreeFlyCamera>(
            M_PI/3, 2, 3*radius, scale * glm::dvec3{-18829.6, 19925.4, 3163.6},
            scale * glm::dvec3{-18829.101141, 19925.3065359, 3164.461629}, 0, 0);
        set_camera(free_fly_camera_);
      } else if (key == GLFW_KEY_KP_8) {
        RemoveComponent(camera());
        tp_camera_ = nullptr;

        double old_radius = 27501;
        double radius = CdlodTerrainSettings::kSphereRadius;
        double scale = radius / old_radius;
        free_fly_camera_ = AddComponent<Silice3D::FreeFlyCamera>(
            M_PI/3, 2, 3*radius, scale * glm::dvec3{-9943.69, 16404.8, 20361.8},
            scale * glm::dvec3{-9942.71, 16404.6, 20361.7}, 50, 0);
        set_camera(free_fly_camera_);
      } else if (key == GLFW_KEY_KP_1) {
        CdlodTerrainSettings::render = !CdlodTerrainSettings::render;
      } else if (key == GLFW_KEY_KP_2) {
        CdlodTerrainSettings::update = !CdlodTerrainSettings::update;
      }
    }
  }

  virtual void Update() override {
    auto cam = dynamic_cast<Silice3D::PerspectiveCamera*>(scene()->camera());
    double height = glm::length(cam->transform().pos());
    double z_far = 1000 * height, z_near = 1;
    cam->set_z_far(z_far);
    cam->set_z_near(z_near);
    if (free_fly_camera_) {
      auto t = free_fly_camera_->transform();
      glm::vec3 new_up = glm::normalize(t.pos());
      t.set_up(new_up);
    }
  }

  virtual void MouseScrolled(double xoffset, double yoffset) override {
    if (free_fly_camera_) {
      auto cam = free_fly_camera_;
      cam->set_speed_per_sec(cam->speed_per_sec() * (yoffset > 0 ? 1.1 : 0.9));
    }
  }
};

#endif
