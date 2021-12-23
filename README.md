# soc_methodology_submission

### 실행 환경 Notation
* (1) : Personal Computer
* (2) : Setup-instance
* (3) : Firesim manager instance
* (4) : Chipyard docker container
* (5) : Firesim F1 instance

### Manager instance pre-setup
* pics안에 있는 그림 01~03을 참고하여 인스턴스 생성 후 로그인
```
(2)$aws configure
(2)$sudo yum -y install python-pip
(2)$sudo pip install boto3
(2)$sudo pip install --upgrade awscli
(2)$wget https://raw.githubusercontent.com/firesim/firesim/master/scripts/aws-setup.py
(2)$python aws-setup.py
```

### Manager instance 셋업
* pics 안에 있는 그림 04~12를 참고하여 Manager instance 생성 (대부분의 작업은 여기서 수행될 예정임)
* 그림 12가 될때까진 약 10분 걸림. 이후 아래 커맨드로 셋업 계속
```
(1)$ssh -i firesim.pem centos@xxx.xxx.xxx.xxx
(3)$git clone https://github.com/firesim/firesim
(3)$cd firesim
(3)$./build-setup.sh fast
(1)$bash scp_instance.sh <Manager_instance_IP>
(3)$source sourceme-f1-manager.sh
(3)$firesim managerinit  ## aws configure access data will be required for this.
```

# 아래 커맨드는 Manager instance 로그인 할 때마다 수행되어야 함.
* ssh-agent를 실행하는 것임.
* 만약 여러명의 개발자들이 수행할 때는 sourceme-f1-manager.sh에서 export FIRESIM_RUNFARM_PREFIX=""에 farm name을 추가해서 각각의 farm을 분리시켜야 개발할 때 편함. (그렇지 않으면 여러 farm이 공유되어서 많은 문제가 야기됨.)
* 아래 커맨드에서 에러가 나면 key copy가 제대로 안되었음. (local에 있는 firesim.pem을 manager instance의 /home/centos로 복사되었는지 확인)
```
source /home/centos/firesim/sourceme-f1-manager.sh
```

### Hwacha-net
* Docker 설치 및 'Chipyard-image' 다운로드
* 해당 도커 이미지에 RISCV cpu 및 Hwacha를 위한 Compiler toolchain이 pre-build 되어 있으므로 편리하게 사용할 수 있음.
* Custom ISA 생성 및 새로 toolchain을 빌드 할 수 있지만, 오래 걸림
```
(3)$sudo yum install docker
(3)$sudo systemctl start docker
(3)$sudo systemctl enable docker
(3)$sudo docker pull ucbbar/chipyard-image
(3)$sudo docker run -it --name hwacha_net_cntr ucbbar/chipyard-image /bin/bash
(4): ctrl+p,q (docker 컨테이너가 실행중인 상태로 밖으로 나옴)
(3)$sudo docker cp /home/centos/hwacha-net hwacha_net_cntr:/root/
(3)$sudo docker attach hwacha_net_cntr
(4)$cd hwacha-net
(4)$make   ## Target 5 layer에 대한 코드가 컴파일 되고, 실행가능한 바이너리가 생성됨.
(4): ctrl+p,q
```

### Firemarshal
* 이 작업은 Target 5 layers 바이너리를 포함한 RISCV CPU의 부팅 이미지를 만드는 작업

```
(3)$bash /home/centos/instance_setup.sh
(3)$source /home/centos/.bashrc
(3)$sudo docker cp hwacha_net_cntr:/root/hwacha-net/soc_project $DIR_FIREMARSHAL/hwacha/overlay/root/
(3)$cd $DIR_FIREMARSHAL/
(3)$./marshal.run  ## 이후 $DIR_FIRESIM/workloads 에 hwacha~~가 생겼는지 확인하기.
```

### Firesim
* 이 작업은 F1 instance를 이용하여 Hwacha에서 Target 5 layer를 실행한 후 종료하는 것 까지임

```
(3)$cd $DIR_FIRESIM
(3)$firesim launchrunfarm
(3)$firesim infrasetup
```
* 위 커맨드를 수행하면서 manager intance 터미널을 새로 열어주고, 아래 작업 잊지말고 말고 해주기.
```
source /home/centos/firesim/sourceme-f1-manager.sh
```

```
(3)$firesim runworkload
(3-new)$ssh <firesim runworkload를 수행한 터미널에서 보이는 로컬 ip 주소값을 입력>
(3-net)$
(3)$firesim terminaterunfarm
```
