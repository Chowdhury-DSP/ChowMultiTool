#pragma once

#include <pch.h>

namespace dsp::waveshaper::spline
{
constexpr chowdsp::WaveshaperPlotParams splineBounds {
    .xMin = -4.0f,
    .xMax = 4.0f,
    .yMin = -1.1f,
    .yMax = 1.1f,
};

constexpr int maxNumDrawPoints = 600;
template <typename T = double>
constexpr auto getSplineScalerAndOffset()
{
    constexpr auto scaler = T (maxNumDrawPoints - 1) / T (splineBounds.xMax - splineBounds.xMin);
    constexpr auto offset = (T) -splineBounds.xMin * scaler;
    return std::make_pair (scaler, offset);
}

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

template <typename Spline>
Spline createSpline (std::span<const juce::Point<float>> points,
                     std::span<double> h,
                     std::span<double> alpha,
                     std::span<double> l,
                     std::span<double> mu,
                     std::span<double> z)
{
    const auto numDrawnPoints = points.size();
    const auto n = size_t (numDrawnPoints - 1);

    jassert (points.size() == numDrawnPoints);
    jassert (h.size() == numDrawnPoints);
    jassert (alpha.size() == numDrawnPoints);
    jassert (l.size() == n);
    jassert (mu.size() == n);
    jassert (z.size() == n);

    Spline set;
    if constexpr (std::is_same_v<Spline, std::vector<SplineSection>>)
    {
        set.resize (n, {});
    }

    for (size_t i = 0; i < n; ++i)
        h[i] = double (points[i + 1].x - points[i].x);

    for (size_t i = 1; i < n; ++i)
        alpha[i] = double (3 * (points[i + 1].y - points[i].y) / h[i] - 3 * (points[i].y - points[i - 1].y) / h[i - 1]);

    l[0] = 1.0;
    mu[0] = 0.0;
    z[0] = 0.0;

    for (size_t i = 1; i < n; ++i)
    {
        l[i] = 2 * double (points[i + 1].x - points[i - 1].x) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }

    l[n] = 1.0;
    z[n] = 0.0;
    double prevC = 0.0;

    JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wshorten-64-to-32", "-Wsign-conversion")
    for (int j = n - 1; j >= 0; --j) // NOLINT
    {
        set[j].a = (double) points[j].y;
        set[j].c = z[j] - mu[j] * prevC;
        set[j].b = ((double) points[j + 1].y - set[j].a) / h[j] - h[j] * (prevC + 2 * set[j].c) / 3;
        set[j].d = (prevC - set[j].c) / 3 / h[j];
        set[j].x = (double) points[j].x;

        prevC = set[j].c;
    }
    JUCE_END_IGNORE_WARNINGS_GCC_LIKE

    return set;
}

template <typename PointsContainer>
struct SplinePointsState : chowdsp::StateValueBase
{
    template <typename DefaultPointsCreator>
    explicit SplinePointsState (std::string_view valueName, DefaultPointsCreator)
        : StateValueBase (valueName),
          defaultValue (DefaultPointsCreator::call()),
          currentValue (defaultValue)
    {
    }
    PointsContainer get() const noexcept { return currentValue; }
    operator PointsContainer() const noexcept { return get(); } // NOSONAR NOLINT(google-explicit-constructor): we want to be able to do implicit conversion

    /** Sets a new value */
    void set (const PointsContainer& v)
    {
        if (v == currentValue)
            return;

        currentValue = v;
        changeBroadcaster();
    }
    SplinePointsState& operator= (const PointsContainer& v)
    {
        set (v);
        return *this;
    }

    void reset() override { set (defaultValue); }

    void serialize (chowdsp::JSONSerializer::SerializedType& serial) const override
    {
        serialize<chowdsp::JSONSerializer> (serial, *this);
    }
    void deserialize (chowdsp::JSONSerializer::DeserializedType deserial) override
    {
        deserialize<chowdsp::JSONSerializer> (deserial, *this);
    }

    const PointsContainer defaultValue;

private:
    template <typename Serializer>
    static void serialize (typename Serializer::SerializedType& serial, const SplinePointsState& value)
    {
        Serializer::addChildElement (serial, value.name);
        Serializer::addChildElement (serial, chowdsp::Serialization::serialize<Serializer> (value.get()));
    }

    template <typename Serializer>
    static void deserialize (typename Serializer::DeserializedType deserial, SplinePointsState& value)
    {
        PointsContainer val {};
        chowdsp::Serialization::deserialize<Serializer> (deserial, val);
        value.set (val);
    }

    PointsContainer currentValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SplinePointsState)
};

template <typename PointsContainer>
struct UndoableSplinePointsSet : juce::UndoableAction
{
    explicit UndoableSplinePointsSet (SplinePointsState<PointsContainer>& splineState,
                                      const PointsContainer& newPoints,
                                      juce::Component* uiSource)
        : state (splineState), points (newPoints), source (uiSource)
    {
    }

    SplinePointsState<PointsContainer>& state;
    PointsContainer points;
    juce::Component::SafePointer<juce::Component> source;
    bool firstTime = true;

    bool perform() override
    {
        if (firstTime)
        {
            firstTime = false;
            return true;
        }

        const auto temp = state.get();
        state.set (points);
        points = temp;

        if (auto* comp = source.getComponent())
            comp->repaint();

        return true;
    }

    bool undo() override { return perform(); }
    int getSizeInUnits() override { return int (sizeof (juce::Point<float>) * points.size()); }
};
} // namespace dsp::waveshaper::spline
