// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

/** @file
 * @brief enum @ref esp::assets::SupportedMeshType, Class @ref
 * esp::assets::BaseMesh
 */

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Reference.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Range.h>
#include <Magnum/Mesh.h>
#include <Magnum/Trade/MeshData3D.h>
#include "CollisionMeshData.h"
#include "MeshData.h"
#include "esp/core/esp.h"
#include "esp/gfx/magnum.h"

namespace esp {
namespace assets {

/**
  @brief Enumeration of mesh types supported by the simulator. Each entry
  suggests a derived class of @ref BaseMesh implementing the specific storage
  and processing interface to accomodate differing asset formats. Identifies the
  derived variant of this object.
  */
enum SupportedMeshType {
  /**
   * @brief Undefined mesh types are created programmatically without a specific
   * format or loaded from an unkown format. Support for this type and behavior
   * is likely limited. Object type is likely @ref BaseMesh.
   */
  NOT_DEFINED = -1,

  /**
   * @brief Instance meshes loaded from sources including segmented object
   * identifier data (e.g. semantic data: chair, table, etc...). Sources include
   * .ply files and reconstructions of Matterport scans. Object is likely of
   * type @ref GenericInstanceMeshData or Mp3dInstanceMeshData.
   */
  INSTANCE_MESH = 0,

  /**
   * @brief Meshes loaded from Replica dataset. Object is likely type @ref
   * PTexMeshData.
   */
  PTEX_MESH = 1,

  /**
   * @brief Meshes loaded from gltf format (i.e. .glb file). Object is likely
   * type @ref GltfMeshData.
   */
  GLTF_MESH = 2,

  /**
   * @brief Number of enumerated supported types.
   */
  NUM_SUPPORTED_MESH_TYPES = 3,
};

/**
 * @brief Base class for storing mesh asset data including geometry and
 * topology. Also manages transfer of this data to GPU memory. Derived classes
 * implement modifications to support specific mesh formats as enumerated by
 * @ref SupportedMeshType.
 */
class BaseMesh {
 public:
  /** @brief Constructor defining the @ref SupportedMeshType of this asset and
   * likely identifying the derived type of this object.*/
  explicit BaseMesh(SupportedMeshType type) : type_(type){};

  /** @brief Destructor */
  virtual ~BaseMesh(){};

  /**
   * @brief Set the @ref SupportedMeshType, @ref type_, of this object.
   * @param type The desired @ref SupportedMeshType to set.
   * @return Whether or not the set was successful.
   */
  bool setMeshType(SupportedMeshType type);

  /**
   * @brief Retrieve the @ref SupportedMeshType, @ref type_, of this object.
   * @return the @ref SupportedMeshType, @ref type_, of this object.
   */
  SupportedMeshType getMeshType() { return type_; }

  /**
   * @brief Upload the mesh data to GPU memory. Virual with no implmentation for
   * @ref BaseMesh.
   */
  virtual void uploadBuffersToGPU(bool){};

  /**
   * @brief Get a pointer to the compiled rendering buffer for the asset. Always
   * nullptr for @ref BaseMesh.
   * @return A pointer to the compiled rendering buffer for the asset.
   */
  virtual Magnum::GL::Mesh* getMagnumGLMesh() { return nullptr; }

  /**
   * @brief Get a pointer to the compiled rendering buffer for a particular
   * sub-component of the asset. Always nullptr for @ref BaseMesh.
   * @return A pointer to the compiled rendering buffer for a particular
   * sub-component of the asset.
   */
  virtual Magnum::GL::Mesh* getMagnumGLMesh(int) { return nullptr; }

  /**
   * @brief Get a reference to the @ref CollisionMeshData (non-render geometry
   * and topology) for the asset. Usage: (1) physics simulation.
   * @return The @ref CollisionMeshData, @ref collisionMeshData_.
   */
  virtual CollisionMeshData& getCollisionMeshData() {
    return collisionMeshData_;
  }

  /**
   * @brief Any transformations applied to the original mesh after loading are
   * stored here. See @ref ResourceManager::translateMesh.
   */
  Magnum::Matrix4 meshTransform_;

  /**
   * @brief Axis aligned bounding box of the mesh. Computed automatically on
   * mesh load. See @ref ResourceManager::computeMeshBB.
   */
  Magnum::Range3D BB;

 protected:
  /**
   * @brief The @ref SupportedMeshType of this asset. Identifies the derived
   * type of this object and the format of the asset.
   */
  SupportedMeshType type_ = SupportedMeshType::NOT_DEFINED;

  /**
   * @brief Whether or not the mesh data has been transfered to GPU.
   */
  bool buffersOnGPU_ = false;

  // ==== rendering ===
  /**
   * @brief Optional storage container for mesh render data. See @ref
   * GltfMeshData::setMeshData.
   */
  Corrade::Containers::Optional<Magnum::Trade::MeshData3D> meshData_;

  // ==== non-rendering ===
  /**
   * @brief Stores references to mesh geometry and topology for use in CPU side
   * physics collision shape generation. Should be updated when mesh data is
   * edited.
   */
  CollisionMeshData collisionMeshData_;
};
}  // namespace assets
}  // namespace esp
