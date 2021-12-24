# Hwacha-net

* 아래 항목에 대하여 설명
* Makefile
* src/


# Makefile
* Line 3: 컴파일러 -> Main README.md에 있는 Hwacha-net 파트의 docker container를 실행하게 되면 내부에 Pre-build toolchain이 있음.
* Line 16: 컴파일할 때 사용되는 플래그로 hwacha extension
* Line 41: target 5 layer가 정의되어 있는 soc_project.c의 파일 명

# src/

### soc_project.c

* Line 41~56: 모델 정의
* Line 58~62: 맨 처음 시작 레이어 구조 정의
* Line 64~66: 시작 레이어와 모델의 각 레이어 연결

* Line 85~91: 파라미터 세팅
* Line 96~99: 인폿 데이터 세팅

* Line 108~120: Hwacha에서 5 layers 실행
* Line 129~141: cpu에서 5 layers 실행


### layer.c

* soc_project.c에서 사용되는 layer_forward(), layer_forward_cpu()가 정의되어 있음

### convolutional_layer.c

* layer.c에서 사용되는 convolutional_precomp_forward_32(), convolutional_precomp_forward_32_cpu()가 정의되어 있음

### gemm.c

* convolutional_layer.c에서 사용되는 gemm_32(), gemm_32_cpu()가 정의되어 있음
* M(Output feature map), N(Input feature map), K(Kernel) loop으로 gemm이 동작함
