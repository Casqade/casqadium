#include <cqde/types/sequences/SequenceFactory.hpp>


namespace cqde::types
{

std::shared_ptr <types::SequenceStep>
SequenceFactory::create(
  const std::string& id ) const
{
  if ( mSequences.count(id) > 0 )
    return mSequences.at(id)();

  return {};
}

std::vector <identifier>
SequenceFactory::sequences() const
{
  std::vector <identifier> sequences {};

  for ( const auto& [id, creator] : mSequences )
    sequences.push_back(id);

  return sequences;
}

} // namespace cqde::types
