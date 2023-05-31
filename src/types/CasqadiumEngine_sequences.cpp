#include <cqde/types/CasqadiumEngine.hpp>

#include <cqde/types/sequences/SequenceFactory.hpp>
#include <cqde/types/sequences/CallbackExecute.hpp>
#include <cqde/types/sequences/CallbackExecuteAPeriodic.hpp>
#include <cqde/types/sequences/CallbackExecutePeriodic.hpp>
#include <cqde/types/sequences/CameraFovInterpolated.hpp>
#include <cqde/types/sequences/Delay.hpp>
#include <cqde/types/sequences/RandomDelay.hpp>
#include <cqde/types/sequences/TextureTintInterpolated.hpp>
#include <cqde/types/sequences/TransformInterpolated.hpp>
#include <cqde/types/sequences/TransformManipulate.hpp>


namespace cqde::types
{

void
CasqadiumEngine::registerSequenceSteps()
{
  auto& sequenceFactory = mRegistry.ctx().emplace <SequenceFactory> ();

  sequenceFactory.registerSequence <CameraFovInterpolated> ("CameraFovInterpolated");
  sequenceFactory.registerSequence <Delay> ("Delay");
  sequenceFactory.registerSequence <RandomDelay> ("RandomDelay");
  sequenceFactory.registerSequence <CallbackExecute> ("CallbackExecute");
  sequenceFactory.registerSequence <CallbackExecuteAPeriodic> ("CallbackExecuteAPeriodic");
  sequenceFactory.registerSequence <CallbackExecutePeriodic> ("CallbackExecutePeriodic");
  sequenceFactory.registerSequence <TextureTintInterpolated> ("TextureTintInterpolated");
  sequenceFactory.registerSequence <TransformInterpolated> ("TransformInterpolated");
  sequenceFactory.registerSequence <TransformManipulate> ("TransformManipulate");
}

} // namespace cqde::types
