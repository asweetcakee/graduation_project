# Graduation Project: DIY Modular Joystick with Hall Sensors

This repository contains the materials and resources related to my graduation project: the development of a modular DIY joystick designed for flight simulators, focusing on personalization and immersive gameplay. 
The project integrates 3D printing, CAD modeling, ESP-WROOM-32 / Arduino Pro Micro programming, and PCB development using KiCad. 
The modularity allows for future enhancements, positioning this joystick as an alternative for DIY enthusiasts in the flight simulation community.

## Project Overview

The goal of this project was to create a customizable joystick for flight simulators, emphasizing durability and precision using Hall sensors. 
The joystick is designed with modular components, enabling the user to upgrade and modify the design to suit individual needs.

### Key Features:
- **3D-printed components** designed in **Fusion 360**
- **PCB design** using **KiCad**
- **ESP-WROOM-32 / Arduino Pro micro** microcontroller programming with 12-bit or 10-bit ADC for high precision
- **Hall effect sensors** (SS495A) for enhanced accuracy
- Custom-built **dual-stage trigger** and **yaw axis** for improved realism

## Repository Content

1. **Step Models**: 3D models of main components (STEP files)
2. **StickControl**: Code for calibrating and controlling the joystick via ESP-WROOM-32 / Arduino Pro Micro
3. **Content-images-vids-gifs**: Visuals of the joystick, including a video demonstrating its testing process

### Top part enclosure:
<div style="display: flex; flex-wrap: wrap;">
  <img src="Content-images-vids-gifs/screenshot-top-enclosure.png" alt="Top enclosure" width="400" height="auto" style="margin: 5px;">
  <img src="Content-images-vids-gifs/screenshot-main-enclosure.png" alt="Main enclosure" width="300" height="auto" style="margin: 5px;">
</div>

### Grip design:
<img src="Content-images-vids-gifs/screenshot-grip.png" alt="Yaw axis control mechanism 1" width="300" height="auto">

### Yaw axis control mechanism:
<div style="display: flex; flex-wrap: wrap;">
  <img src="Content-images-vids-gifs/screenshot-yaw-axis.png" alt="Yaw axis control mechanism 1" width="300" height="auto" style="margin: 5px;">
  <img src="Content-images-vids-gifs/screenshot-yaw-axis-2.png" alt="Yaw axis control mechanism 2" width="300" height="auto" style="margin: 5px;">
  <img src="Content-images-vids-gifs/screenshot-yaw-axis-3.png" alt="Yaw axis control mechanism 3" width="300" height="auto" style="margin: 5px;">
  <img src="Content-images-vids-gifs/real-yaw-axis.png" alt="Yaw axis control mechanism 4" width="500" height="auto" style="margin: 5px;">
  <img src="Content-images-vids-gifs/hiw-yaw-axis.gif" alt="Yaw Axis Mechanism in Action" width="500" height="auto" style="margin: 5px;">
</div>

### Gimbal mechanism:
<div style="display: flex; flex-wrap: wrap;">
  <img src="Content-images-vids-gifs/screenshot-gimbal-1.png" alt="Gimbal mechanism 1" width="400" height="auto" style="margin: 5px;">
  <img src="Content-images-vids-gifs/screenshot-gimbal-2.png" alt="Gimbal mechanism 2" width="400" height="auto" style="margin: 5px;">
  <img src="Content-images-vids-gifs/real-gimbal.png" alt="Gimbal mechanism 3" width="300" height="auto" style="margin: 5px;">
</div>

### Lead screw mechanism:
<div style="display: flex; flex-wrap: wrap;">
  <img src="Content-images-vids-gifs/screenshot-lead-screw-1.png" alt="Lead screw 1" width="400" height="auto" style="margin: 5px;">
  <img src="Content-images-vids-gifs/screenshot-lead-screw-2.png" alt="Lead screw 2" width="400" height="auto" style="margin: 5px;">
  <img src="Content-images-vids-gifs/hiw-lead-screw.gif" alt="Lead screw in Action" width="300" height="auto" style="margin: 5px;">
</div>

### PCB design:
<div style="display: flex; flex-wrap: wrap;">
  <img src="Content-images-vids-gifs/screenshot-pcb-1.png" alt="PCB design" width="400" height="auto" style="margin: 5px;">
  <img src="Content-images-vids-gifs/screenshot-pcb-2.png" alt="PCB design paths" width="400" height="auto" style="margin: 5px;">
</div>

### Testing process on War Thunder game:
<div style="display: flex; flex-wrap: wrap; gap: 10px;">
  <video width="400" controls>
    <source src="Content-images-vids-gifs/test-stick.mp4" type="video/mp4">
    Your browser does not support the video tag.
  </video>
</div>

