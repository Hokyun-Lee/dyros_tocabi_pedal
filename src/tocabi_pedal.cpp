#include "../include/dyros_tocabi_pedal/tocabi_pedal.h"

TocabiPedal::TocabiPedal(int size_) : speed_MAF(size_), angvel_MAF(size_)
{
    pedal_sub_ = nh_.subscribe<sensor_msgs::Joy>("joy", 10, &TocabiPedal::pedalCallback, this);
    
    // joy_command_pub_ = nh_.advertise<std_msgs::Float32>("/joy_topic", 1);
    // joy_command_pub_ = nh_.advertise<geometry_msgs::Twist>("/joy_topic", 1);
    
    walkingspeed_pub = nh_.advertise<std_msgs::Float32 >("/tocabi/walkingspeedcommand", 100);
    walkingduration_pub = nh_.advertise<std_msgs::Float32>("/tocabi/walkingdurationcommand", 100);
    walkingangvel_pub = nh_.advertise<std_msgs::Float32>("/tocabi/walkingangvelcommand", 100);
    kneetargetangle_pub = nh_.advertise<std_msgs::Float32>("/tocabi/kneetargetanglecommand", 100);
    footheight_pub = nh_.advertise<std_msgs::Float32>("/tocabi/footheightcommand", 100);
    // speed_value = 0;

    walkingduration_msg.data = 0.6;
    kneetargetangle_msg.data = 0.1;
    footheight_msg.data = 0.05;
    mode = 0; //default : 0
}

void TocabiPedal::pedalCallback(const sensor_msgs::Joy::ConstPtr& pedal)
{
    std::cout << "-----------------------------------" << std::endl << std::endl;
    std::cout << "mode : " << mode << std::endl << std::endl;

    R_pedal = pedal->axes[0];
    L_pedal = pedal->axes[1];
    Rot_pedal = pedal->axes[2];
    std::cout << "R_pedal value(%) : " << (R_pedal+1)*50 << std::endl;
    std::cout << "L_pedal value(%) : " << (L_pedal+1)*50 << std::endl;
    std::cout << "Rot_pedal value(%) : " << Rot_pedal*100 << std::endl << std::endl;

    SaturationValue();

    R_pedal = (R_pedal+0.9); // max_speed = 0.6
    L_pedal = (L_pedal+0.9); // min_speed = -0.4
    Rot_pedal = Rot_pedal; // max,min_angvel = +-0.5 회전각도, 시계방향 (-), 반시계방향(+)
    
    if(mode == 0){ //init mode
        if (R_pedal > 0.02){
            mode = 1;
        }
        else if(L_pedal > 0.02){
            mode = 2;
        }
        walkingspeed_msg.data = 0;
        speed_MAF.enqueue(walkingspeed_msg.data);
        double speed_sum = 0;
        std::cout << "speed queue : ";
        std::cout.precision(5);
        for(int i = 0; i < speed_MAF.capacity; i++){
            if(i % 10 == 0)
                std::cout << std::endl;
            std::cout << speed_MAF.cq[i] << " ";
            speed_sum += speed_MAF.cq[i];
        }
        std::cout << std::endl << std::endl;
        walkingspeed_msg.data = speed_sum/speed_MAF.capacity;
        walkingspeed_pub.publish(walkingspeed_msg);
        std::cout << "walkingspeed : " << walkingspeed_msg.data << std::endl << std::endl;
        // std::cout << "pedal :" << R_pedal << L_pedal << std::endl;
    } 
    else if(mode == 1){ //foward mode
        if (R_pedal < 0.02){
            mode = 0;
        }
        walkingspeed_msg.data = R_pedal/3;
        speed_MAF.enqueue(walkingspeed_msg.data);
        double speed_sum = 0;
        std::cout << "speed queue : ";
        std::cout.precision(5);
        for(int i = 0; i < speed_MAF.capacity; i++){
            if(i % 10 == 0)
                std::cout << std::endl;
            std::cout << speed_MAF.cq[i] << " ";
            speed_sum += speed_MAF.cq[i];
        }
        std::cout << std::endl << std::endl;
        walkingspeed_msg.data = speed_sum/speed_MAF.capacity;
        walkingspeed_pub.publish(walkingspeed_msg);
        std::cout << "walkingspeed : " << walkingspeed_msg.data << std::endl << std::endl;
        // std::cout << "pedal :" << R_pedal << L_pedal << std::endl;
    }
    else if(mode == 2){ //backward mode
        if (L_pedal < 0.02){
            mode = 0;
        }
        walkingspeed_msg.data = -1 * L_pedal*2/9;
        speed_MAF.enqueue(walkingspeed_msg.data);
        double speed_sum = 0;
        std::cout << "speed queue : ";
        std::cout.precision(5);
        for(int i = 0; i < speed_MAF.capacity; i++){
            if(i % 10 == 0)
                std::cout << std::endl;
            std::cout << speed_MAF.cq[i] << " ";
            speed_sum += speed_MAF.cq[i];
        }
        std::cout << std::endl << std::endl;
        walkingspeed_msg.data = speed_sum/speed_MAF.capacity;
        walkingspeed_pub.publish(walkingspeed_msg);
        std::cout << "walking speed : " << walkingspeed_msg.data << std::endl << std::endl;
        // std::cout << "pedal :" << R_pedal << L_pedal << std::endl;
    }

    walkingangvel_msg.data = Rot_pedal*2/5;
    angvel_MAF.enqueue(walkingangvel_msg.data);
    double angvel_sum = 0;
    std::cout << "angvel queue : ";
    std::cout.precision(5);
    for(int i = 0; i < angvel_MAF.capacity; i++){
        if(i % 10 == 0)
            std::cout << std::endl;
        std::cout << angvel_MAF.cq[i] << " ";
        angvel_sum += angvel_MAF.cq[i];
    }
    std::cout << std::endl << std::endl;
    walkingangvel_msg.data = angvel_sum/angvel_MAF.capacity;
    walkingangvel_pub.publish(walkingangvel_msg);
    std::cout << "angle velocity : " << walkingangvel_msg.data << std::endl << std::endl;


}

