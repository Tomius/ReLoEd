// Copyright (c), Tamas Csala

/*       _                     _          __   _____           _   _
 *      | |                   | |        / _| |  ___|         | | | |
 *      | |     __ _ _ __   __| |   ___ | |_  | |__  __ _ _ __| |_| |__
 *      | |    / _` | '_ \ / _` |  / _ \|  _| |  __|/ _` | '__| __| '_ \
 *      | |___| (_| | | | | (_| | | (_) | |   | |__| (_| | |  | |_| | | |
 *      \_____/\__,_|_| |_|\__,_|  \___/|_|   \____/\__,_|_|   \__|_| |_|
 */

#include "main_scene.hpp"

int main(int argc, char* argv[]) {
  try {
    Silice3D::GameEngine engine;
    engine.LoadScene(std::unique_ptr<Silice3D::Scene>{new MainScene{&engine, engine.window()}});
    engine.Run();
  } catch(const std::exception& err) {
    std::cerr << err.what();
  }
}
