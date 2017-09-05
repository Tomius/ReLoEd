Land of Earth Reloaded
======================

Land of Earth is a global spherical terrain renderer that is based on a 172800 * 86400 heightmap, and it works realtime. It is an "extension" of [Filip Strugar's CDLOD concept](http://www.vertexasylum.com/downloads/cdlod/cdlod_latest.pdf), with texture streaming of course. This is a rewritten version of my original [Land of Earth](https://github.com/Tomius/LoE) project, with better performance and visual quality. Unfortunately I couldn't upload the dataset required to run this code.


![screenshot](screenshots/screenshot.png)

[![video](screenshots/video.png)](https://youtu.be/n4WSMzjVrB8)


Dependencies (linux only):
-------------------------
```
libmagick++-dev clang cmake xorg-dev libglu1-mesa-dev libfreetype6-dev
```

Camera usage (press space to switch between them):
----------------------------------------------------
* FPS camera
  * WASD keys: position
  * mouse move: camera direction
  * mouse scroll: movement speed
* Sphere viewer camera (default):
  * mouse move: position
  * mouse scroll: zoom

----------------------
If you have any problem, please post in the issues tab or mail me at icyplusplus@gmail.com. Any feedback would be appreciated.

