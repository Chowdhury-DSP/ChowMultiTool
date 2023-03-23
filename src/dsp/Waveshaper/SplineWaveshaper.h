#pragma once

#include <pch.h>

namespace dsp::waveshaper::spline
{
constexpr int numDrawPoints = 600;

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

using SplinePoints = std::array<juce::Point<float>, numDrawPoints>;
using Spline = std::array<SplineSection, numDrawPoints - 1>;
using SplineADAA = std::pair<Spline, std::array<SplineADAASection, numDrawPoints - 1>>;

SplinePoints getDefaultSplinePoints();
Spline createSpline (const SplinePoints& points);
double evaluateSpline (const Spline& spline, double x);

struct SplineState : chowdsp::StateValueBase
{
    explicit SplineState (std::string_view valueName);
    SplinePoints get() const noexcept { return currentValue; }
    operator SplinePoints() const noexcept { return get(); } // NOSONAR NOLINT(google-explicit-constructor): we want to be able to do implicit conversion

    /** Sets a new value */
    void set (const SplinePoints& v);
    SplineState& operator= (const SplinePoints& v);

    void reset() override { set (defaultValue); }

    void serialize (chowdsp::JSONSerializer::SerializedType& serial) const override;
    void deserialize (chowdsp::JSONSerializer::DeserializedType deserial) override;

    const SplinePoints defaultValue = getDefaultSplinePoints();

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
        SplinePoints val {};
        chowdsp::Serialization::deserialize<Serializer> (deserial, val);
        value.set (val);
    }

    SplinePoints currentValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SplineState)
};

constexpr chowdsp::WaveshaperPlotParams splineBounds {
    .xMin = -4.0f,
    .xMax = 4.0f,
    .yMin = -1.1f,
    .yMax = 1.1f,
};

class SplineWaveshaper
{
public:
    explicit SplineWaveshaper (SplineState& splineState);
    ~SplineWaveshaper();

    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    void processBlock (const chowdsp::BufferView<double>& buffer) noexcept;

private:
    SplineState& splineState;
    std::unique_ptr<SplineADAA> spline;

    std::vector<double> x1;

    chowdsp::FirstOrderHPF<double> dcBlocker;

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
} // namespace dsp::waveshaper::spline
