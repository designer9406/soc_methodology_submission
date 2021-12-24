# soc_methodology_submission
* 각 커맨드 옆에 적힌 시간은 대략적인 시간일뿐, 정확하지 않습니다.
* 처음부터 끝까지는 약 2시간 반 걸렸으며, 처음 한번만 세팅 해놓으면 되는 것들이 있습니다.
* 깃 클론은 우분투 환경의 Personal Computer에 하는 것을 추천드립니다.
* 문의 사항은 아래로 메일을 보내주세요.
* hongjunum@hanyang.ac.kr
* youngbinson@hanyang.ac.kr
* hamjh9510@hanyang.ac.kr

# 실행 환경 Notation
* (1) : Personal Computer
* (2) : Setup-instance
* (3) : Firesim manager instance
* (4) : Chipyard docker container
* (5) : Firesim F1 instance
* (6) : RISCV Linux

# 중요
* 각 실행 커맨드들이 어디서 실행되는 것인지 중요하므로 위의 [실행 환경 Notation]을 잘 확인하여야 함.
* 또한, firesim.pem은 깃 클론 후 권한 확인/변경해야함.
```
(1)$chmod 400 firesim.pem
```

# Firesim pre-setup
* 이 작업은 '566403305827'계정에 대해서는 이미 진행되어 있으므로, Manager instance 셋업으로 바로 넘어가도 됨.
* manager-instance-pics 안에 있는 01~03.png를 참고하여 인스턴스 생성 후 로그인
```
(2)$aws configure
(2)$sudo yum -y install python-pip
(2)$sudo pip install boto3
(2)$sudo pip install --upgrade awscli
(2)$wget https://raw.githubusercontent.com/firesim/firesim/master/scripts/aws-setup.py
(2)$python aws-setup.py
```
* 이후 setup-instance 종료


# Manager instance 셋업
* manager-instance-pics 안에 있는 04~12.png를 참고하여 Manager instance 생성 (대부분의 작업은 이 인스턴스에서 수행될 예정임)
* 07.png의 사용자 데이터에 입력하는 데이터는 manager-instance-pics/user_data.txt에 있는 것을 그대로 복사/붙여넣기
* 12.png가 될때까진 약 10분 걸림. 이후 아래 커맨드로 셋업 계속
```
(3)$git clone https://github.com/firesim/firesim
(3)$cd firesim
(3)$./build-setup.sh fast   ## ~ 30 min (maybe?)
(1)$bash scp_instance.sh <Manager_instance_IP>
(3)$source sourceme-f1-manager.sh
(3)$firesim managerinit  ## aws configure access data will be required for this. ([ID]/[PW]/us-west-2/json)
```

### 아래 커맨드는 Manager instance 로그인 할 때마다 수행되어야 함.
* ssh-agent를 실행하는 것임.
* 만약 여러명의 개발자들이 수행할 때는 sourceme-f1-manager.sh에서 export FIRESIM_RUNFARM_PREFIX=""에 farm name을 추가해서 각각의 farm을 분리시켜야 개발할 때 편함. (그렇지 않으면 여러 farm이 공유되어서 많은 문제가 야기됨.)
* 아래 커맨드에서 에러가 나면 key copy가 제대로 안되었음. (local에 있는 firesim.pem을 manager instance의 /home/centos로 복사되었는지 확인)
```
(3)$source /home/centos/firesim/sourceme-f1-manager.sh
```

# Hwacha-net
* Docker 설치 및 'Chipyard-image' 다운로드
* 해당 도커 이미지에 RISCV cpu 및 Hwacha를 위한 Compiler toolchain이 pre-build 되어 있으므로 편리하게 사용할 수 있음.
```
(3)$sudo yum install docker
(3)$sudo systemctl start docker
(3)$sudo systemctl enable docker
(3)$sudo docker pull ucbbar/chipyard-image   ## ~ 10 min
(3)$sudo docker run -it --name hwacha_net_cntr ucbbar/chipyard-image /bin/bash
(4): ctrl+p,q (docker 컨테이너가 실행중인 상태로 밖으로 나옴)
(3)$sudo docker cp /home/centos/hwacha-net hwacha_net_cntr:/root/
(3)$sudo docker attach hwacha_net_cntr
(4)$cd hwacha-net
(4)$make   ## Target 5 layer에 대한 코드가 컴파일 되고, 실행가능한 바이너리가 생성됨.
(4): ctrl+p,q
```

