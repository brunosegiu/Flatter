#pragma once

#include <memory>
#include <vector>

#include "Entity.h"
#include "commons/random/PerlinNoiseGenerator.h"
#include "terrain/TerrainTile.h"

class Terrain : public Entity {
 public:
  Terrain(const Rendering::Vulkan::ContextRef& context,
          glm::uvec2 tileSubdivisions,
          glm::vec2 tileSize);

  void draw(const Rendering::Camera& camera,
            const Rendering::Vulkan::PipelineRef& pipeline,
            const vk::CommandBuffer& commandBuffer) override;

  virtual ~Terrain();

 private:
  Terrain(Terrain const&) = delete;
  Terrain& operator=(Terrain const&) = delete;

  std::vector<TerrainTileRef> mTiles;
  Flatter::PerlinNoiseGenerator<float> mPerlinNoiseGenerator;
};

using TerrainRef = std::shared_ptr<Terrain>;
