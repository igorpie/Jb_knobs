#include <JuceHeader.h>
#include "Editor.h"
#include "Processor.h"

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TutorialProcessor();
}