# Firemarshal
* 이 작업은 Target 5 layers 바이너리를 포함한 RISCV CPU의 부팅 이미지를 만드는 작업

```
(3)$bash /home/centos/instance_setup.sh
(3)$source /home/centos/.bashrc  ## 이후 Manager-instace의 터미널을 다시 여는 것을 추천
(3)$sudo docker cp hwacha_net_cntr:/root/hwacha-net/soc_project $DIR_FIREMARSHAL/hwacha/overlay/root/
(3)$cd $DIR_FIREMARSHAL/
(3)$./init-submodules.sh   ## ~ 10 min
(3)$./marshal -v build br-base.json  ## ~ 40 min. If you are having problems with the message "riscv toolchain not detected. ~~ ", you must have forgotten "source /home/centos/firesim/sourceme-f1-manager.sh".
(3)$./marshal.run  ## 이후 $DIR_FIRESIM/workloads 에 hwacha~~가 생겼는지 확인하기.
```

# Firesim (F1 instance execution)
* 이 작업은 F1 instance를 이용하여 Hwacha에서 Target 5 layer를 실행한 후 종료하는 것 까지임
* Hwacha AGFI(AWS FPGA IMAGE)는 pre-build 되어 있는 것을 사용하였으며, custom build를 하려면 아래 Firesim (Hardware build)를 따라 할 것

```
(3)$cd $DIR_FIRESIM
(3)$firesim launchrunfarm   ## F1 instance 켜주는 작업
(3)$firesim infrasetup      ## F1 instance에서 필요한 여러 환경들을 세팅해줌. (xdma, uart, FPGA download 등?)
```
* 위 커맨드를 수행하면서 manager intance 터미널을 새로 열어주고, 아래 작업 잊지말고 말고 해주기.
```
(3-new)$source /home/centos/firesim/sourceme-f1-manager.sh
```

* ./soc_project 바이너리를 실행하였을 때, Hwacha는 빠르게 끝나지만, CPU는 상당히 오래 걸리는 것으로 보임. (CPU에서 오랫동안 걸리는데, 멈춘 것은 아니며, 계속 돌고 있으므로 최종 값을 보기 위해서는 인내심을 가지고 기다려야 함.)
* 소스코드 상에서 측정한 CPU의 실행 시간은 대략 ~120s정도로 출력됨.
```
(3)$firesim runworkload    ## F1 instance에서 RISCV CPU를 부팅 시킴.
(3-new)$ssh <firesim runworkload를 수행한 터미널에서 보이는 로컬 ip 주소값을 입력>
(5)$screen -r fsim0
(6)$ Login -> ID:root, PW: firesim
(6)$chmod +x soc_project
(6)$./soc_project   ## ~ 30 min
```

* F1 instance 종료
```
(3)$ ctrl+c
(3)$firesim terminaterunfarm -> 'yes'
```

# Firesim (Hardware build)
* $DIR_FIRESIM의 폴더에 config_build.ini, config_build_recipes.ini, config_hwdb.ini, config_runtime.ini가 있음
* config_build_recipes.ini  : AGFI를 만들 목록
* config_build.ini  : config_build_recipes에 있는 것들 중 어떤 것을 만들 것인지 설정해줌 -> 아래 'firesim buildafi' 명령을 통해 Build instance를 새로 생성하고, 해당 인스턴스에서 하드웨어 합성이 실행됨.
* config_hwdb.ini  : 합성된 Hardware의 AGFI ID가 저장되어 있음
* config_runtime.ini : F1 instance에서 사용하게 될 AGFI 및 RISCV CPU 부팅 이미지를 지정해줌

```
(3)$cd $DIR_FIRESIM
(3)$firesim buildafi
```

* 추가 설명
* config_build_recipes.ini에 TARGET_CONFIG의 끝부분에 'chipyard.HwachaRocketConfig'로 정의되어 있음
* 해당 내용은 ~/firesim/target-design/chipyard/generators/chipyard/src/main/scala/config/RocketConfigs.scala에 class로 선언되어 있음

# Reference

* Firesim: https://docs.fires.im/en/latest/index.html
* Firemarshal: https://firemarshal.readthedocs.io/en/latest/index.html
* Chipyard: https://chipyard.readthedocs.io/en/latest/

# End
