#include "SplineWaveshaper.h"

namespace dsp::waveshaper
{
SplineState::SplineState (std::string_view valueName, Spline defaultVal)
    : StateValueBase (valueName),
      defaultValue (std::move (defaultVal)),
      currentValue (defaultValue)
{
}

/** Sets a new value */
void SplineState::set (Spline v)
{
    bool areEqual = true;
    areEqual &= v.size() == currentValue.size();
    for (auto [current, next] : chowdsp::zip (v, currentValue))
        areEqual &= current == next;

    if (areEqual)
        return;

    currentValue = std::move (v);
    changeBroadcaster();
}

SplineState& SplineState::operator= (Spline v)
{
    set (std::move (v));
    return *this;
}

void SplineState::serialize (chowdsp::JSONSerializer::SerializedType& serial) const
{
    serialize<chowdsp::JSONSerializer> (serial, *this);
}

void SplineState::deserialize (chowdsp::JSONSerializer::DeserializedType deserial)
{
    deserialize<chowdsp::JSONSerializer> (deserial, *this);
}

Spline createSpline (std::vector<juce::Point<float>> points)
{
    points.insert (points.begin(), points.front().withX (splineBounds.xMin));
    points.push_back (points.back().withX (splineBounds.xMax));
    //        points.insert (points.begin(), { xMin, yMin });
    //        points.push_back ({ xMax, yMax });

    const auto n = points.size() - 1;
    Spline set;
    set.resize (n, {});

    std::vector<double> h;
    h.reserve (n);
    for (size_t i = 0; i < n; ++i)
        h.push_back (double (points[i + 1].x - points[i].x));

    std::vector<double> alpha;
    alpha.reserve (n);
    alpha.push_back (0);
    for (size_t i = 1; i < n; ++i)
        alpha.push_back (double (3 * (points[i + 1].y - points[i].y) / h[i] - 3 * (points[i].y - points[i - 1].y) / h[i - 1]));

    std::vector<double> l (n + 1);
    std::vector<double> mu (n + 1);
    std::vector<double> z (n + 1);
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

// @TODO: we can definitely optimize this!
double evaluateSpline (const Spline& spline, double x)
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

// ADAA reference: https://www.desmos.com/calculator/o36ytu5rro
double evaluateSplineADAA (const SplineADAASection& ss, double x)
{
    const auto xSq = x * x;
    const auto xCb = chowdsp::Power::ipow<3> (x - ss.x);
    const auto xQ = xCb * (x - ss.x);
    return ss.c0 + ss.c1 * x + ss.c2 * xSq + ss.c3 * xCb + ss.c4 * xQ;
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

std::unique_ptr<SplineADAA> createADAASpline (const Spline& spline)
{
    auto splineADAA = std::make_unique<SplineADAA>();

    splineADAA->first = spline;
    splineADAA->second.resize (spline.size(), {});
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

//======================================
SplineWaveshaper::SplineWaveshaper (SplineState& state)
    : splineState (state)
{
    splineState.changeBroadcaster.connect (
        [this]
        {
            SplinePtr uiSpline;
            uiSpline.ptr = createADAASpline (splineState.get()).release();
            uiToLiveQueue.enqueue (uiSpline);
        });

    startTimer (100);
}

SplineWaveshaper::~SplineWaveshaper()
{
    SplinePtr deadSpline {};
    while (liveToDeadQueue.try_dequeue (deadSpline))
        deadSpline.kill();
    while (uiToLiveQueue.try_dequeue (deadSpline))
        deadSpline.kill();
}

void SplineWaveshaper::timerCallback()
{
    SplinePtr deadSpline {};
    while (liveToDeadQueue.try_dequeue (deadSpline))
        deadSpline.kill();
}

void SplineWaveshaper::prepare (const juce::dsp::ProcessSpec& spec)
{
    dcBlocker.prepare (spec);
    dcBlocker.calcCoefs (30.0f, chowdsp::CoefficientCalculators::butterworthQ<double>, spec.sampleRate);

    x1.resize (spec.numChannels, 0.0);

    spline = createADAASpline (splineState.get());
}

void SplineWaveshaper::reset()
{
    dcBlocker.reset();
}

void SplineWaveshaper::processBlock (const chowdsp::BufferView<double>& buffer) noexcept
{
    SplinePtr splineFromUI {};
    if (uiToLiveQueue.try_dequeue (splineFromUI))
    {
        SplinePtr deadSpline {};
        deadSpline.ptr = spline.release();
        spline.reset (splineFromUI.ptr);
        liveToDeadQueue.try_enqueue (deadSpline);
    }

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
} // namespace dsp::waveshaper
