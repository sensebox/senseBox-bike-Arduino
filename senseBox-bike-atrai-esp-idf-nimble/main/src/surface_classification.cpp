#include "surface_classification.hpp"

dl::image::img_t decode_jpeg_to_img(const camera_fb_t *pic) {
    dl::image::jpeg_img_t jpeg_img = {
        .data = (uint8_t *) pic->buf,
        .width = (int) pic->width,
        .height = (int) pic->height,
        .data_size = (uint32_t)(pic->len),
    };

    dl::image::img_t img;
    img.pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB888;
    sw_decode_jpeg(jpeg_img, img, true);
    
    return img;
}

std::vector<dl::cls::result_t> run_inference(
    dl::Model *model, 
    const dl::image::img_t &img, 
    const char *tag) 
{
    ESP_LOGI(tag, "Running inference on image with width: %zu and height: %zu", img.width, img.height);
    
    dl::image::ImagePreprocessor image_preprocessor(model, {123.675, 116.28, 103.53}, {58.395, 57.12, 57.375});
    image_preprocessor.preprocess(img);

    model->run();

    ModelPostProcessor postprocessor(model, 5, std::numeric_limits<float>::lowest(), true);
    return postprocessor.postprocess();
}