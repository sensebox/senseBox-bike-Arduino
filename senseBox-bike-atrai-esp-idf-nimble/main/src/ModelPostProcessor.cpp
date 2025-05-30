#include "ModelPostProcessor.hpp"
#include "surface_category_name.hpp"
#include "dl_model_base.hpp"

ModelPostProcessor::ModelPostProcessor(
    dl::Model *model, const int top_k, const float score_thr, bool need_softmax, const std::string &output_name) :
    ClsPostprocessor(model, top_k, score_thr, need_softmax, output_name)
{
    m_cat_names = surface_cat_names;
}
