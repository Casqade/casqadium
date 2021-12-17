#include <Graphics3D/SceneNode.hpp>


namespace Graphics3D
{

SceneNode::SceneNode( const SceneNode* parent )
  : mParent(parent)
{}

SceneNode::SceneNode( SceneNode& node )
{
  for ( const auto& child : mChildren )
    node.addChild( child->clone() );
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
SceneNode::addChild( const std::shared_ptr <SceneNode> node )
{
  node->setParent( this );
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
    node.addChild( child );

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
