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

glm::mat4
SceneNode::modelWorld() const
{
  if ( mParent )
    return mParent->modelWorld() * modelLocal();

  return modelLocal();
}

} // namespace Graphics3D
