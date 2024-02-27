#include <PluginProcessor.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "helpers/test_helpers.h"

TEST_CASE("PluginProcessor", "[instance]") {
    PluginProcessor testPlugin;

    // This lets us use JUCE's MessageManager without leaking.
    // PluginProcessor might need this if you use the APVTS for example.
    // You'll also need it for tests that rely on juce::Graphics, juce::Timer, etc.
    auto gui = juce::ScopedJuceInitialiser_GUI{};

    SECTION("getName() returns expected name") {
        CHECK_THAT(testPlugin.getName().toStdString(),
                   Catch::Matchers::Equals("WabiSonorance Synthesizer"));
    }
}

#ifdef PAMPLEJUCE_IPP
#include <ipp.h>

TEST_CASE("IPP version", "[ipp]") {
    CHECK_THAT(ippsGetLibVersion()->Version,
               Catch::Matchers::Equals("2021.10.1 (r0x8e799c51)"));
}
#endif