void TocabiPedal::SaturationValue()
{
 if(R_pedal > 0.9)
   {
       R_pedal = 0.9;
   }
   else if(R_pedal < -0.9)
   {
       R_pedal = -0.9;
   }
   if(L_pedal > 0.9)
   {
       L_pedal = 0.9;
   }
   else if(L_pedal < -0.9)
   {
       L_pedal = -0.9;
   }
}

MovAvgFilter::MovAvgFilter(int size):front(0), back(0), capacity(size)
{
    cq = new double[size + 1]; //더미노드까지 포함하여 포인터를 생성함
    for(int i = 0; i < size; i++){
        enqueue(0);
    }
}

void MovAvgFilter::enqueue(double data)
{
    int pos;
    // back이 마지막 인덱스에 채우는 경우
    // back은 계속 순환하니까 0으로 초기화
    // 마지막엔 pos로 담는다.
    if (back == capacity-1){
        pos = back;
        cq[pos] = data;
        back = 0;
    }
    else
        cq[back++] = data;
}

int MovAvgFilter::dequeue()
{
    int pos = front;
    // front가 마지막 인덱스의 데이터를 제거하는 경우
    // front도 순환하므로 0으로 초기화
    // 리턴은 pos로 한다.
    if (front == capacity - 1) front = 0;
    else front++;
    return cq[pos];
}

int MovAvgFilter::getFront() const
{
    return cq[front];
}
 
int MovAvgFilter::getBack() const
{
    // back은 enqueue에서 증가하므로 -1 해서 접근해야 함
    return cq[back-1];
}
 
bool MovAvgFilter::isEmpty() const
{
    return front == back;
}

int MovAvgFilter::getSize() const
{
    // front가 back보다 작거나 같은경우
    if (front <= back) return back - front;
    // front가 back보다 큰 경우는 
    // back이 다시 돌아오고 front가 진행하는 경우이다.
    else if (front > back) return capacity - front + back;
}

bool MovAvgFilter::isFull() const
{
    // front가 back보다 작으면 back이 더미 바로 전에 있어야 함
    if (front <= back) return back - front == capacity - 1;
    // front가 back보다 크면 back이 front 바로 전에 있어야 함
    else return back + 1 == front;
}


int main(int argc, char** argv)
{
    std::cout << "Started~~"<<std::endl;
    ros::init(argc, argv, "dyros_tocabi_Pedal");
    int capacity = 50;
    TocabiPedal tocabiPedal(capacity);
    std::cout << "Dyros Tocabi Pedal Controller Started"<<std::endl;
    ros::Rate rate(20);
    while(ros::ok())
    {
        ros::spinOnce();
        rate.sleep();
    }
}