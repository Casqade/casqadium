#include <Graphics3D/SceneNode.hpp>


namespace Graphics3D
{

SceneNode::SceneNode( const SceneNode* parent )
  : Transformable()
  , mParent(parent)
  , mChildren()
{}

SceneNode::SceneNode( SceneNode& node )
  : Transformable(node.mOrigin, node.mOrientation, node.mScale)
  , mParent(node.mParent)
{
  for ( const auto& child : node.mChildren )
    addChild( child->clone() );
}

SceneNode::~SceneNode()
{
  for ( auto& child : mChildren )
    child->setParent( nullptr );

  clearChildren();
}

std::shared_ptr <SceneNode>
SceneNode::clone()
{
  return std::make_shared <SceneNode> (*this);
}

void
SceneNode::setParent( const SceneNode* parent )
{
  mParent = parent;
}

void
SceneNode::addChild( const std::shared_ptr <SceneNode> child )
{
  child->setParent( this );
  mChildren.insert( child );
}

void
SceneNode::removeChild( const std::shared_ptr <SceneNode> child )
{
  mChildren.erase( child );
}

void
SceneNode::clearChildren()
{
  for ( auto& child : mChildren )
    child->clearChildren();

  mChildren.clear();
}

void
SceneNode::transferChildren( SceneNode& node )
{
  for ( auto& child : mChildren )
    node.addChild( child );

  mChildren.clear();
}

void
SceneNode::appendCulled(  std::multimap < float, SceneNode*, std::greater <float>>& depthBuffer,
                          const Graphics3D::Camera* camera )
{
  for ( auto& child : mChildren )
    child->appendCulled( depthBuffer, camera );
}

void
SceneNode::draw()
{
  return;
}

void
SceneNode::setOriginWorld( const glm::vec3& origin )
{
  mOrigin = toLocalSpace(origin);
}

void
SceneNode::setOrientationWorld( const glm::quat& orientation )
{
  mOrientation = toLocalSpace(orientation);
}

void
SceneNode::setScaleWorld( const glm::vec3& scale )
{
  mScale = (modelLocal() * glm::inverse(modelWorld()) * glm::translate(glm::mat4(1.0f), scale))[3];
}

glm::vec3
SceneNode::toLocalSpace( const glm::vec3& point ) const
{
  return glm::translate( modelLocal() * glm::inverse(modelWorld()), point )[3];
}

glm::quat
SceneNode::toLocalSpace( const glm::quat& orientation ) const
{
  return glm::normalize( glm::quat(modelLocal() * glm::inverse(modelWorld())) * orientation );
}

glm::vec3
SceneNode::toWorldSpace( const glm::vec3& point ) const
{
  return glm::translate( modelWorld() * glm::inverse(modelLocal()), point )[3];
}

glm::quat
SceneNode::toWorldSpace( const glm::quat& orientation ) const
{
  return glm::normalize( glm::quat(modelWorld() * glm::inverse(modelLocal())) * orientation );
}

glm::mat4
SceneNode::modelWorld() const
{
  if ( mParent )
    return mParent->modelWorld() * modelLocal();

  return modelLocal();
}

} // namespace Graphics3D
