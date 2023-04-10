#include "SplineWaveshaper.h"

namespace dsp::waveshaper::spline
{
SplinePoints DefaultSplineCreator::call()
{
    static constexpr auto scalerAndOffset = getSplineScalerAndOffset<float>();
    static constexpr auto scaler = scalerAndOffset.first;
    static constexpr auto offset = scalerAndOffset.second;

    SplinePoints points;
    for (auto [index, point] : chowdsp::enumerate (points))
    {
        point.x = ((float) index - offset) / scaler;
        point.y = std::tanh (point.x);
    }
    return points;
}

Spline createSpline (const SplinePoints& points)
{
    static constexpr auto n = size_t (maxNumDrawPoints - 1);
    std::array<double, n> h {};
    std::array<double, n> alpha {};
    std::array<double, (size_t) maxNumDrawPoints> l {};
    std::array<double, (size_t) maxNumDrawPoints> mu {};
    std::array<double, (size_t) maxNumDrawPoints> z {};

    return createSpline<Spline> (points, h, alpha, l, mu, z);
}

double evaluateSpline (const Spline& spline, double x)
{
    x = juce::jlimit ((double) splineBounds.xMin, (double) splineBounds.xMax, x);

    static constexpr auto scalerAndOffset = getSplineScalerAndOffset();
    static constexpr auto scaler = scalerAndOffset.first;
    static constexpr auto offset = scalerAndOffset.second;
    const auto splineSetIndex = juce::truncatePositiveToUnsignedInt (scaler * x + offset);

    const auto& ss = spline[splineSetIndex];
    return chowdsp::Polynomials::estrin<3> ({ ss.d, ss.c, ss.b, ss.a }, x - ss.x);
}

// ADAA reference: https://www.desmos.com/calculator/o36ytu5rro
double evaluateSplineADAA (const SplineADAASection& ss, double x)
{
    const auto xSq = x * x;
    const auto xCb = chowdsp::Power::ipow<3> (x - ss.x);
    const auto xQ = xCb * (x - ss.x);
    return ss.c0 + ss.c1 * x + ss.c2 * xSq + ss.c3 * xCb + ss.c4 * xQ;
}

double evaluateSplineADAA (const std::array<SplineADAASection, maxNumDrawPoints - 1>& spline, double x)
{
    x = juce::jlimit ((double) splineBounds.xMin, (double) splineBounds.xMax, x);

    static constexpr auto scaler = double (maxNumDrawPoints - 1) / double (splineBounds.xMax - splineBounds.xMin);
    static constexpr auto offset = (double) -splineBounds.xMin * scaler;
    const auto splineSetIndex = juce::truncatePositiveToUnsignedInt (scaler * x + offset);

    return evaluateSplineADAA (spline[splineSetIndex], x);
}

std::unique_ptr<SplineADAA> createADAASpline (const SplinePoints& splinePoints)
{
    auto splineADAA = std::make_unique<SplineADAA>();
    const auto& spline = splineADAA->first = createSpline (splinePoints);
    for (auto [adaa, sp] : chowdsp::zip (splineADAA->second, spline))
    {
        adaa.c0 = sp.c * chowdsp::Power::ipow<3> (sp.x) / 3.0;
        adaa.c1 = sp.a - sp.b * sp.x;
        adaa.c2 = sp.b * 0.5;
        adaa.c3 = sp.c / 3.0;
        adaa.c4 = sp.d * 0.25;
        adaa.x = sp.x;
    }

    for (size_t i = 1; i < spline.size(); ++i)
    {
        auto& ss = splineADAA->second[i];
        ss.c0 += evaluateSplineADAA (splineADAA->second[i - 1], ss.x) - evaluateSplineADAA (ss, ss.x);
    }

    return splineADAA;
}

//========================================================================
VectorSplinePoints DefaultVectorSplineCreator::call()
{
    //    VectorSplinePoints points {};
    //    points.emplace_back (splineBounds.xMin, -1.0f);
    //    points.emplace_back (splineBounds.xMax, 1.0f);
    return {};
}

VectorSpline createSpline (const VectorSplinePoints& points)
{
    auto copyPoints = points;
    if (copyPoints.empty())
    {
        copyPoints.emplace_back (splineBounds.xMin, -1.0f);
        copyPoints.emplace_back (splineBounds.xMax, 1.0f);
    }
    else
    {
        copyPoints.insert (copyPoints.begin(), points.front().withX (splineBounds.xMin));
        copyPoints.push_back (points.back().withX (splineBounds.xMax));
    }

    const auto numDrawPoints = copyPoints.size();
    const auto n = size_t (numDrawPoints - 1);
    std::vector<double> data {};
    data.resize (2 * n + 3 * numDrawPoints);

    std::span<double> h { data.data(), n };
    std::span<double> alpha { data.data() + n, n };
    std::span<double> l { data.data() + 2 * n, numDrawPoints };
    std::span<double> mu { data.data() + 2 * n + numDrawPoints, numDrawPoints };
    std::span<double> z { data.data() + 2 * n + 2 * numDrawPoints, numDrawPoints };

    return createSpline<VectorSpline> (copyPoints, h, alpha, l, mu, z);
}

double evaluateSpline (const VectorSpline& spline, double x)
{
    x = juce::jlimit ((double) splineBounds.xMin, (double) splineBounds.xMax, x);

    size_t splineSetIndex = 0;
    for (size_t ii = splineSetIndex + 1; ii < spline.size(); ++ii)
    {
        if (x < spline[ii].x)
            break;
        splineSetIndex = ii;
    }

    const auto& ss = spline[splineSetIndex];
    return chowdsp::Polynomials::estrin<3> ({ ss.d, ss.c, ss.b, ss.a }, x - ss.x);
}

double evaluateSplineADAA (const std::vector<SplineADAASection>& spline, double x)
{
    x = juce::jlimit ((double) splineBounds.xMin, (double) splineBounds.xMax, x);

    size_t splineSetIndex = 0;
    for (size_t ii = splineSetIndex + 1; ii < spline.size(); ++ii)
    {
        if (x < spline[ii].x)
            break;
        splineSetIndex = ii;
    }

    return evaluateSplineADAA (spline[splineSetIndex], x);
}

std::unique_ptr<VectorSplineADAA> createADAASpline (const VectorSplinePoints& splinePoints)
{
    if (splinePoints.empty())
        return {};

    auto splineADAA = std::make_unique<VectorSplineADAA>();
    const auto& spline = splineADAA->first = createSpline (splinePoints);
    splineADAA->second.resize (spline.size(), {});
    for (auto [adaa, sp] : chowdsp::zip (splineADAA->second, splineADAA->first))
    {
        adaa.c0 = sp.c * chowdsp::Power::ipow<3> (sp.x) / 3.0;
        adaa.c1 = sp.a - sp.b * sp.x;
        adaa.c2 = sp.b * 0.5;
        adaa.c3 = sp.c / 3.0;
        adaa.c4 = sp.d * 0.25;
        adaa.x = sp.x;
    }

    for (size_t i = 1; i < spline.size(); ++i)
    {
        auto& ss = splineADAA->second[i];
        ss.c0 += evaluateSplineADAA (splineADAA->second[i - 1], ss.x) - evaluateSplineADAA (ss, ss.x);
    }

    return splineADAA;
}

//======================================
template <typename SplinePointsType, typename SplineADAAType>
SplineWaveshaper<SplinePointsType, SplineADAAType>::SplineWaveshaper (SplinePointsState<SplinePointsType>& state)
    : splineState (state)
{
    splineState.changeBroadcaster.connect ([this]
                                           { splineUIToAudioPipeline.write (createADAASpline (splineState.get())); });
}

template <typename SplinePointsType, typename SplineADAAType>
void SplineWaveshaper<SplinePointsType, SplineADAAType>::prepare (const juce::dsp::ProcessSpec& spec)
{
    dcBlocker.prepare (spec);
    dcBlocker.calcCoefs (1.0, spec.sampleRate);

    x1.resize (spec.numChannels, 0.0);

    splineUIToAudioPipeline.write (createADAASpline (splineState.get()));
    splineUIToAudioPipeline.read();
}

template <typename SplinePointsType, typename SplineADAAType>
void SplineWaveshaper<SplinePointsType, SplineADAAType>::reset()
{
    std::fill (x1.begin(), x1.end(), 0.0f);
    dcBlocker.reset();
}

template <typename SplinePointsType, typename SplineADAAType>
void SplineWaveshaper<SplinePointsType, SplineADAAType>::processBlock (const chowdsp::BufferView<double>& buffer) noexcept
{
    const auto* spline = splineUIToAudioPipeline.read();
    if (spline == nullptr)
        return;

    if (spline->first.empty() || spline->second.empty())
        return;

    for (auto [idx, channel] : chowdsp::buffer_iters::channels (buffer))
    {
        chowdsp::ScopedValue _x1 { x1[(size_t) idx] };

        for (auto& x : channel)
        {
            double y;
            if (std::abs (x - _x1.get()) < 1.0e-6)
            {
                const auto& ss = spline->first;
                y = 0.5 * (evaluateSpline (ss, x) + evaluateSpline (ss, _x1.get()));
            }
            else
            {
                const auto& ss = spline->second;
                y = (evaluateSplineADAA (ss, x) - evaluateSplineADAA (ss, _x1.get())) / (x - _x1.get());
            }

            _x1.get() = x;
            x = y;
        }
    }

    dcBlocker.processBlock (buffer);
}

template class SplineWaveshaper<SplinePoints, SplineADAA>;
template class SplineWaveshaper<VectorSplinePoints, VectorSplineADAA>;
} // namespace dsp::waveshaper::spline
