#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/sequences/SequenceStep.hpp>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>


namespace cqde::types
{

class SequenceFactory
{
  using SequenceCreator = std::function <std::shared_ptr <SequenceStep> ()>;

  std::unordered_map <std::string, SequenceCreator> mSequences {};

public:
  SequenceFactory() = default;

  template <class T>
  void registerSequence( const std::string& id );

  std::shared_ptr <SequenceStep> create( const std::string& id ) const;

  std::vector <identifier> sequences() const;
};

template <class T>
void
SequenceFactory::registerSequence(
  const std::string& id )
{
  mSequences[id] = std::make_shared <T>;
}

} // namespace cqde::types
