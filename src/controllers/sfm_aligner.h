#ifndef SRC_CONTROLLERS_SFM_ALIGNER_H_
#define SRC_CONTROLLERS_SFM_ALIGNER_H_

#include <vector>
#include <memory>

#include "util/bitmap.h"
#include "base/reconstruction.h"
#include "base/similarity_transform.h"
#include "estimators/sim3.h"
#include "optim/bundle_adjustment.h"
#include "graph/graph.h"
#include "sfm/twoview_info.h"

using namespace colmap;
using namespace GraphSfM::graph;

namespace GraphSfM {

using SimilarityGraph = std::unordered_map<size_t, std::unordered_map<size_t, Sim3>>;

class SfMAligner
{
public:
    const static std::vector<BitmapColor<float>> ColorContainers;
    struct AlignOptions
    {
        bool merge_largest_cc = true;

        double threshold = 0.1;

        double confidence = 0.99;

        double max_reprojection_error = 1.8;

        bool retriangulate = false;

        bool final_ba = false;

        bool assign_color_for_clusters = false;
    };

    struct Summary
    {
        double construct_recon_graph_time;

        double construct_mst_time;

        double find_anchor_node_time;

        double compute_final_transformation_time;

        double merging_time;
    };

    SfMAligner(const std::vector<Reconstruction*>& reconstructions,
               const AlignOptions& options);

    bool Align();

    Node GetAnchorNode() const;

    std::vector<Sim3> GetSim3ToAnchor() const;

private:
    AlignOptions options_;

    SfMAligner::Summary summary_;

    std::vector<Reconstruction*> reconstructions_;

    Graph<Node, Edge> recons_graph_;

    Node anchor_node_;

    // The similarity transformation to anchor node, which is used for final transformation
    std::vector<Sim3> sim3_to_anchor_;

    std::vector<std::unordered_map<size_t, Sim3>> paths_;

    // similarity graph, each cluster is represented as a node,
    // and each edge is the relative similarity transformation
    SimilarityGraph sim3_graph_;    

    void ConstructReconsGraph();

    double ComputeEdgeWeight(const uint i, const uint j);

    void FindAnchorNode(Graph<Node, Edge>* graph);

    void ComputePath(int src, int dst);

    void MergeReconstructions(std::vector<size_t>& node_ids);
};

} // namespace GraphSfM

#endif