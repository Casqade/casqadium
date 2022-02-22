#ifndef GRAPHICS_3D_SCENE_NODE_HPP
#define GRAPHICS_3D_SCENE_NODE_HPP

#include <Graphics3D/Transformable.hpp>

#include <set>
#include <map>
#include <memory>


namespace Graphics3D
{

class Camera;

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

  virtual void appendCulled(  std::multimap < float, SceneNode*, std::greater <float>>& depthBuffer,
                              const Camera* );

  virtual void draw();

  void setOriginWorld( const glm::vec3& );
  void setOrientationWorld( const glm::quat& );
  void setScaleWorld( const glm::vec3& );

  glm::vec3 toLocalSpace( const glm::vec3& ) const;
  glm::quat toLocalSpace( const glm::quat& ) const;

  glm::vec3 toWorldSpace( const glm::vec3& ) const;
  glm::quat toWorldSpace( const glm::quat& ) const;

  glm::mat4 modelWorld() const;
};

} // namespace Graphics3D


#endif
