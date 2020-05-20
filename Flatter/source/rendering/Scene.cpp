#include "rendering/Scene.h"

#include "rendering/loaders/GLTFLoader.h"

using namespace Rendering;

Scene::Scene() {}

void Scene::add(const MeshRef mesh) {
  mMeshes.push_back(mesh);
}

Scene::~Scene() {}