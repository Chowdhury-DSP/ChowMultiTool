#pragma once

#include <pch.h>

namespace dsp::waveshaper
{
struct SplineSection
{
    double a;
    double b;
    double c;
    double d;
    double x;
};

struct SplineADAASection
{
    double c0;
    double c1;
    double c2;
    double c3;
    double c4;
    double x;
};

PFR_FUNCTIONS_FOR (SplineSection);

using Spline = std::vector<SplineSection>;
using SplineADAA = std::pair<Spline, std::vector<SplineADAASection>>;

struct SplineState : chowdsp::StateValueBase
{
    SplineState (std::string_view valueName, Spline defaultVal);
    Spline get() const noexcept { return currentValue; }
    operator Spline() const noexcept { return get(); } // NOSONAR NOLINT(google-explicit-constructor): we want to be able to do implicit conversion

    /** Sets a new value */
    void set (Spline v);
    SplineState& operator= (Spline v);

    void reset() override { set (defaultValue); }

    void serialize (chowdsp::JSONSerializer::SerializedType& serial) const override;
    void deserialize (chowdsp::JSONSerializer::DeserializedType deserial) override;

    const Spline defaultValue;

private:
    template <typename Serializer>
    static void serialize (typename Serializer::SerializedType& serial, const SplineState& value)
    {
        Serializer::addChildElement (serial, value.name);
        Serializer::addChildElement (serial, chowdsp::Serialization::serialize<Serializer> (value.get()));
    }

    template <typename Serializer>
    static void deserialize (typename Serializer::DeserializedType deserial, SplineState& value)
    {
        Spline val {};
        chowdsp::Serialization::deserialize<Serializer> (deserial, val);
        value.set (val);
    }

    Spline currentValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SplineState)
};

constexpr chowdsp::WaveshaperPlotParams splineBounds {
    .xMin = -4.0f,
    .xMax = 4.0f,
    .yMin = -1.1f,
    .yMax = 1.1f,
};

Spline createSpline (std::vector<juce::Point<float>> points);
double evaluateSpline (const Spline& spline, double x);

class SplineWaveshaper : private juce::Timer
{
public:
    explicit SplineWaveshaper (SplineState& splineState);
    ~SplineWaveshaper() override;

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    void processBlock (const chowdsp::BufferView<double>& buffer) noexcept;

private:
    void timerCallback() override;

    SplineState& splineState;
    std::unique_ptr<SplineADAA> spline;

    std::vector<double> x1;

    chowdsp::SecondOrderHPF<double> dcBlocker;

    struct SplinePtr
    {
        void kill()
        {
            if (ptr != nullptr)
            {
                delete ptr;
                ptr = nullptr;
            }
        }

        SplineADAA* ptr = nullptr;
    };
    moodycamel::ReaderWriterQueue<SplinePtr, 4> uiToLiveQueue;
    moodycamel::ReaderWriterQueue<SplinePtr, 4> liveToDeadQueue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SplineWaveshaper)
};
} // namespace dsp::waveshaper
