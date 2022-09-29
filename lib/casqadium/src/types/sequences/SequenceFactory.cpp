#include <cqde/types/sequences/SequenceFactory.hpp>


namespace cqde::types
{

void
SequenceFactory::sequenceRegister(
  const SequenceId& id,
  const InstanceGetter& getter )
{
  mSequences[id] = getter;
}

std::shared_ptr <types::SequenceStep>
SequenceFactory::get(
  const SequenceId& id ) const
{
  if ( mSequences.count(id) > 0 )
    return mSequences.at(id)();

  return {};
}

std::vector <SequenceId>
SequenceFactory::sequences() const
{
  std::vector <SequenceId> sequences {};

  for ( const auto& [id, getter] : mSequences )
    sequences.push_back(id);

  return sequences;
}

} // namespace cqde::types
