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
  detachChildren();
}

void
SceneNode::setParent( const SceneNode* parent )
{
  mParent = parent;
}


std::shared_ptr <SceneNode>
SceneNode::clone()
{
  return std::make_shared <SceneNode> (*this);
}

SceneNode*
SceneNode::parent() const
{
  return const_cast <SceneNode*> (mParent);
}

void
SceneNode::addChild( const std::shared_ptr <SceneNode> child )
{
  if ( child == nullptr )
    return;

  child->setParent( this );
  mChildren.insert( child );
}

void
SceneNode::addChildren( const std::set <std::shared_ptr <SceneNode>> children )
{
  for ( const auto& child : children )
    addChild(child);
}

void
SceneNode::deleteChildren()
{
  for ( auto& child : mChildren )
  {
    child->setParent();
    child->deleteChildren();
  }

  mChildren.clear();
}

void
SceneNode::deleteChild( const std::shared_ptr <SceneNode> child )
{
  detachChild( child );
}

std::set <std::shared_ptr <SceneNode>>
SceneNode::detachChildren()
{
  const auto children = mChildren;
  mChildren.clear();

  for ( auto& child : children )
    child->setParent();

  return children;
}

std::shared_ptr <SceneNode>
SceneNode::detachChild( const std::shared_ptr <SceneNode> child )
{
  if ( mChildren.find(child) == mChildren.end() )
    return nullptr;

  child->setParent();
  mChildren.erase( child );

  return child;
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

glm::mat4
SceneNode::toLocalSpace( const glm::mat4& mat ) const
{
  return modelLocal() * glm::inverse(modelWorld()) * mat;
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
SceneNode::toWorldSpace( const glm::mat4& mat ) const
{
  return modelWorld() * glm::inverse(modelLocal()) * mat;
}

glm::mat4
SceneNode::modelWorld() const
{
  if ( mParent )
    return mParent->modelWorld() * modelLocal();

  return modelLocal();
}

} // namespace Graphics3D
