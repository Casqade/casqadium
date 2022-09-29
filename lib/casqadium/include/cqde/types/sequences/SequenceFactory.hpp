#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/sequences/SequenceStep.hpp>

#include <memory>
#include <string>
#include <unordered_map>


namespace cqde::types
{

class SequenceFactory
{
  using InstanceGetter = std::function <std::shared_ptr <types::SequenceStep> ()>;

  std::unordered_map <SequenceId, InstanceGetter,
                      identifier_hash> mSequences;

public:
  void sequenceRegister( const SequenceId&,
                         const InstanceGetter& );

  std::shared_ptr <types::SequenceStep> get( const SequenceId& ) const;

  std::vector <SequenceId> sequences() const;
};

} // namespace cqde::types
