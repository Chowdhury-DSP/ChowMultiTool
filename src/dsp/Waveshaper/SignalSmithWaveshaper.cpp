#include "SignalSmithWaveshaper.h"

namespace dsp::waveshaper
{
static constexpr auto TOL = chowdsp::ScientificRatio<1, -5>::value<double>;

[[nodiscard]] inline auto nlFunc (const xsimd::batch<double>& x, double k, double M, double k_sq) noexcept
{
    using chowdsp::Power::ipow;
    return x * ((1.0 - k_sq) + k * xsimd::sin (M * xsimd::log (xsimd::abs (x))));
}

[[nodiscard]] inline auto nlFunc_AD1 (const xsimd::batch<double>& x, double k, double M, double k_sq, double M_sq) noexcept
{
    using chowdsp::Power::ipow;
    const auto [sinterm, costerm] = xsimd::sincos (M * xsimd::log (xsimd::abs (x)));

    auto y = 2.0 * sinterm - M * costerm;
    y *= k / (M_sq + 4.0);
    y += 0.5 * (1.0 - k_sq);
    return ipow<2> (x) * y;
}

[[nodiscard]] inline auto nlFunc_AD2 (const xsimd::batch<double>& x, double M, double k_sq, double M_sq, double k_o_M_term) noexcept
{
    using chowdsp::Power::ipow;
    const auto [sinterm, costerm] = xsimd::sincos (M * xsimd::log (xsimd::abs (x)));

    auto y = k_o_M_term * ((6.0 - M_sq) * sinterm - 5.0 * M * costerm);
    y += (1.0 - k_sq) / 6.0;
    return ipow<3> (x) * y;
}

void SignalSmithWaveshaper::prepare (double sample_rate, int block_size, int numChannels)
{
    k_smooth.prepare (sample_rate, block_size);
    k_smooth.setRampLength (0.05);
    M_smooth.prepare (sample_rate, block_size);
    M_smooth.setRampLength (0.05);

    x1.resize ((size_t) numChannels, 0.0);
    x2.resize ((size_t) numChannels, 0.0);
}

void SignalSmithWaveshaper::reset()
{
    std::fill (x1.begin(), x1.end(), 0.0);
    std::fill (x2.begin(), x2.end(), 0.0);
}

void SignalSmithWaveshaper::processBlock (const chowdsp::BufferView<xsimd::batch<double>>& buffer, float k_param, float M_param) noexcept
{
    const auto numSamples = buffer.getNumSamples();
    k_smooth.process ((double) k_param, numSamples);
    const auto* k_smooth_data = k_smooth.getSmoothedBuffer();
    M_smooth.process ((double) M_param, numSamples);
    const auto* M_smooth_data = M_smooth.getSmoothedBuffer();

    for (auto [ch, data] : chowdsp::buffer_iters::channels (buffer))
    {
        chowdsp::ScopedValue<xsimd::batch<double>> _x1 { x1[(size_t) ch] };
        chowdsp::ScopedValue<xsimd::batch<double>> _x2 { x2[(size_t) ch] };

        const auto calcD1 = [&k_smooth_data, &M_smooth_data] (const xsimd::batch<double>& x0,
                                                              const xsimd::batch<double>& x1_,
                                                              const xsimd::batch<double>& _ad2_x0,
                                                              const xsimd::batch<double>& _ad2_x1,
                                                              double k_sq,
                                                              double M_sq,
                                                              size_t n)
        {
            const auto illCondition = xsimd::abs (x0 - x1_) < TOL;
            return xsimd::select (illCondition,
                                  nlFunc_AD1 (0.5 * (x0 + x1_), k_smooth_data[n], M_smooth_data[n], k_sq, M_sq),
                                  (_ad2_x0 - _ad2_x1) / (x0 - x1_));
        };

        const auto fallback = [&_x1, &_x2, &k_smooth_data, &M_smooth_data] (const xsimd::batch<double>& x,
                                                                            const xsimd::batch<double>& _ad2_x1,
                                                                            double k_sq,
                                                                            double M_sq,
                                                                            double k_o_M_term,
                                                                            size_t n)
        {
            const auto xBar = 0.5 * (x + _x2.get());
            const auto delta = xBar - _x1.get();
            const auto illCondition = xsimd::abs (delta) < TOL;
            return xsimd::select (illCondition,
                                  nlFunc (0.5 * (xBar + _x1.get()), k_smooth_data[n], M_smooth_data[n], k_sq),
                                  (2.0 / delta) * (nlFunc_AD1 (xBar, k_smooth_data[n], M_smooth_data[n], k_sq, M_sq) + (_ad2_x1 - nlFunc_AD2 (xBar, M_smooth_data[n], k_sq, M_sq, k_o_M_term)) / delta));
        };

        for (auto [n, x] : chowdsp::enumerate (data))
        {
            const auto illCondition = xsimd::abs (x - _x2.get()) < TOL;

            const auto M_sq = chowdsp::Power::ipow<2> (M_smooth_data[n]);
            const auto k_sq = chowdsp::Power::ipow<2> (k_smooth_data[n]);
            const auto k_o_M_term = k_smooth_data[n] / ((M_sq + 9.0) * (M_sq + 4.0));

            const auto _ad2_x0 = nlFunc_AD2 (x, M_smooth_data[n], k_sq, M_sq, k_o_M_term);
            const auto _ad2_x1 = nlFunc_AD2 (_x1.get(), M_smooth_data[n], k_sq, M_sq, k_o_M_term);
            const auto _ad2_x2 = nlFunc_AD2 (_x2.get(), M_smooth_data[n], k_sq, M_sq, k_o_M_term);

            const auto d1 = calcD1 (x, _x1.get(), _ad2_x0, _ad2_x1, k_sq, M_sq, n);
            const auto d2 = calcD1 (_x1.get(), _x2.get(), _ad2_x1, _ad2_x2, k_sq, M_sq, n);
            const auto y = xsimd::select (illCondition, fallback (x, _ad2_x1, k_sq, M_sq, k_o_M_term, n), (2.0 / (x - _x2.get())) * (d1 - d2));

            // update state
            _x2.get() = _x1.get();
            _x1.get() = x;

            x = y;
        }
    }
}
} // namespace dsp::waveshaper
