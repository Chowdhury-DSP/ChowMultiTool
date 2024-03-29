//
// Created by Rachel Locke on 04/05/2023.
//
#include "EQOptimiser.h"
#include "EQProcessor.h"
#include "LBFGSB.h"

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wsign-conversion", "-Wsign-compare", "-Wshorten-64-to-32", "-Wimplicit-float-conversion")
// NOLINTBEGIN

namespace dsp::eq
{
using namespace LBFGSpp;

EQOptimiser::EQOptimiser() = default;

float EQOptimiser::operator() (const VectorXf& x, VectorXf& grad, bool is_top_level)
{
    const int n = x.size(); // NOLINT
    const int num_bands = n / 3;
    const int num_points = desiredMagResponse.size();

    //parameters
    std::vector<float> fc (num_bands);
    std::vector<float> g (num_bands);
    std::vector<float> q (num_bands);
    for (int i = 0; i < num_bands; i++)
    {
        fc[i] = x[i];
        g[i] = x[num_bands + i]; //gain dB here
        q[i] = x[2 * num_bands + i];
    }

    //compute mag response from params (freq domain)
    std::array<float, numPoints> magResponse {};
    for (int band = 0; band < num_bands; band++)
    {
        chowdsp::EQ::BellPlot bellFilter;
        bellFilter.setQValue (q[band]);
        bellFilter.setCutoffFrequency (fc[band]);
        bellFilter.setGainDecibels (g[band]);
        //get mag response for band
        std::vector<float> magBandResponse (freqs.size());
        for (int freqIndex = 0; freqIndex < freqs.size(); freqIndex++)
        {
            magBandResponse[freqIndex] = 20 * std::log10 (bellFilter.getMagnitudeForFrequency (freqs[freqIndex]));
        }
        //add magnitude responses to get the overall response
        std::transform (magResponse.begin(), magResponse.end(), magBandResponse.begin(), magResponse.begin(), std::plus<float>());
    }

    //weighted average of absolute difference and squared difference
    float weight = 0.5f;
    float loss = 0.0f;
    for (int i = 0; i < num_points; i++)
    {
        assert (! std::isinf (magResponse[i]));
        assert (! std::isinf (desiredMagResponse[i]));
        float diff = magResponse[i] - desiredMagResponse[i];
        loss += weight * abs (diff) + (1 - weight) * diff * diff;
    }
    loss /= num_points;

    if (is_top_level)
    {
        for (int band = 0; band < num_bands; band++)
        {
            // set gradient for fc parameter
            VectorXf x_plus = x; //temp copy of x
            x_plus[band] += stepSize;
            VectorXf x_minus = x; //temp copy of x
            x_minus[band] -= stepSize;
            float f_plus = this->operator() (x_plus, grad, false); //f(x+h)
            float f_minus = this->operator() (x_minus, grad, false); //f(x-h)
            float g_fc = (f_plus - f_minus) / (2 * stepSize); //(f(x+h) - f(x-h)) / 2h
            grad[band] = g_fc; //g(x) = (f(x+h) - f(x-h)) / 2h

            // set gradient for g parameter
            x_plus = x;
            x_plus[num_bands + band] += stepSize;
            x_minus = x;
            x_minus[num_bands + band] -= stepSize;
            f_plus = this->operator() (x_plus, grad, false);
            f_minus = this->operator() (x_minus, grad, false);
            float g_g = (f_plus - f_minus) / (2 * stepSize);
            grad[num_bands + band] = g_g;

            // set gradient for q parameter
            x_plus = x;
            x_plus[2 * num_bands + band] += stepSize; //f(x+h)
            x_minus = x;
            x_minus[2 * num_bands + band] -= stepSize; //f(x+h)
            f_plus = (*this) (x_plus, grad, false);
            f_minus = (*this) (x_minus, grad, false);
            float g_q = (f_plus - f_minus) / (2 * stepSize);
            grad[2 * num_bands + band] = g_q;
        }
    }
    iterationCount = solver.k;
    if (iterationCount % 10) // write to log every 10 iterations
    {
        juce::Logger::writeToLog ("Optimiser Iteration Count: " + juce::String { iterationCount });
        juce::Logger::writeToLog ("Loss : " + juce::String { loss });
    }
    return loss;
}

void EQOptimiser::runOptimiser (std::array<float, numPoints>&& desiredResponse)
{
    juce::Logger::writeToLog ("Running EQ optimisation...");
    desiredMagResponse = desiredResponse;

    param.ftol = 1e-6;
    param.epsilon_rel = 5e-7;
    param.max_linesearch = 1'000;
    param.max_iterations = maxIter;

    //random number gen
    std::random_device rd;
    std::mt19937 gen (rd());
    std::uniform_real_distribution<float> gDis (-18, 18);
    std::uniform_real_distribution<float> qDis (0.5, 20);
    //create Eigen vectors for initial values and gradient
    int numBands = 8;
    VectorXf lb = VectorXf::Constant (numBands * 3, 0.0f);
    VectorXf ub = VectorXf::Constant (numBands * 3, 0.0f);
    // Set bounds for fcs
    for (int i = 0; i < numBands; i++)
    {
        lb[i] = 20.0f;
        ub[i] = 20000.0f;
    }
    // Set bounds for gs
    for (int i = numBands; i < 2 * numBands; i++)
    {
        lb[i] = -18.0f;
        ub[i] = 18.0f;
    }
    // Set bounds for qs
    for (int i = 2 * numBands; i < 3 * numBands; i++)
    {
        lb[i] = 0.5f;
        ub[i] = 20.f;
    }
    //set initial guess
    VectorXf initial (24);
    for (int i = 0; i < 8; ++i)
    {
        initial (i) = std::pow (10, std::log10 (20.f) + i * (std::log10 (20000.f) - std::log10 (20.f)) / 7.f);
        initial (numBands + i) = gDis (gen);
        initial (2 * numBands + i) = qDis (gen);
    }

    float best_cost;
    int numIters;
    try
    {
        numIters = solver.minimize (*this, initial, best_cost, lb, ub);
    }
    catch (const std::exception& e)
    {
        // TODO: should we alert the user if an exception has occured (I'm leaning towards no)
        juce::Logger::writeToLog (juce::String { "Encountered exception during EQ optimization. Aborting! " } + e.what());
    }

    std::stringstream ss;
    ss << "Optimised Params: " << initial.transpose();
    juce::Logger::writeToLog (ss.str());
    juce::Logger::writeToLog ("Max Optimiser Iter: " + juce::String { numIters });
    optParams = initial.transpose();
}

JUCE_END_IGNORE_WARNINGS_GCC_LIKE
// NOLINTEND

void EQOptimiser::updateEQParameters (chowdsp::EQ::StandardEQParameters<EQToolParams::numBands>& eqParameters,
                                      juce::UndoManager& um) const
{
    static constexpr auto numBands = EQToolParams::numBands;
    using EQParamList = EQToolParams::EQParams::Params;
    EQParamList paramList {};
    for (size_t bandIdx = 0; bandIdx < numBands; bandIdx++)
    {
        paramList.bands[bandIdx].params.bandType = 6; // all optimised filters are "Bell" filters (for now)
        paramList.bands[bandIdx].params.bandFreqHz = optParams[Eigen::Index (bandIdx)];
        paramList.bands[bandIdx].params.bandGainDB = optParams[Eigen::Index (numBands + bandIdx)];
        paramList.bands[bandIdx].params.bandQ = optParams[Eigen::Index (2 * numBands + bandIdx)];
        paramList.bands[bandIdx].params.bandOnOff = std::abs (paramList.bands[bandIdx].params.bandGainDB) > 0.25f; // If the filter gain is very small, we can pretend it's "Off"
    }

    struct UndoableEQLoadParameters : juce::UndoableAction
    {
        chowdsp::EQ::StandardEQParameters<EQToolParams::numBands>& eqParameters;
        EQParamList eqParamList;

        explicit UndoableEQLoadParameters (chowdsp::EQ::StandardEQParameters<EQToolParams::numBands>& eqParams, EQParamList&& newParamList)
            : eqParameters (eqParams),
              eqParamList (std::move (newParamList))
        {
        }

        bool perform() override
        {
            const auto oldEQParams = eqParameters.getEQParameters (eqParameters.eqParams);
            eqParameters.loadEQParameters (eqParamList, eqParameters);
            eqParamList = oldEQParams;
            return true;
        }

        bool undo() override { return perform(); }
        int getSizeInUnits() override { return int (sizeof (*this)); }
    };

    um.beginNewTransaction ("Set Optimised EQ Parameters");
    um.perform (new UndoableEQLoadParameters { eqParameters, std::move (paramList) });
}
} // namespace dsp::eq
