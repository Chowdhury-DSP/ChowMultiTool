#include "SignalSmithWaveshaper.h"

namespace dsp::waveshaper
{
static constexpr auto TOL = chowdsp::ScientificRatio<1, -2>::value<double>;

[[nodiscard]] inline auto sin_arg (const xsimd::batch<double>& x)
{
    const auto arg = xsimd::max (xsimd::abs (x), xsimd::broadcast (5.0 * std::numeric_limits<double>::epsilon()));
    const auto y = xsimd::log (arg);
    //    jassert (! xsimd::any (xsimd::isnan (y)));
    return y;
}

[[nodiscard]] inline auto nlFunc (const xsimd::batch<double>& x, double k, double M, double k_sq) noexcept
{
    using chowdsp::Power::ipow;
    return x * ((1.0 - k_sq) + k * xsimd::sin (M * sin_arg (x)));
}

[[nodiscard]] inline auto nlFunc_AD1 (const xsimd::batch<double>& x, double k, double M, double k_sq, double M_sq) noexcept
{
    using chowdsp::Power::ipow;
    const auto [sinterm, costerm] = xsimd::sincos (M * sin_arg (x));

    auto y = 2.0 * sinterm - M * costerm;
    y *= k / (M_sq + 4.0);
    y += 0.5 * (1.0 - k_sq);
    return ipow<2> (x) * y;
}

[[nodiscard]] inline auto nlFunc_AD2 (const xsimd::batch<double>& x, double M, double k_sq, double M_sq, double k_o_M_term) noexcept
{
    using chowdsp::Power::ipow;
    const auto [sinterm, costerm] = xsimd::sincos (M * sin_arg (x));

    auto y = k_o_M_term * ((6.0 - M_sq) * sinterm - 5.0 * M * costerm);
    y += (1.0 - k_sq) / 6.0;
    return ipow<3> (x) * y;
}

void SignalSmithWaveshaper::prepare (double sample_rate, int block_size, int numChannels)
{
    if (sample_rate == 0.0)
        return;

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

        for (auto [n, x] : chowdsp::enumerate (data))
        {
            const auto M_sq = chowdsp::Power::ipow<2> (M_smooth_data[n]);
            const auto k_sq = chowdsp::Power::ipow<2> (k_smooth_data[n]);
            const auto k_o_M_term = k_smooth_data[n] / ((M_sq + 9.0) * (M_sq + 4.0));

            const auto ill_condition_x_x2 = xsimd::abs (x - _x2.get()) < TOL;

            // fallback for x - x2 ill-condition
            const auto y_fallback_x_x2 = [&] (auto this_n, auto this_x)
            {
                const auto x_bar = 0.5 * (this_x + _x2.get());
                const auto delta = x_bar - _x1.get();
                const auto ill_condition_xbar_x1 = xsimd::abs (delta) < TOL;

                return xsimd::select (ill_condition_xbar_x1,
                                      nlFunc (0.5 * (x_bar + _x1.get()), k_smooth_data[this_n], M_smooth_data[this_n], k_sq),
                                      (2.0 / delta) * (nlFunc_AD1 (x_bar, k_smooth_data[this_n], M_smooth_data[this_n], k_sq, M_sq) + (nlFunc_AD2 (_x1.get(), M_smooth_data[this_n], k_sq, M_sq, k_o_M_term) - nlFunc_AD2 (x_bar, M_smooth_data[this_n], k_sq, M_sq, k_o_M_term)) / delta));
            }(n, x);

            const auto adaa1_internal = [&](auto this_n, auto this_x, auto this_x1) -> auto
            {
                const auto y_ic = nlFunc_AD1 (0.5 * (this_x + this_x1), k_smooth_data[this_n], M_smooth_data[this_n], k_sq, M_sq);
                const auto y_ad0 = nlFunc_AD2 (this_x, M_smooth_data[this_n], k_sq, M_sq, k_o_M_term);
                const auto y_ad1 = nlFunc_AD2 (this_x1, M_smooth_data[this_n], k_sq, M_sq, k_o_M_term);
                const auto y_no_ic = (y_ad0 - y_ad1) / (this_x - this_x1);

                const auto illCondition = xsimd::abs (this_x - this_x1) < TOL;
                return xsimd::select (illCondition, y_ic, y_no_ic);
            };
            const auto term1 = adaa1_internal (n, x, _x1.get());
            const auto term2 = adaa1_internal (n, _x1.get(), _x2.get());
            const auto y_ideal = (2.0 / (x - _x2.get())) * (term1 - term2);

            // update state
            _x2.get() = _x1.get();
            _x1.get() = x;

            x = xsimd::select (ill_condition_x_x2, y_fallback_x_x2, y_ideal);
        }
    }
}
} // namespace dsp::waveshaper
