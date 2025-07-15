
### Overview

As part of the Imperial College London first year C project, we coded a VR Minecraft-like voxel game, and designed a working VR headset and wireless controller. We placed 2nd out of 58 teams and won the "Most Interesting Extension" prize.

The game can be played either on a computer or on the headset we designed, working on all operating systems. This project has been optimised to run on a Raspberry Pi 4, running at a comfortable 30-35 fps, meaning it runs very well on a good laptop or PC. After running into bugs caused by the frame rate being too high, we decided to cap the fps at the screen's refresh rate.

#### Setup Instructions

We do not distribute prebuilt versions, so you have to build it yourself.

Running on a Raspberry Pi:
1. Install dependencies:
   
   `sudo apt install -y build-essential cmake libglm-dev libglfw3-dev libgl1-mesa-dev mesa-utils`
   
2. Clone this repository:
   
   `git clone --recursive https://github.com/lxkast/vr-voxel-game.git`
   
3. Build:

  `cd vr-voxel-game/`

  `cmake -DBUILD_FOR_RPI=ON -B build`

  `cmake --build build`
  
4. Run:

   `cd build/game`
   
   `./game`

For the above to work you will have to have an ICM-42688 connected as shown on the following wiring diagram:

![headset electronics drawio](https://github.com/user-attachments/assets/0c210079-015f-408b-aa73-c655c1c78981)

#### Controls

##### Controls on PC:
- WASD - movement
- Left Click - mine a block
- Right Click - place a block
- Number Keys - select an item in your hotbar
- B key - opens 3D hotbar

- P key - switches between normal view (one image without distortion) and headset view (two distorted images)
- O key - shows the wireframe view of the world

##### Controls with Headset and Controller:

Head movement controls where you look and the controller handles everything else.

#TODO


#### Credits/Copyright

All code not in external is completely written by us, with the exception of `gl.c`. Some external code, such as `uthash.h` or `stb_image.h` perform relatively basic functions which we would have written ourselves, given more time.

All textures were made entirely by us.

All audio files were found on [pixabay.com](https://pixabay.com), and are free to use under their copyright rules. When using an audio file we have linked to the specific sound we used, although in some cases these were heavily modified to better fit our game. The copyright license can be found [here](https://pixabay.com/service/license-summary/).

#### Final Notes About the Project

Please note that this was originally coded on the Imperial GitLab, meaning that when moving the code onto GitHub we lost some information on merge requests or other small details. In commit messages we use our Imperial shortcodes to show who made it, as this is advised for internal marking purposes.

This code was also written entirely in 4 weeks, along with other parts of the coursework. Due to this time pressure, some parts of the codebase or architecture are not as clean as it could be. Currently there have been no major changes to our project post-submission.
