# DYROS TOCABI PEDAL

* made by modifying [DYROS TOCABI JOYSTICK](https://github.com/Hokyun-Lee/dyros_tocabi_joystick)
* This is a DYROS Tocabi Humanoid's PEDAL control interface Repository
* using [THRUSTMASTER TFRP RUDDER PEDALS](http://www.thrustmaster.com/ko_KR/press/thrustmaster-tfrp-rudder-pedals-smart)
* in Ubuntu 16.04.6 LTS with ROS <br>
* using [Joy(ROS driver for a generic Linux joystick)](http://wiki.ros.org/joy).

<p align="center"><img src="https://user-images.githubusercontent.com/68094299/89493148-6b439180-d7ee-11ea-8457-ba2b5c2ff1b0.png" height="300"><img src="https://user-images.githubusercontent.com/68094299/89492962-db9de300-d7ed-11ea-8901-9ee6053f3ca9.png" height="300">

## Configuring Joy ##
1. Joy 패키지 설치
```sh
sudo apt-get install ros-kinetic-joy
```

2. Rudder Pedal 인식 확인
* Connect your Rudder Pedal to your computer.
```sh
ls /dev/input/
```

* listing the permissions of the Pedal.(in my case, js0)
```sh
ls -1 /dev/input/jsX
sudo chmod a+rw /dev/input/jsX
```

* test joystick
```sh
sudo jstest /dev/input/jsX
```

* Starting the joy Node
```sh
roscore
rosparam set joy_node/dev "/dev/input/jsX"
rosrun joy joy_node
```
* Check published topic
```sh
rostopic echo /joy
```

<p align="center"><img src="https://user-images.githubusercontent.com/68094299/89494391-09385b80-d7f1-11ea-85a3-139cc3c5e128.png" height="300">
<br></br>

 ## How do I run dyros_tocabi_pedal ? ##
* Launch DaegyuLim's Custom-Controller (https://github.com/DaegyuLim/Custom-Controller)
* Enable mujoco
* in tocabi_gui > COMMAND > Mode > Select "13.CustomController 4" > Click "SEND" <br>

* roslaunch
```ch
roslaunch dyros_tocabi_pedal pedal.launch
```
  also, you can check topic with `rostopic list` and `rostopic echo /tocabi/...`
  <br></br>
