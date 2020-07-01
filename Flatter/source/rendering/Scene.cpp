#include "rendering/Scene.h"

#include "rendering/loaders/GLTFLoader.h"

using namespace Rendering;

Scene::Scene() {}

void Scene::add(const EntityRef& entity) {
  mEntities.push_back(entity);
}

void Scene::add(const std::vector<EntityRef>& entities) {
  mEntities.insert(mEntities.end(), entities.begin(), entities.end());
}

Scene::~Scene() {}