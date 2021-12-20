#ifndef GRAPHICS_3D_SCENE_NODE_HPP
#define GRAPHICS_3D_SCENE_NODE_HPP

#include <Graphics3D/Transformable.hpp>

#include <set>
#include <memory>


namespace Graphics3D
{

class SceneNode : public Transformable
{
protected:
  const SceneNode* mParent;
  std::set <std::shared_ptr <SceneNode>> mChildren;

  void setParent( const SceneNode* );
  virtual std::shared_ptr <SceneNode> clone();

public:
  SceneNode( const SceneNode* parent = {} );
  SceneNode( SceneNode& );
  virtual ~SceneNode();

  void addChild( const std::shared_ptr <SceneNode> );
  void removeChild( const std::shared_ptr <SceneNode> );

  void clearChildren();
  void transferChildren( SceneNode& );

//  std::shared_ptr <SceneNode> extractChild();

  glm::mat4 modelWorld() const;
};

} // namespace Graphics3D


#endif
