// Author: Florian Lier [flier AT techfak.uni-bielefeld DOT de]

/*
 * Copyright (c) 2012-2016 Aldebaran Robotics. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the COPYING file.
 */

#include "naoqiwrapper.hpp"

NaoQiWrapper::NaoQiWrapper(std::string _robot_ip, std::string _robot_port) {
    robot_ip = _robot_ip;
    std::cout << ">>> Using Robot " << _robot_ip << ":" << std::atoi(_robot_port.c_str()) << std::endl;
    motion = new AL::ALMotionProxy(_robot_ip, std::atoi(_robot_port.c_str()));
    robot_posture = new AL::ALRobotPostureProxy(robot_ip);
    fractionMaxSpeed = 0.3f;
    joints = AL::ALValue::array("HeadYaw", "HeadPitch");
    angles = AL::ALValue::array(0.0f, 0.0f);
    stiff = AL::ALValue::array(1.0f, 1.0f);
    motion->setFallManagerEnabled(true);
    motion->setSmartStiffnessEnabled(true);
    wake();
    setStiff(1.0f);
    motion->setStiffnesses(joints, stiff);
    std::cout << ">>> Setting Stiffness to 1.0" << std::endl;
    robot_posture->goToPosture("Stand", 0.5f);
    std::cout << ">>> Standing Up" << std::endl;
    motion->moveInit();
    AL::ALValue breathConfig;
    breathConfig.arraySetSize(2);
    AL::ALValue tmp;
    tmp.arraySetSize(2);
    tmp[0] = "Bpm";
    tmp[1] = 10.0f;
    breathConfig[0] = tmp;
    tmp[0] = "Amplitude";
    tmp[1] = 1.0f;
    breathConfig[1] = tmp;
    motion->setBreathConfig(breathConfig);
    motion->setBreathEnabled("Body", true);
}

NaoQiWrapper::~NaoQiWrapper(){}

float NaoQiWrapper::getHeadPitch(){
    return motion->getAngles("HeadPitch", useSensors)[0];
}

float NaoQiWrapper::getHeadYaw(){
    return motion->getAngles("HeadYaw", useSensors)[0];
}

void NaoQiWrapper::setHeadYaw(float _yaw) {
    angles[0] = degreeToRad(_yaw);
}

void NaoQiWrapper::setHeadPitch(float _pitch) {
    angles[1] = degreeToRad(_pitch);
}

void NaoQiWrapper::setFractionSpeed(float _speed) {
    fractionMaxSpeed = _speed;
}

void NaoQiWrapper::setStiff(float _stiff) {
    for (int i=0; i < joints.getSize(); i++) {
        stiff[i] = _stiff;
    }
}

void NaoQiWrapper::executeMotion() {
    motion->setAngles(joints, angles, fractionMaxSpeed);
    // std::cout << "Executing Motion" << std::endl;
}

float NaoQiWrapper::degreeToRad(float _angle){
    return (_angle * M_PI) / 180;
}

float NaoQiWrapper::getRandom(float min, float max) {
    return  (max - min) * ((((float) rand()) / (float) RAND_MAX)) + min ;
}

void NaoQiWrapper::shutdown(){
    std::cout << ">>> Setting Stiffness to 0.0" << std::endl;
    setStiff(0.0f);
    motion->setStiffnesses(joints, stiff);
    robot_posture->goToPosture("SitRelax", 0.5f);
    rest();
}

void NaoQiWrapper::wake(){
    std::cout << ">>> Waking Up" << std::endl;
    motion->wakeUp();
}

void NaoQiWrapper::rest(){
    std::cout << ">>> Resting" << std::endl;
    motion->rest();
}
