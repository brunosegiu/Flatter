# Flatter

A C++ Vulkan sanbox built with the intent of learning Vulkan and some advanced rendering techiques.

<p align="center">
  <img width="500vw" height="auto" src="docs/images/terrain.gif">
</p>

## Rendering

<p align="center">
  <img width="500vw" height="auto" src="docs/images/day.gif">
</p>

A GBuffer consisting of worldspace positions, albedo and normals is stored in three independant framebuffer attachments and read at the next step. Lights information is sent through a uniform buffer, while transforms are recorded into de command buffer as push constants, lights are capped at a maximum, determined at runtime as set by a specialization constant.

## Data management

Models and static objects are loaded using the [GLTF](https://www.khronos.org/gltf/) format standard, while other entities are created through a patented™ ECS (not really). Terrain rendering is a whole ball of wax since it interleaves higher and lower level concepts, since this is a personal project interfaces aren't really definded.

<p align="center">
  <img width="500vw" height="auto" src="docs/images/suzanne.gif">
</p>