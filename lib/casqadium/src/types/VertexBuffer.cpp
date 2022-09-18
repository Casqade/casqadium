#include <cqde/types/VertexBuffer.hpp>


namespace cqde::types
{

bool
VertexBufferComparator::operator () (
  const VertexBuffer& lhs, const VertexBuffer& rhs ) const
{
  return lhs.depth > rhs.depth;
}

} // namespace cqde::types
