/**
 *  @file   LArContent/include/LArUtility/VertexSelectionAlgorithm.h
 * 
 *  @brief  Header file for the vertex selection algorithm class.
 * 
 *  $Log: $
 */
#ifndef LAR_VERTEX_SELECTION_ALGORITHM_H
#define LAR_VERTEX_SELECTION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lar_content
{

/**
 *  @brief  VertexSelectionAlgorithm::Algorithm class
 */
class VertexSelectionAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

    /**
     *  @brief  Default constructor
     */
    VertexSelectionAlgorithm();

private:
    /**
     *  @brief  VertexScore class
     */
    class VertexScore
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  pVertex the address of the vertex
         *  @param  score the score
         */
        VertexScore(pandora::Vertex *pVertex, const float score);

        /**
         *  @brief  Get the address of the vertex
         * 
         *  @return the address of the vertex
         */
        pandora::Vertex *GetVertex() const;

        /**
         *  @brief  Get the score
         * 
         *  @return the score
         */
        float GetScore() const;

        /**
         *  @brief  operator<
         * 
         *  @param  rhs the value for comparison
         * 
         *  @return boolean
         */
        bool operator< (const VertexScore &rhs) const;

    private:
        pandora::Vertex    *m_pVertex;      ///< The address of the vertex
        float               m_score;        ///< The score
    };

    typedef std::vector<VertexScore> VertexScoreList;

    pandora::StatusCode Run();

    /**
     *  @brief  Get the figure of merit for a candidate vertex, using the provided parameters
     * 
     *  @param  pVertex the address of the vertex
     *  @param  maxHitVertexDisplacement the max hit to vertex displacement to consider hit contribution to histogram
     *  @param  hitDeweightingPower the hit power used for distance-weighting hit contributions to histograms
     * 
     *  @return the figure of merit
     */
    float GetFigureOfMerit(const pandora::Vertex *const pVertex, const float maxHitVertexDisplacement, const float hitDeweightingPower) const;

    /**
     *  @brief  Get the figure of merit for a trio of histograms
     * 
     *  @param  histogramU the histogram for the u view
     *  @param  histogramV the histogram for the v view
     *  @param  histogramW the histogram for the w view
     * 
     *  @return the figure of merit
     */
    float GetFigureOfMerit(const pandora::Histogram &histogramU, const pandora::Histogram &histogramV, const pandora::Histogram &histogramW) const;

    /**
     *  @brief  Get the figure of merit contribution for a single histogram
     * 
     *  @param  histogram the histogram
     * 
     *  @return the figure of merit
     */
    float GetFigureOfMerit(const pandora::Histogram &histogram) const;

    /**
     *  @brief  Use hits in clusters (in the provided named list) to fill a provided histogram with hit-vertex relationship information
     * 
     *  @param  pVertex the address of the vertex
     *  @param  hitType the relevant hit type
     *  @param  clusterListName the cluster list name
     *  @param  maxHitVertexDisplacement the max hit to vertex displacement to consider hit contribution to histogram
     *  @param  hitDeweightingPower the hit power used for distance-weighting hit contributions to histograms
     *  @param  histogram to receive the populated histogram
     * 
     *  @return whether the vertex projection lies on a hit in the provided cluster list
     */
    bool FillHistogram(const pandora::Vertex *const pVertex, const pandora::HitType hitType, const std::string &clusterListName,
        const float maxHitVertexDisplacement, const float hitDeweightingPower, pandora::Histogram &histogram) const;

    /**
     *  @brief  Use a provided vertex position and cluster to fill a provided histogram with hit-vertex relationship information
     * 
     *  @param  vertexPosition2D the projected vertex position
     *  @param  pCluster the address of the cluster
     *  @param  maxHitVertexDisplacement the max hit to vertex displacement to consider hit contribution to histogram
     *  @param  hitDeweightingPower the hit power used for distance-weighting hit contributions to histograms
     *  @param  histogram to receive the populated histogram
     * 
     *  @return whether the vertex projection lies on a hit in the provided cluster list
     */
    bool FillHistogram(const pandora::CartesianVector &vertexPosition2D, const pandora::Cluster *const pCluster,
        const float maxHitVertexDisplacement, const float hitDeweightingPower, pandora::Histogram &histogram) const;

    /**
     *  @brief  From the top-scoring candidate vertices, select a subset for further investigation
     * 
     *  @param  vertexScoreList the vertex score list
     *  @param  selectedVertexScoreList to receive the selected vertex score list
     */
    void SelectTopScoreVertices(const VertexScoreList &vertexScoreList, VertexScoreList &selectedVertexScoreList) const;

    /**
     *  @brief  Whether to accept a candidate vertex, based on its spatial position in relation to other selected candidates
     * 
     *  @param  pVertex the address of the vertex
     *  @param  vertexScoreList the vertex score list
     * 
     *  @return boolean
     */
    bool AcceptVertexLocation(const pandora::Vertex *const pVertex, const VertexScoreList &vertexScoreList) const;

    /**
     *  @brief  Whether to accept a candidate vertex, based on its score in relation to other selected candidates
     * 
     *  @param  score the vertex score
     *  @param  vertexScoreList the vertex score list
     * 
     *  @return boolean
     */
    bool AcceptVertexScore(const float score, const VertexScoreList &vertexScoreList) const;

    /**
     *  @brief  Select the final vertex from the list of chosen top-scoring candidates
     * 
     *  @param  vertexScoreList the list of chosen top-scoring vertex candidates
     *  @param  pFinalVertex to receive the address of the final vertex
     */
    void SelectFinalVertex(const VertexScoreList &vertexScoreList, pandora::Vertex *&pFinalVertex) const;

    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    std::string             m_inputClusterListNameU;        ///< The name of the view U cluster list
    std::string             m_inputClusterListNameV;        ///< The name of the view V cluster list
    std::string             m_inputClusterListNameW;        ///< The name of the view W cluster list
    std::string             m_outputVertexListName;         ///< The name under which to save the output vertex list
    bool                    m_replaceCurrentVertexList;     ///< Whether to replace the current vertex list with the output list

    unsigned int            m_histogramNPhiBins;            ///< The number of histogram bins in phi
    float                   m_histogramPhiMin;              ///< The histogram lower phi bound
    float                   m_histogramPhiMax;              ///< The histogram upper phi bound

    float                   m_maxHitVertexDisplacement;     ///< Max hit-vertex displacement for contribution to histograms
    float                   m_maxOnHitDisplacement;         ///< Max hit-vertex displacement for declaring vertex to lie on a hit in each view
    float                   m_hitDeweightingPower;          ///< The hit power used for distance-weighting hit contributions to histograms

    unsigned int            m_maxTopScoreCandidates;        ///< Max number of top-scoring vertices to examine and put forward for selection
    unsigned int            m_maxTopScoreSelections;        ///< Max number of top-scoring vertices to select for further investigation
    float                   m_minCandidateDisplacement;     ///< Ignore other top-scoring candidates located in close proximity to original
    float                   m_minCandidateScoreFraction;    ///< Ignore other top-scoring candidates with score less than a fraction of original

    pandora::FloatVector    m_hitVertexDisplacementList;    ///< The list of hit-vertex displacement cuts used in tests comparing top-scoring vertices
    pandora::FloatVector    m_hitDeweightingPowerList;      ///< The list of hit deweighting powers used in tests comparing top-scoring vertices
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *VertexSelectionAlgorithm::Factory::CreateAlgorithm() const
{
    return new VertexSelectionAlgorithm();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline VertexSelectionAlgorithm::VertexScore::VertexScore(pandora::Vertex *pVertex, const float score) :
    m_pVertex(pVertex),
    m_score(score)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Vertex *VertexSelectionAlgorithm::VertexScore::GetVertex() const
{
    return m_pVertex;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float VertexSelectionAlgorithm::VertexScore::GetScore() const
{
    return m_score;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool VertexSelectionAlgorithm::VertexScore::operator< (const VertexScore &rhs) const
{
    return (this->GetScore() > rhs.GetScore());
}

} // namespace lar_content

#endif // #ifndef LAR_VERTEX_SELECTION_ALGORITHM_H
