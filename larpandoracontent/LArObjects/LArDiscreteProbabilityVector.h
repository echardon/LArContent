/**
 *  @file   larpandoracontent/LArObjects/LArDiscreteProbabilityVector.h
 *
 *  @brief  Header file for the lar discrete cumulative distribution class.
 *
 *  $Log: $
 */
#ifndef LAR_DISCRETE_PROBABILITY_VECTOR_H
#define LAR_DISCRETE_PROBABILITY_VECTOR_H 1

#include <vector>
#include <iostream>

#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

namespace lar_content
{

/**
 *  @brief  DiscreteProbabilityVector class
 */
class DiscreteProbabilityVector
{
public:

    template <typename TX, typename TY>
    using InputDatum = std::pair<TX, TY>;

    template <typename TX, typename TY>
    using InputData = std::vector<InputDatum<TX, TY> >;

    typedef std::vector<float> ResamplingPoints;

    /**
     *  @brief  Constructor
     *
     *  @param  param description
     */
    template <typename TX, typename TY>
    DiscreteProbabilityVector(InputData<TX, TY> const &inputData, const TX xUpperBound);

    /**
     *  @brief  Constructor
     *
     *  @param  param description
     */
    DiscreteProbabilityVector(DiscreteProbabilityVector const &discreteProbabilityVector, ResamplingPoints const &resamplingPoints);

    /**
     *  @brief  Evaluate cumulative probability at arbritrary x
     *
     *  @param  x the x value
     */
    float EvaluateCumulativeProbability(float x) const;

    void Print();


    /**
     *  @brief  Store data to later convert to a cumulative distribution
     *
     *  @param  x independent variable
     *  @param  y dependent variable
     */
    //template <typename T1, typename T2>
    //void CollectInputData(T1 x, T2 y);


private:

    class DiscreteProbabilityDatum
    {
        public:
            DiscreteProbabilityDatum(const float &x, const float &densityDatum, const float &cumulativeDatum);

            float GetX() const;
            float GetDensityDatum() const;
            float GetCumulativeDatum() const;

        private:
            const float m_x;                     ///< The x coordinate
            const float m_densityDatum;          ///< The probability density value
            const float m_cumulativeDatum;       ///< The cumulative probability value
    };
    typedef std::vector<DiscreteProbabilityDatum> DiscreteProbabilityData;

    typedef std::vector<std::pair<float, float > > DiscreteCumulativeProbabilityData;

    template<typename TX, typename TY>
    DiscreteProbabilityData InitialiseDiscreteProbabilityData(DiscreteProbabilityVector::InputData<TX, TY> inputData);

    DiscreteProbabilityData ResampleDiscreteProbabilityData(DiscreteProbabilityVector const &discreteProbabilityVector, ResamplingPoints const &resamplingPoints);

    template <typename TX, typename TY>
    static bool SortInputDataByX(InputDatum<TX, TY> lhs, InputDatum<TX, TY> rhs);

    template <typename TX, typename TY>
    float CalculateNormalisation(InputData<TX, TY> const &inputData);

    void VerifyCompleteData();

    const float m_xUpperBound;
    const DiscreteProbabilityData m_discreteProbabilityData;
};

template <typename TX, typename TY>
inline DiscreteProbabilityVector::DiscreteProbabilityVector(DiscreteProbabilityVector::InputData<TX, TY> const &inputData, const TX xUpperBound) :
    m_xUpperBound(static_cast<float>(xUpperBound)),
    m_discreteProbabilityData(InitialiseDiscreteProbabilityData(inputData))
{
}

inline DiscreteProbabilityVector::DiscreteProbabilityVector(DiscreteProbabilityVector const &discreteProbabilityVector, ResamplingPoints const &resamplingPoints) :
    m_xUpperBound(discreteProbabilityVector.m_xUpperBound),
    m_discreteProbabilityData(ResampleDiscreteProbabilityData(discreteProbabilityVector, resamplingPoints))
{
    VerifyCompleteData();
}




inline float DiscreteProbabilityVector::EvaluateCumulativeProbability(const float x) const
{
    if (2 > m_discreteProbabilityData.size())
        throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);

    if (x > m_discreteProbabilityData.back().GetX())
        return 1.f;

    if (x < m_discreteProbabilityData.front().GetX())
        return 0.f;

    for (size_t iDatum = 1; iDatum < m_discreteProbabilityData.size(); ++iDatum)
    {
        if (x > m_discreteProbabilityData.at(iDatum).GetX())
            continue;

        float xLow(m_discreteProbabilityData.at(iDatum-1).GetX());
        float yLow(m_discreteProbabilityData.at(iDatum-1).GetCumulativeDatum());
        float xHigh(m_discreteProbabilityData.at(iDatum).GetX());
        float yHigh(m_discreteProbabilityData.at(iDatum).GetCumulativeDatum());

        if (std::fabs(xHigh-xLow) < std::numeric_limits<float>::epsilon())
            throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);

        float m((yHigh-yLow)/(xHigh-xLow));
        float c(yLow-m*xLow);

