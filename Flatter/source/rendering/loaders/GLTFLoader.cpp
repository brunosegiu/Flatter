﻿#include "rendering/loaders/GLTFLoader.h"

#include "nlohmann/json.hpp"
#include "tiny_gltf.h"

using namespace Rendering;

GLTFLoader::GLTFLoader(const Rendering::Vulkan::ContextRef& context)
    : mContext(context) {}

EntityRef processPrimitive(const Rendering::Vulkan ::ContextRef context,
                           const tinygltf::Model& model,
                           const tinygltf::Mesh& mesh,
                           const tinygltf::Primitive& primitive) {
  const std::string positionName = "POSITION";
  const tinygltf::Accessor& positionAccessor =
      model.accessors[primitive.attributes.at(positionName)];

  std::vector<glm::vec3> positions(positionAccessor.count);
  {
    const tinygltf::BufferView& positionBufferView =
        model.bufferViews[positionAccessor.bufferView];
    const tinygltf::Buffer& positionBuffer =
        model.buffers[positionBufferView.buffer];
    const size_t positionBufferOffset =
        positionBufferView.byteOffset + positionAccessor.byteOffset;
    size_t vetexStride = positionAccessor.ByteStride(positionBufferView);
    const unsigned char* positionData =
        &positionBuffer.data[positionBufferOffset];

    const unsigned int positionCount =
        static_cast<unsigned int>(positionAccessor.count);
    for (unsigned int positionIndex = 0; positionIndex < positionCount;
         ++positionIndex) {
      const float* positionDataFloat = reinterpret_cast<const float*>(
          &positionData[vetexStride * positionIndex]);
      positions[positionIndex] = glm::vec3(
          positionDataFloat[0], positionDataFloat[1], positionDataFloat[2]);
    }
  }

  const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
  std::vector<uint16_t> indices;
  {
    const unsigned int indexCount =
        static_cast<unsigned int>(indexAccessor.count);
    const tinygltf::BufferView& indexBufferView =
        model.bufferViews[indexAccessor.bufferView];
    const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
    const size_t indexOffset =
        indexBufferView.byteOffset + indexAccessor.byteOffset;
    const uint16_t* pIndexData =
        reinterpret_cast<const uint16_t*>(&indexBuffer.data[indexOffset]);
    indices = std::vector<uint16_t>(pIndexData, pIndexData + indexCount);
  }

  const Rendering::Vulkan::IndexedVertexBufferRef geometry =
      std::make_shared<Rendering::Vulkan::IndexedVertexBuffer>(
          context, positions, indices);
  const TransformRef transform = std::make_shared<Transform>();
  return std::make_shared<Entity>(transform, geometry);
}

std::vector<EntityRef> GLTFLoader::load(std::string path) {
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  std::vector<EntityRef> entities;
  entities.reserve(model.meshes.size());

  if (loader.LoadBinaryFromFile(&model, &err, &warn, path)) {
    for (const auto& mesh : model.meshes) {
      for (const auto& primitive : mesh.primitives) {
        entities.push_back(processPrimitive(mContext, model, mesh, primitive));
      }
    }
  }
  return entities;
}