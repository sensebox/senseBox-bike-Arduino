#include "dl_model_base.hpp"
#include "dl_image_define.hpp"
#include "dl_image_preprocessor.hpp"
#include "dl_cls_postprocessor.hpp"
#include "dl_image_jpeg.hpp"
#include "surface_category_name.hpp"
#include "esp_log.h"
#include "ModelPostProcessor.hpp"
#include "esp_camera.h"

dl::image::img_t decode_jpeg_to_img(const camera_fb_t *pic);
std::vector<dl::cls::result_t> run_inference(
    dl::Model *model, 
    const dl::image::img_t &img, 
    const char *tag);