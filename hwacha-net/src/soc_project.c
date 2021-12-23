#include <stdio.h>
#include <sys/time.h>
#include "layer.h"
#include "util.h"

#define NLAYERS 9

void swap(float** a, float** b)
{
  void* t = *a;
  *a = *b;
  *b = t;
}

int main(int argc, char** argv)
{
  //hwacha_init();
  printf("----- Start test\n");

  layer \
    conv1, pool1,
    conv2, pool2,
    conv3, pool3,
    conv4, pool4,
    conv5;
  layer* layers[NLAYERS] = {
    &conv1, &pool1,
    &conv2, &pool2,
    &conv3, &pool3,
    &conv4, &pool4,
    &conv5
    };

  float* input_hwacha, *input_cpu;
  float* output_hwacha, *output_cpu;
  float* workspace_hwacha, *workspace_cpu;


  printf("----- Create layers\n");
  {
    conv1.type = conv2.type = conv3.type = conv4.type = conv5.type = CONVOLUTIONAL;
    pool1.type = pool2.type = pool3.type = pool4.type = MAXPOOL;
    
    conv1.n =  64; conv1.size =  3; conv1.stride =  1; conv1.pad =  1;
    pool1.n =  64; pool1.size =  2; pool1.stride =  2; pool1.pad =  0; // Actually, 'n of pool layer' is a 'dont care' for the value.

    conv2.n = 128; conv2.size =  3; conv2.stride =  1; conv2.pad =  1;
    pool2.n = 128; pool2.size =  2; pool2.stride =  2; pool2.pad =  0;

    conv3.n = 256; conv3.size =  3; conv3.stride =  1; conv3.pad =  1;
    pool3.n = 256; pool3.size =  2; pool3.stride =  2; pool3.pad =  0;

    conv4.n = 512; conv4.size =  3; conv4.stride =  1; conv4.pad =  1;
    pool4.n = 512; pool4.size =  2; pool4.stride =  2; pool4.pad =  0;

    conv5.n = 512; conv5.size =  3; conv5.stride =  1; conv5.pad =  1;

    layer start;
    start.type = START;
    start.output_h = start.output_w = 224;
    start.output_c = 3;
    start.prec = SINGLE;

    setup_layers(&start, &conv1);
    for (int i = 0; i < NLAYERS - 1; i++)
      setup_layers(layers[i], layers[i+1]);
  }

  {
    size_t max_io = max_size(layers, NLAYERS);
    size_t max_ws = max_workspace(layers, NLAYERS);

    input_hwacha = safe_malloc(sizeof(float) * max_io);
    output_hwacha = safe_malloc(sizeof(float) * max_io);
    workspace_hwacha  = safe_malloc(sizeof(float) * max_ws);

    input_cpu = safe_malloc(sizeof(float) * max_io);
    output_cpu = safe_malloc(sizeof(float) * max_io);
    workspace_cpu = safe_malloc(sizeof(float) * max_ws);

  }
  
  printf("----- Load weights\n");
  {
    for (int i=0; i<NLAYERS; i++){
      layer* l = layers[i];

      l->weights_32 = safe_malloc(l->nweights * sizeof(float));
      for (int idx=0; idx<l->nweights; idx++)
        l->weights_32[idx] = 0.001;
    }
  }

  printf("----- Load images\n");
  {
    for(int i = 0; i < 224*224*3; i++){
      input_hwacha[i] = (float)1.0;
      input_cpu[i] = (float)1.0;
    }
  }

    struct timeval T_start, T_end;
    double exec_time_hwacha, exec_time_cpu;

    printf("----- Run 5 layers on Hwacha\n");
    gettimeofday(&T_start, NULL);
  
    layer_forward(&conv1, input_hwacha, output_hwacha, workspace_hwacha);swap(&input_hwacha, &output_hwacha);
    layer_forward(&pool1, input_hwacha, output_hwacha, workspace_hwacha);
                                                                       
    layer_forward(&conv2, input_hwacha, output_hwacha, workspace_hwacha); swap(&input_hwacha, &output_hwacha);
    layer_forward(&pool2, input_hwacha, output_hwacha, workspace_hwacha);
                                                                       
    layer_forward(&conv3, input_hwacha, output_hwacha, workspace_hwacha); swap(&input_hwacha, &output_hwacha);
    layer_forward(&pool3, input_hwacha, output_hwacha, workspace_hwacha);
                                                                       
    layer_forward(&conv4, input_hwacha, output_hwacha, workspace_hwacha); swap(&input_hwacha, &output_hwacha);
    layer_forward(&pool4, input_hwacha, output_hwacha, workspace_hwacha);
                                                                       
    layer_forward(&conv5, input_hwacha, output_hwacha, workspace_hwacha);
  
    gettimeofday(&T_end, NULL);
    exec_time_hwacha = (double)( T_end.tv_sec - T_start.tv_sec ) + ((double)( T_end.tv_usec - T_start.tv_usec ) / 1000000);
    printf("Exec time  (hwacha):  %f s\n", exec_time_hwacha);

    printf("----- Run 5 layers on CPU\n");
    gettimeofday(&T_start, NULL);
  
    layer_forward_cpu(&conv1, input_cpu, output_cpu, workspace_cpu); swap(&input_cpu, &output_cpu);
    layer_forward_cpu(&pool1, input_cpu, output_cpu, workspace_cpu);
                                                   
    layer_forward_cpu(&conv2, input_cpu, output_cpu, workspace_cpu); swap(&input_cpu, &output_cpu);
    layer_forward_cpu(&pool2, input_cpu, output_cpu, workspace_cpu);
                                                   
    layer_forward_cpu(&conv3, input_cpu, output_cpu, workspace_cpu); swap(&input_cpu, &output_cpu);
    layer_forward_cpu(&pool3, input_cpu, output_cpu, workspace_cpu);
                                                   
    layer_forward_cpu(&conv4, input_cpu, output_cpu, workspace_cpu); swap(&input_cpu, &output_cpu);
    layer_forward_cpu(&pool4, input_cpu, output_cpu, workspace_cpu);
                                                   
    layer_forward_cpu(&conv5, input_cpu, output_cpu, workspace_cpu);
  
    gettimeofday(&T_end, NULL);
    exec_time_cpu = (double)( T_end.tv_sec - T_start.tv_sec ) + ((double)( T_end.tv_usec - T_start.tv_usec ) / 1000000);
    printf("Exec time  (cpu):  %f s\n", exec_time_cpu);

  printf("----- Check results\n");
  {
    for(int j=0; j<14; j++){
      for(int i=0; i<14; i++){
        if (output_hwacha[i+14*j] != output_cpu[i+14*j]) printf("%.3f\t %.3f", i+14*j, output_hwacha[i+14*j], output_cpu[i+14*j]);
      }
    }
  }
  
  return 0;
}
