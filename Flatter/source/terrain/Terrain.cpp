#include "terrain/Terrain.h"

Terrain::Terrain(const Rendering::Vulkan::ContextRef& context,
                 glm::uvec2 tileSubdivisions,
                 glm::vec2 tileSize)
    : mPerlinNoiseGenerator(5) {
  const unsigned int debugTileCount = 20;
  for (unsigned int xIndex = 0; xIndex < debugTileCount; ++xIndex) {
    for (unsigned int yIndex = 0; yIndex < debugTileCount; ++yIndex) {
      const glm::vec2 offset(
          (static_cast<int>(xIndex) - static_cast<int>(debugTileCount) / 2),
          (static_cast<int>(yIndex) - static_cast<int>(debugTileCount) / 2));
      TerrainTileRef tile = std::make_shared<TerrainTile>(
          context, mPerlinNoiseGenerator, tileSubdivisions,
          glm::vec2(static_cast<float>(xIndex), static_cast<float>(yIndex)));
      tile->getTransform()->setPosition(
          glm::vec3(tileSize.x * offset.x, 0.0f, tileSize.y * offset.y));
      tile->getTransform()->setScale(glm::vec3(tileSize.x, 1.0f, tileSize.y));
      mTiles.push_back(tile);
    }
  }
}

void Terrain::draw(const Rendering::Camera& camera,
                   const Rendering::Vulkan::PipelineRef& pipeline,
                   const vk::CommandBuffer& commandBuffer) {
  for (const auto& tile : mTiles) {
    tile->draw(camera, pipeline, commandBuffer);
  }
}

Terrain::~Terrain() {}