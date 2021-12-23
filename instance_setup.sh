#!/bin/bash

FIREMARSHAL_DIR=/home/centos/firesim/sw/firesim-software
FIRESIM_DIR=/home/centos/firesim/deploy
mkdir -p $FIREMARSHAL_DIR/hwacha/overlay/root

echo "{
  "name" : "hwacha",
  "base" : "br-base.json",
  "overlay" : "overlay",
}
" > $FIREMARSHAL_DIR/hwacha.json

echo "#!/bin/bash

rm -rf /home/centos/firesim/target-design/chipyard/software/firemarshal/images/hwacha*
./marshal build hwacha.json
./marshal install hwacha.json
" > $FIREMARSHAL_DIR/marshal.run

chmod +x $FIREMARSHAL_DIR/marshal.run

cp /home/centos/firesim_config/* $FIRESIM_DIR/

echo "
DIR_FIREMARSHAL=/home/centos/firesim/sw/firesim-software
DIR_FIRESIM=/home/centos/firesim/deploy
" >> ~/.bashrc
