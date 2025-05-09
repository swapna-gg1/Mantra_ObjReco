#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
//#include "tensorflow/lite/version.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "model.h"
#include "app.h"

#define IMAGE_WIDTH 300
#define IMAGE_HEIGHT 300
#define IMAGE_CHANNELS 3
#define IMAGE_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT * IMAGE_CHANNELS)
constexpr int TENSOR_ARENA_SIZE = 300 * 1024; // trial & error

//alignas(16) static uint8_t tensor_arena[TENSOR_ARENA_SIZE];
static uint8_t __attribute__ ((section(".region_nocache"), aligned(16))) tensor_arena[TENSOR_ARENA_SIZE] = { 0 };

const char* coco_labels[92] = {
    "background", "person", "bicycle", "car", "motorcycle", "airplane", "bus",
    "train", "truck", "boat", "traffic light", "fire hydrant", "street sign",
    "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse",
    "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "hat", "backpack",
    "umbrella", "shoe", "eye glasses", "handbag", "tie", "suitcase", "frisbee",
    "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove",
    "skateboard", "surfboard", "tennis racket", "bottle", "plate", "wine glass",
    "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich",
    "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair",
    "couch", "potted plant", "bed", "mirror", "dining table", "window", "desk",
    "toilet", "door", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
    "microwave", "oven", "toaster", "sink", "refrigerator", "blender", "book",
    "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush", "hair brush"
};

void run_object_detection(uint8_t* fb0) 
{

    // Setup
    static tflite::MicroErrorReporter micro_error_reporter;  
    // Define a resolver with the required number of operators
    //constexpr int kOpCount = 10;
    //static tflite::MicroMutableOpResolver<kOpCount> micro_op_resolver;
    static tflite::MicroMutableOpResolver<10> micro_op_resolver;
  
    // Register only the necessary operators for SSD-MobileNet model
    micro_op_resolver.AddDepthwiseConv2D();
    micro_op_resolver.AddConv2D();   
    micro_op_resolver.AddAveragePool2D();
    micro_op_resolver.AddMaxPool2D();
    micro_op_resolver.AddSoftmax();
    micro_op_resolver.AddReshape();
    micro_op_resolver.AddFullyConnected();   
    micro_op_resolver.AddDetectionPostprocess();
    micro_op_resolver.AddLogistic();
    micro_op_resolver.AddAdd();

#if 0          
    const tflite::Model* model = tflite::GetModel(detect_tflite);
   
       
    for (int i = 0; i < 10; ++i) {
        printf("Model data[%d]: 0x%02X\n", i, detect_tflite[i]);
    }
    printf("Model version: %d\n", model->version());

 /*    
  
    if (model->version() != TFLITE_SCHEMA_VERSION) 
    {
        TF_LITE_REPORT_ERROR(&micro_error_reporter,
                           "Model provided is schema version %d not equal "
                           "to supported version %d.",
                           model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }
*/    
    static tflite::MicroInterpreter interpreter(model, micro_op_resolver, tensor_arena,
                                                TENSOR_ARENA_SIZE, &micro_error_reporter);
    
    if (interpreter.AllocateTensors() != kTfLiteOk) 
    {
        TF_LITE_REPORT_ERROR(&micro_error_reporter, "Tensor allocation failed");
        return;
    }
      

    


    TfLiteTensor* input = interpreter.input(0);

    // Convert from BGR framebuffer to model's RGB input
    for (int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++) {
        input->data.uint8[i * 3 + 0] = fb0[i * 3 + 2]; // R
        input->data.uint8[i * 3 + 1] = fb0[i * 3 + 1]; // G
        input->data.uint8[i * 3 + 2] = fb0[i * 3 + 0]; // B
    }

    if (interpreter.Invoke() != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(&micro_error_reporter, "Invoke failed");
        return;
    }

    // SSD-MobileNet v1 quantized output format
    TfLiteTensor* boxes = interpreter.output(0);    // [1,10,4]
    TfLiteTensor* classes = interpreter.output(1);  // [1,10]
    TfLiteTensor* scores = interpreter.output(2);   // [1,10]
    TfLiteTensor* count = interpreter.output(3);    // [1]

    int num = static_cast<int>(count->data.f[0]);
    for (int i = 0; i < num; ++i) 
    {
        float score = scores->data.f[i];
        int class_id = static_cast<int>(classes->data.f[i]);
        if (score > 0.5f) 
        {
            const char* label = (class_id >= 0 && class_id < 91) ? coco_labels[class_id] : "unknown";
            printf("Object %d: %s (class_id=%d) score=%.2f\n", i, label, class_id, score);
        }
    }
#endif
}
