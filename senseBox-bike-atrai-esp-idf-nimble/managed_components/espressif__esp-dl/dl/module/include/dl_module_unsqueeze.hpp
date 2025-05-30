#pragma once

#include "dl_module_base.hpp"

namespace dl {
namespace module {

// https://onnx.ai/onnx/operators/onnx__Unsqueeze.html
class Unsqueeze : public Module {
private:
    TensorBase *m_axes; /*<! Specified axes for output >*/

public:
    /**
     * @brief Construct a new Unsqueeze object.
     *
     * @param name            name of module
     * @param inplace         inplace type.
     */
    Unsqueeze(TensorBase *axes,
              const char *name = NULL,
              module_inplace_t inplace = MODULE_NON_INPLACE,
              quant_type_t quant_type = QUANT_TYPE_NONE) :
        Module(name, inplace, quant_type), m_axes(axes)
    {
    }

    /**
     * @brief Destroy the Unsqueeze object.
     */
    ~Unsqueeze()
    {
        if (m_axes) {
            delete m_axes;
            m_axes = nullptr;
        }
    }

    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes)
    {
        assert(input_shapes.size() == 1);
        std::vector<int> input_shape = input_shapes[0];
        std::vector<int> output_shape;

        int64_t *axes_param = static_cast<int64_t *>(m_axes->get_element_ptr());
        for (int i = 0; i < m_axes->get_size(); i++) {
            if (axes_param[i] < 0) {
                axes_param[i] += input_shape.size() + 1;
            }
        }

        int count = 0;
        for (int i = 0; i < input_shape.size() + m_axes->get_size(); i++) {
            bool in = false;
            for (int j = 0; j < m_axes->get_size(); j++) {
                if (axes_param[j] == i) {
                    in = true;
                    break;
                }
            }
            if (in) {
                output_shape.push_back(1);
                count++;
            } else {
                output_shape.push_back(input_shape[i - count]);
            }
        }

        std::vector<std::vector<int>> output_shapes(1, output_shape);
        return output_shapes;
    }

    void forward(std::vector<dl::TensorBase *> &tensors, runtime_mode_t mode)
    {
        DL_LOG_LAYER_LATENCY_INIT();
        DL_LOG_LAYER_LATENCY_START();
        TensorBase *input = tensors[m_inputs_index[0]];
        TensorBase *output = tensors[m_outputs_index[0]];
        assert(input->get_size() == output->get_size());
        if (output->get_element_ptr() != input->get_element_ptr()) {
            output->assign(input);
        }
        DL_LOG_LAYER_LATENCY_END(this->name, "Unsqueeze");
    }

    void forward_args(void *args) {}

    /**
     * @brief deserialize Unsqueeze module instance by node serialization information
     */
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name)
    {
        Module *op = nullptr;
        quant_type_t quant_type;
        fbs_model->get_operation_attribute(node_name, "quant_type", quant_type);
        TensorBase *axes = fbs_model->get_operation_parameter(node_name, 1);

        // Create module
        op = new Unsqueeze(axes, node_name.c_str(), MODULE_INPLACE_UNCHANGED_BUFFER, quant_type);
        return op;
    }

    void print()
    {
        ESP_LOGI("Unsqueeze",
                 "quant_type: %s, axes: %s.",
                 quant_type_to_string(quant_type),
                 shape_to_string(m_axes->get_shape()).c_str());
    }
};
} // namespace module
} // namespace dl
