# VR Voxel Game with Headset and Controller Integration
- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Controls](#controls)
- [Screenshots](#screenshots)
- [Credits/Copyright](#creditscopyright)
- [Final Words](#final-words)

# Overview
A repository for our first-year C group project at Imperial College London containing:
- A VR voxel game written in C with OpenGL.
- A custom VR headset design (Raspberry Pi 4) along with code to track headset orientation, integrated into the game.
- A custom bluetooth controller design (Raspberry Pi Pico) with code.

The game runs at about 30-35 FPS on the headset (Raspberry Pi 4). 

We placed 2nd out of 58 teams and won the "**Most Interesting Extension**" prize.


https://github.com/user-attachments/assets/068f9dd2-8918-490c-8be2-e46a97b15cb6

# Features
- Procedural terrain generation (infinite world).
- Vertex lighting.
- Breaking and placing blocks.
- Physics system.
- 3D Audio with miniaudio.
- Render distance-based fog.
- Extremely optimised (frustum culling, meshing algorithms, multi-threading etc).

# Installation
- [Running the game on PC (Windows, Mac, Linux).](#pc-installation-windows-mac-linux)
- [Building the headset and running the game on it (Raspberry Pi 4).]()

## PC Installation (Windows, Mac, Linux)
**Note:** Linux users will need to install the GLFW dependencies separately in order to build, see https://www.glfw.org/docs/3.3/compile.html.

1. Clone this repository and its submodules.

   ```
   git clone --recursive https://github.com/lxkast/vr-voxel-game.git
   ```

2. Build with CMake (use GCC/Clang/MinGW).

   ```
   cd vr-voxel-game/
   ```
   ```
   cmake -B build
   ```
   ```
   cmake --build build
   ```

3. The game executable is found in `./build/game/src`.

## Building the VR Headset
Our headset is powered by a Raspberry Pi 4. Below is an overview of the headset's setup.

<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/884662aa-ff0c-4a37-89ea-24c3fe18a1df" />

The ICM-42688 is connected like so:

![headset electronics drawio](https://github.com/user-attachments/assets/0c210079-015f-408b-aa73-c655c1c78981)

### Raspberry Pi 4 Setup
1. Install the GLFW dependencies, see https://www.glfw.org/docs/3.3/compile.html.
   
2. Clone this repository:
   
   ```
   git clone --recursive https://github.com/lxkast/vr-voxel-game.git
   ```
   
3. Build:

     ```
     cd vr-voxel-game/
     ```

     ```
     cmake -DBUILD_FOR_RPI=ON -B build
     ```
   
     ```
     cmake --build build
     ```
  
4. Run:

   ```
   cd build/game/src
   ```
   
   ```
   ./game
   ```

# Controls

## Controls on PC:
- WASD - movement
- Left Click - mine a block
- Right Click - place a block
- Space - Jump
- Number Keys - select an item in your hotbar
- B - opens 3D hotbar
- P - switches between normal view (one image without distortion) and headset view (two distorted images)
- O - shows the wireframe view of the world
- ESC - quit

## Controls with Headset and Controller:

Head movement controls where you look and the controller handles everything else.

Below shows what we decided on for our controller: 

<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/2e28a9e8-7e04-4beb-a42d-7dc4fd04e75f" />


Should you decide to use your own controller, our program uses 5 buttons from the controller, which are taken as part of an array, as well as an axis input from a joystick. This is how each button is assigned:

- Button 0 - Opens the 3D hotbar
- Button 1 - Mines a block
- Button 2 - Place a block
- Button 3 - Sprinting
- Button 4 - Jump

# Screenshots

<img width="2040" height="1188" alt="image" src="https://github.com/user-attachments/assets/580bbdf4-6a7a-43e3-991f-abaf22c5b024" />
<img width="1014" height="596" alt="image" src="https://github.com/user-attachments/assets/2f6b8d74-f454-4473-8283-f4aa70f01cc8" />
<img width="1917" height="1050" alt="image" src="https://github.com/user-attachments/assets/3b03df9f-14b5-4edd-9765-f9cf23648bbc" />
<img width="1014" height="591" alt="image" src="https://github.com/user-attachments/assets/0d915dda-b6d9-48db-922f-d546414433a9" />


# Credits/Copyright

All code outside of `game/external` is our own, with the exception of `gl.c`.

All textures are our own.

Audio files are sources from [pixabay.com](https://pixabay.com) and are used in accordance with their [content license](https://pixabay.com/service/license-summary/).

# Final Words

This repository was migrated from the original GitLab repository owned by the university. Commit messages are prepended with our personal shortcodes for marking purposes.

This project had a 4-week deadline, so we did not have time to make major refactors for any poor architectural decisions. There have been no major changes to the project post-submission.
