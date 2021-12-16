#ifndef GRAPHICS_3D_SCENE_NODE_HPP
#define GRAPHICS_3D_SCENE_NODE_HPP

#include <Graphics3D/Transformable.hpp>

#include <set>
#include <memory>


namespace Graphics3D
{

class SceneNode : public Transformable
{
  std::shared_ptr <SceneNode> mParent;
  std::set <std::shared_ptr <SceneNode>> mChildren;

public:
  SceneNode( const std::shared_ptr <SceneNode> parent = {} );
  SceneNode( SceneNode& );
  ~SceneNode();

  void setParent( const std::shared_ptr <SceneNode> );

  void addChild( const std::shared_ptr <SceneNode> );
  void removeChild( const std::shared_ptr <SceneNode> );

  void clearChildren();
  void transferChildren( SceneNode& );

  std::shared_ptr <SceneNode> extractChild();

  glm::mat4 model() const override;
};

} // namespace Graphics3D


#endif
