#include "terrain/TerrainTile.h"

using namespace Rendering;

TerrainTile::TerrainTile(
    const Rendering::Vulkan::ContextRef& context,
    const Flatter::PerlinNoiseGenerator<float>& noiseGenerator,
    const glm::uvec2 tileSubdivisions,
    const glm::vec2 offset)
    : Entity() {
  std::pair<std::vector<glm::vec3>, std::vector<uint16_t>> geometryInfo =
      generateGeometry(noiseGenerator, tileSubdivisions, offset);
  mGeometry = std::make_shared<Rendering::Vulkan::IndexedVertexBuffer>(
      context, geometryInfo.first, geometryInfo.second);
}

std::pair<std::vector<glm::vec3>, std::vector<uint16_t>>
TerrainTile::generateGeometry(
    const Flatter::PerlinNoiseGenerator<float>& noiseGenerator,
    const glm::uvec2 tileSubdivisions,
    const glm::vec2 offset) {
  const float incrementX = 1.0f / static_cast<float>(tileSubdivisions.x);
  const float incrementY = 1.0f / static_cast<float>(tileSubdivisions.y);

  std::vector<glm::vec3> vertexBuffer;
  std::vector<uint16_t> indexBuffer;
  vertexBuffer.reserve(static_cast<size_t>(tileSubdivisions.x) *
                       static_cast<size_t>(tileSubdivisions.y));
  indexBuffer.reserve(3 * 2 * static_cast<size_t>(tileSubdivisions.x) *
                      static_cast<size_t>(tileSubdivisions.y));

  for (unsigned int yIndex = 0; yIndex <= tileSubdivisions.y; ++yIndex) {
    for (unsigned int xIndex = 0; xIndex <= tileSubdivisions.x; ++xIndex) {
      const glm::vec2 normalizedCoords(
          xIndex / static_cast<float>(tileSubdivisions.x),
          yIndex / static_cast<float>(tileSubdivisions.y));
      vertexBuffer.emplace_back(
          normalizedCoords.x - 0.5f,
          noiseGenerator.getNoise((normalizedCoords.x + offset.x),
                                  (normalizedCoords.y + offset.y)),
          normalizedCoords.y - 0.5f);
    }
  }

  for (unsigned int yIndex = 0; yIndex < tileSubdivisions.y; ++yIndex) {
    for (unsigned int xIndex = 0; xIndex < tileSubdivisions.x; ++xIndex) {
      const uint16_t v0 = xIndex * (tileSubdivisions.y + 1) + yIndex;
      const uint16_t v1 = xIndex * (tileSubdivisions.y + 1) + (yIndex + 1);
      const uint16_t v2 = (xIndex + 1) * (tileSubdivisions.y + 1) + yIndex;
      const uint16_t v3 =
          (xIndex + 1) * (tileSubdivisions.y + 1) + (yIndex + 1);

      indexBuffer.emplace_back(v0);
      indexBuffer.emplace_back(v2);
      indexBuffer.emplace_back(v1);

      indexBuffer.emplace_back(v1);
      indexBuffer.emplace_back(v2);
      indexBuffer.emplace_back(v3);
    }
  }
  return std::pair<std::vector<glm::vec3>, std::vector<uint16_t>>(vertexBuffer,
                                                                  indexBuffer);
}

TerrainTile::~TerrainTile() {}