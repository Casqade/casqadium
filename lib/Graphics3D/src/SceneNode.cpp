#include <Graphics3D/SceneNode.hpp>


namespace Graphics3D
{

SceneNode::SceneNode( const std::shared_ptr <SceneNode> parent )
  : mParent(parent)
{}

SceneNode::SceneNode( SceneNode& node )
{
  for ( auto& child : mChildren )
  {
    node.mChildren.insert( child );
  }
}

SceneNode::~SceneNode()
{
  for ( auto& child : mChildren )
    child->setParent( nullptr );

  clearChildren();
}

void
SceneNode::setParent( const std::shared_ptr <SceneNode> parent )
{
  mParent = parent;
}

void
SceneNode::addChild( const std::shared_ptr <SceneNode> node )
{
  mChildren.insert( node );
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
    child->clearChildren();

  mChildren.clear();
}

glm::mat4
SceneNode::model() const
{
  if ( mParent )
    return mParent->model() * Transformable::model();

  return Transformable::model();
}

} // namespace Graphics3D
