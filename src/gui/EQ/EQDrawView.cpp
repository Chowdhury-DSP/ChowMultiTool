#include "EQDrawView.h"
#include "gui/Shared/Colours.h"

namespace gui::eq
{
float getMagnitudeAtFrequency (const EQPath& eqPath, float frequencyHz, const chowdsp::SpectrumPlotParams& plotParams)
{
    const auto xNorm = std::log (frequencyHz / plotParams.minFrequencyHz) / std::log (plotParams.maxFrequencyHz / plotParams.minFrequencyHz);
    const auto indexForFrequency = (size_t) juce::roundToInt (juce::jmap (xNorm, 0.0f, (float) maxNumDrawPoints - 1.0f));
    return eqPath[indexForFrequency].y;
}

EQDrawView::EQDrawView (const chowdsp::SpectrumPlotBase& plotBase)
    : spectrumPlot (plotBase)
{
}

void EQDrawView::paint (juce::Graphics& g)
{
    const auto drawCircle = [&g, dim = 0.035f * (float) getHeight()] (juce::Point<float> pos)
    {
        g.fillEllipse (juce::Rectangle { dim, dim }.withCentre (pos));
    };

    if (mousePos.has_value())
    {
        g.setColour (colours::thumbColours[0].withAlpha (0.75f));
        drawCircle (*mousePos);
    }

    const auto getPoint = [this] (float xCoord)
    {
        const auto freqHz = spectrumPlot.getFrequencyForXCoordinate (xCoord);
        const auto magDB = getMagnitudeAtFrequency (eqPath, freqHz, spectrumPlot.params);
        return juce::Point { xCoord, spectrumPlot.getYCoordinateForDecibels (magDB) };
    };

    juce::Path eqPlotPath;
    eqPlotPath.startNewSubPath (getPoint (0.0f));

    for (float x = 1.0f; x < (float) getWidth(); x += 0.5f)
    {
        eqPlotPath.lineTo (getPoint (x));
    }

    g.setColour (colours::thumbColours[0]);
    g.strokePath (eqPlotPath, juce::PathStrokeType { juce::PathStrokeType::curved });
}

std::array<float, dsp::eq::EQOptimiser::numPoints> EQDrawView::getDrawnMagnitudeResponse()
{
    std::array<float, dsp::eq::EQOptimiser::numPoints> freqs = optimiser.freqs;
    std::cout << "Frequency Vector: ";
    for (auto& f : freqs)
        std::cout << f << " ";
    std::cout << "\n";
    std::array<float, dsp::eq::EQOptimiser::numPoints> magnitudeResponse;

    std::cout << "Magnitude Response - Get Drawn Magnitude Response: ";
    for (size_t i = 0; i < dsp::eq::EQOptimiser::numPoints; i++)
    {
        magnitudeResponse[i] = gui::eq::getMagnitudeAtFrequency (eqPath, freqs[i], spectrumPlot.params);
        std::cout << magnitudeResponse[i] << " ";
    }
    std::cout << "\n";
    std::cout << "Magnitude Response - Get Drawn Magnitude Response (Return): ";
    for (auto& mags : magnitudeResponse)
        std::cout << mags << " ";
    return magnitudeResponse;
}

void EQDrawView::triggerOptimiser (chowdsp::EQ::StandardEQParameters<dsp::eq::EQToolParams::numBands>& eqParameters)
{
    // get desired response from eqPath
    auto desiredResponse = getDrawnMagnitudeResponse(); //mag dB here
    std::cout << "\n";
    std::cout << "Magnitude Response - Trigger Optimiser: ";
    for (auto& mag : desiredResponse)
        std::cout << mag << " ";
    std::cout << "\n";
    optimiser.runOptimiser (std::move (desiredResponse));
    optimiser.updateEQParameters (eqParameters);
}

//VectorXf EQDrawView::EQDrawViewOptimise()
//{
//    getDrawnMagnitudeResponse();
//
//    //Set up parameters
//    LBFGSBParam<float> param;
//    param.ftol = 1e-5;
//    param.epsilon_rel = 5e-7;
//    param.max_linesearch = 1'000.f;
//    param.max_iterations = 1'000.f;
//
//    // Initialize the solver
//    LBFGSBSolver<float> solver(param);
//
//    //random number gen
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_real_distribution<float> gDis(-12, 12);
//    std::uniform_real_distribution<float> qDis(0.5, 10);
//    //create Eigen vectors for initial values and gradient
//    int numBands = 8;
//    VectorXf lb = VectorXf::Constant(numBands * 3, 0.0f);
//    VectorXf ub = VectorXf::Constant(numBands * 3, 0.0f);
//    // Set bounds for fcs
//    for (int i = 0; i < numBands; i++) {
//        lb[i] = 20.0f;
//        ub[i] = 20000.0f;
//    }
//    // Set bounds for gs
//    for (int i = numBands; i < 2*numBands; i++) {
//        lb[i] = -12.0f;
//        ub[i] = 12.0f;
//    }
//    // Set bounds for qs
//    for (int i = 2*numBands; i < 3*numBands; i++) {
//        lb[i] = 0.5f;
//        ub[i] = 20.f;
//    }
//    //set initial guess
//    VectorXf initial(24);
//    for (int i = 0; i < 8; ++i) {
//        initial(i) = std::pow(10, std::log10(20.f) + i * (std::log10(20000.f) - std::log10(20.f)) / 7.f);
//        initial(numBands + i) = gDis(gen);
//        initial(2 * numBands + i) = qDis(gen);
//    }
//
//    float best_cost;
//
//    EQOptimiser optimiser(gui::eq::desiredMagnitudeResponse, gui::eq::frequencyVector);
//
//    auto min = solver.minimize(optimiser, initial, best_cost, lb, ub);
//    eqMinimizerMaxIter = min;
//    return initial.transpose();
//}

void EQDrawView::setEQPathPoint (juce::Point<float> point)
{
    const auto pointIndex = (size_t) juce::roundToInt (juce::jmap (point.x, 0.0f, (float) getWidth(), 0.0f, (float) maxNumDrawPoints - 1.0f));
    eqPath[pointIndex].y = juce::jmap ((float) getHeight() - point.y, 0.0f, (float) getHeight(), -18.0f, 18.0f);
}

void EQDrawView::mouseDown (const juce::MouseEvent& e)
{
    lastMouseDragPoint = e.getEventRelativeTo (this).getPosition().toFloat();
    setEQPathPoint (lastMouseDragPoint);
    repaint();
}

void EQDrawView::mouseDrag (const juce::MouseEvent& e)
{
    auto newMouseDragPoint = e.getEventRelativeTo (this).getPosition().toFloat();
    newMouseDragPoint.x = juce::jlimit (0.0f, (float) getWidth(), newMouseDragPoint.x);
    newMouseDragPoint.y = juce::jlimit (0.0f, (float) getHeight(), newMouseDragPoint.y);
    if (lastMouseDragPoint.x < newMouseDragPoint.x)
    {
        for (float x = lastMouseDragPoint.x; x <= newMouseDragPoint.x; x += 0.1f)
        {
            float pct = (x - lastMouseDragPoint.x) / (newMouseDragPoint.x - lastMouseDragPoint.x);
            setEQPathPoint ({ x, juce::jmap (pct, lastMouseDragPoint.y, newMouseDragPoint.y) });
        }
    }
    else if (newMouseDragPoint.x < lastMouseDragPoint.x)
    {
        for (float x = lastMouseDragPoint.x; x >= newMouseDragPoint.x; x -= 0.1f)
        {
            float pct = (x - lastMouseDragPoint.x) / (newMouseDragPoint.x - lastMouseDragPoint.x);
            setEQPathPoint ({ x, juce::jmap (pct, lastMouseDragPoint.y, newMouseDragPoint.y) });
        }
    }

    lastMouseDragPoint = newMouseDragPoint;
    mousePos.emplace (lastMouseDragPoint);

    repaint();
}

void EQDrawView::mouseMove (const juce::MouseEvent& e)
{
    const auto pos = e.getEventRelativeTo (this).getPosition().toFloat();
    mousePos.emplace (pos);
    repaint();
}

void EQDrawView::mouseExit (const juce::MouseEvent&)
{
    mousePos.reset();
    repaint();
}
} // namespace gui::eq
