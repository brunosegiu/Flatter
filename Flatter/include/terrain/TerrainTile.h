#pragma once

#include <memory>
#include <vector>

#include "Entity.h"
#include "commons/random/PerlinNoiseGenerator.h"

class TerrainTile : public Entity {
 public:
  TerrainTile(const Rendering::Vulkan::ContextRef& context,
              const Flatter::PerlinNoiseGenerator<float>& noiseGenerator,
              const glm::uvec2 tileSubdivisions,
              const glm::vec2 offset);

  virtual ~TerrainTile();

 private:
  TerrainTile(TerrainTile const&) = delete;
  TerrainTile& operator=(TerrainTile const&) = delete;

  std::pair<std::vector<glm::vec3>, std::vector<uint16_t>> generateGeometry(
      const Flatter::PerlinNoiseGenerator<float>& noiseGenerator,
      const glm::uvec2 tileSubdivisions,
      const glm::vec2 offset);
};

using TerrainTileRef = std::shared_ptr<TerrainTile>;