        return m*x+c;
    }

    throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);

    return 0.f;
}

void DiscreteProbabilityVector::Print()
{
    for (size_t iDatum = 0; iDatum < m_discreteProbabilityData.size(); ++iDatum)
    {
        std::cout<<"Datum: " << iDatum << "  x: " << m_discreteProbabilityData.at(iDatum).GetX() << "  density: " << m_discreteProbabilityData.at(iDatum).GetDensityDatum() << "  cumulative: " << m_discreteProbabilityData.at(iDatum).GetCumulativeDatum() << std::endl;
    }
}

inline DiscreteProbabilityVector::DiscreteProbabilityDatum::DiscreteProbabilityDatum(const float &x, const float &densityDatum, const float &cumulativeDatum) :
    m_x(x),
    m_densityDatum(densityDatum),
    m_cumulativeDatum(cumulativeDatum)
{
}

inline float DiscreteProbabilityVector::DiscreteProbabilityDatum::GetX() const
{
    return m_x;
}

inline float DiscreteProbabilityVector::DiscreteProbabilityDatum::GetDensityDatum() const
{
    return m_densityDatum;
}

inline float DiscreteProbabilityVector::DiscreteProbabilityDatum::GetCumulativeDatum() const
{
    return m_cumulativeDatum;
}

template <typename TX, typename TY>
inline DiscreteProbabilityVector::DiscreteProbabilityData DiscreteProbabilityVector::InitialiseDiscreteProbabilityData(DiscreteProbabilityVector::InputData<TX, TY> inputData)
{
    if (2 > inputData.size())
        throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);


    std::sort(inputData.begin(), inputData.end(), DiscreteProbabilityVector::SortInputDataByX<TX,TY>);

    if (inputData.back().first > m_xUpperBound)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);

    float normalisation(CalculateNormalisation(inputData));
    float accumulationDatum(0.f);

    DiscreteProbabilityData data;
    for (size_t iDatum = 0; iDatum < inputData.size(); ++iDatum)
    {
        float x(inputData.at(iDatum).first);
        float densityDatum(static_cast<float>(inputData.at(iDatum).second) / normalisation);
        accumulationDatum+=densityDatum;
        data.emplace_back(DiscreteProbabilityVector::DiscreteProbabilityDatum(x, densityDatum, accumulationDatum));
        std::cout<<iDatum << "  " << x << "  " << densityDatum << "  " << accumulationDatum << std::endl;
    }
    return data;
}

inline DiscreteProbabilityVector::DiscreteProbabilityData DiscreteProbabilityVector::ResampleDiscreteProbabilityData(DiscreteProbabilityVector const & discreteProbabilityVector, ResamplingPoints const & resamplingPoints)
{
    DiscreteProbabilityData resampledProbabilityData;

    float prevCumulativeData(0.f);
    for (size_t iSample = 0; iSample < resamplingPoints.size(); iSample++)
    {
        float xResampled(resamplingPoints.at(iSample));
        float cumulativeDatumResampled(discreteProbabilityVector.EvaluateCumulativeProbability(xResampled));
        float densityDatumResampled(cumulativeDatumResampled-prevCumulativeData);
        resampledProbabilityData.emplace_back(DiscreteProbabilityVector::DiscreteProbabilityDatum(xResampled, densityDatumResampled, cumulativeDatumResampled));
        prevCumulativeData = cumulativeDatumResampled;
    }

    return resampledProbabilityData;
}



template <typename TX, typename TY>
inline bool DiscreteProbabilityVector::SortInputDataByX(InputDatum<TX, TY> lhs, InputDatum<TX, TY> rhs)
{
    float deltaX(static_cast<float>(rhs.first) - static_cast<float>(lhs.first));
    if (std::fabs(deltaX) < std::numeric_limits<float>::epsilon())
        return (lhs.second < rhs.second);

    return (lhs.first < rhs.first);
}

template <typename TX, typename TY>
inline float DiscreteProbabilityVector::CalculateNormalisation(InputData<TX, TY> const &inputData)
{
    if (2 > inputData.size())
        throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);

    float normalisation(0.f);

    for (size_t iDatum = 0; iDatum < inputData.size()-1; ++iDatum)
    {
        std::cout<<"datum " << iDatum << std::endl;
        float deltaX(static_cast<float>(inputData.at(iDatum+1).first) - static_cast<float>(inputData.at(iDatum).first));
        float y(static_cast<float>(inputData.at(iDatum).second));
        normalisation += y*deltaX;
    }
    float deltaX(m_xUpperBound - static_cast<float>(inputData.back().first));
    float y(static_cast<float>(inputData.back().first));
    normalisation += y*deltaX;

    return normalisation;
}

void DiscreteProbabilityVector::VerifyCompleteData()
{
    if (2 > m_discreteProbabilityData.size())
        throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);

    if (m_discreteProbabilityData.back().GetX() > m_xUpperBound)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);

    return;
}


} // namespace lar_content

#endif // £ifndef  LAR_DISCRETE_PROBABILITY_VECTOR_H
