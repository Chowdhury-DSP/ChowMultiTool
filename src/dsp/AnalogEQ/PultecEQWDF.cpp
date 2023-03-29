#include "PultecEQWDF.h"

namespace dsp::analog_eq
{
void PultecEqWDF::prepare (float sampleRate)
{
    C_treble_boost.prepare (sampleRate);
    L_treble_boost.prepare (sampleRate);
    C_low_boost.prepare (sampleRate);
    C_treble_cut.prepare (sampleRate);
    C_low_cut.prepare (sampleRate);
}

void PultecEqWDF::reset()
{
    C_treble_boost.reset();
    L_treble_boost.reset();
    C_low_boost.reset();
    C_treble_cut.reset();
    C_low_cut.reset();
}

void PultecEqWDF::setParameters (float treble_boost, float treble_boost_q, float treble_boost_freq, float treble_cut, float treble_cut_freq, float low_boost, float low_cut, float bass_freq)
{
    // @TODO: defer impedance changes?

    // see sim/pultec_bass_freq_curve_fit.py
    P_low_cut.setResistanceValue (1.0f + std::pow (low_cut, 1.443f) * 99.999e3f);
    P_low_boost.setResistanceValue (1.0f + std::pow (low_boost, 2.587f) * 9.999e3f);
    const auto bass_boost_cap_val = 1.0f / (juce::MathConstants<float>::twoPi * 1.13062281e+04f * (bass_freq - 1.30692349e+01f)) + 1.68909808e-07f;
    const auto bass_cut_cap_val = 1.0f / (juce::MathConstants<float>::twoPi * 2.27142549e+05f * (bass_freq - 1.24565293e+01f)) + 7.10814416e-09f;
    C_low_boost.setCapacitanceValue (bass_boost_cap_val);
    C_low_cut.setCapacitanceValue (bass_cut_cap_val);

    const auto treble_cut_skew = std::pow (treble_cut, 0.395f);
    P_treble_cut_plus.setResistanceValue (1.0e3f * (1.0f - treble_cut_skew));
    P_treble_cut_minus.setResistanceValue (1.0e3f * treble_cut_skew);
    const auto treble_cut_cap_val = 1.0f / (juce::MathConstants<float>::twoPi * 169.3f * treble_cut_freq);
    C_treble_cut.setCapacitanceValue (treble_cut_cap_val);

    const auto treble_boost_skew = std::pow (treble_boost, 0.621f);
    P_treble_boost_plus.setResistanceValue (10.0e3f * (1.0f - treble_boost_skew));
    P_treble_boost_minus.setResistanceValue (10.0e3f * treble_boost_skew);
    R_treble_boost_bw.setResistanceValue (1.0f + (1.0f - treble_boost_q) * 2499.0f);

    // see sim/pultec_treble_boost_freq_curve_fit.py
    static constexpr auto A_r = gcem::sqrt (1.0e7f);
    const auto g_val = chowdsp::Polynomials::estrin<3> ({ -4.96665892e-17f, 1.73050404e-12f, -2.05889893e-08f, 9.75043069e-05f }, treble_boost_freq);
    const auto treble_boost_cap_val = g_val / A_r;
    const auto treble_boost_ind_val = chowdsp::Power::ipow<2> (A_r) * treble_boost_cap_val;
    C_treble_boost.setCapacitanceValue (treble_boost_cap_val);
    L_treble_boost.setInductanceValue (treble_boost_ind_val);
}
} // namespace dsp::pultec
