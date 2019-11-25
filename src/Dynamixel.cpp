// -*- C++ -*-
/*!
 * @file  Dynamixel.cpp
 * @brief Component for Multiple Dynamixels
 * @date $Date$
 *
 * $Id$
 */

#include "Dynamixel.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;


// Module specification
// <rtc-template block="module_spec">
static const char* dynamixel_spec[] =
  {
    "implementation_id", "Dynamixel",
    "type_name",         "Dynamixel",
    "description",       "Component for Multiple Dynamixels",
    "version",           "1.0.0",
    "vendor",            "MasutaniLab",
    "category",          "Actuator",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.NUM_ACTUATOR", "5",
    "conf.default.DEVICENAME", "COM9",
    "conf.default.BAUDRATE", "1000000",
    "conf.default.MODEL", "AX12",
    "conf.default.initialPositionString", "512",
    "conf.default.initialSpeedString", "30",

    // Widget
    "conf.__widget__.NUM_ACTUATOR", "text",
    "conf.__widget__.DEVICENAME", "text",
    "conf.__widget__.BAUDRATE", "text",
    "conf.__widget__.MODEL", "text",
    "conf.__widget__.initialPositionString", "text",
    "conf.__widget__.initialSpeedString", "text",
    // Constraints

    "conf.__type__.NUM_ACTUATOR", "short",
    "conf.__type__.DEVICENAME", "string",
    "conf.__type__.BAUDRATE", "int",
    "conf.__type__.MODEL", "string",
    "conf.__type__.initialPositionString", "string",
    "conf.__type__.initialSpeedString", "string",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Dynamixel::Dynamixel(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_goalPositionIn("goalPosition", m_goalPosition),
    m_movingSpeedIn("movingSpeed", m_movingSpeed),
    m_pGainIn("pGain", m_pGain),
    m_presentPositionOut("presentPosition", m_presentPosition),
    m_movingOut("moving", m_moving)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Dynamixel::~Dynamixel()
{
}



RTC::ReturnCode_t Dynamixel::onInitialize()
{
  RTC_INFO(("onInitialize()"));
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("goalPosition", m_goalPositionIn);
  addInPort("movingSpeed", m_movingSpeedIn);
  addInPort("pGain", m_pGainIn);
  
  // Set OutPort buffer
  addOutPort("presentPosition", m_presentPositionOut);
  addOutPort("moving", m_movingOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("NUM_ACTUATOR", m_NUM_ACTUATOR, "5");
  bindParameter("DEVICENAME", m_DEVICENAME, "COM9");
  bindParameter("BAUDRATE", m_BAUDRATE, "1000000");
  bindParameter("MODEL", m_MODEL, "AX12");
  bindParameter("initialPositionString", m_initialPositionString, "512");
  bindParameter("initialSpeedString", m_initialSpeedString, "30");
  // </rtc-template>
  m_pDynamixel = nullptr;
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Dynamixel::onFinalize()
{
  RTC_INFO(("onFinalize()"));

  if (m_pDynamixel != nullptr) {
    delete m_pDynamixel;
  }

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Dynamixel::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Dynamixel::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t Dynamixel::onActivated(RTC::UniqueId ec_id)
{
  RTC_INFO(("onActivated()"));
  m_rtcError = false;  //OpenRTM-aist-1.2.0のバグ回避
  m_ready = false;
  try {
    if (m_pDynamixel == nullptr) {
      m_pDynamixel
        = new DynamixelBase(m_MODEL, m_NUM_ACTUATOR, m_DEVICENAME, m_BAUDRATE);
    }
  }
  catch (std::exception& e) {
    RTC_ERROR((e.what()));
    m_rtcError = true;  //OpenRTM-aist-1.2.0のバグ回避
    return RTC::RTC_ERROR;
  }

  // 位置指令の初期値の変換
  m_initialPosition.resize(m_NUM_ACTUATOR);
  stringToVector(m_initialPositionString, m_initialPosition);
  {
    ostringstream oss;
    oss << "m_initialPosition: ";
    for (int i = 0; i < m_NUM_ACTUATOR; i++) {
      oss << m_initialPosition[i] << ", ";
    }
    RTC_INFO((oss.str().c_str()));
  }

  // 速度の初期値の変換
  m_initialSpeed.resize(m_NUM_ACTUATOR);
  stringToVector(m_initialSpeedString, m_initialSpeed);
  {
    ostringstream oss;
    oss << "m_initialSpeed: ";
    for (int i = 0; i < m_NUM_ACTUATOR; i++) {
      oss << m_initialSpeed[i] << ", ";
    }
    RTC_INFO((oss.str().c_str()));
  }

  m_speed = m_initialSpeed;

  //各ダイナミクセルのトルクの最大値を設定する．
  m_errorType = m_pDynamixel->setAllTorqueMax();
  if (m_errorType != ETNone) {
    m_rtcError = true;  //OpenRTM-aist-1.2.0のバグ回避
    return RTC::RTC_ERROR;
  }
  RTC_INFO(("全アクチュエータのトルク制限を最大に設定"));

  //各ダイナミクセルのトルクをオンにする．
  m_errorType = m_pDynamixel->setAllTorqueOn();
  if (m_errorType != ETNone) {
    m_rtcError = true;  //OpenRTM-aist-1.2.0のバグ回避
    return RTC::RTC_ERROR;
  }
  RTC_INFO(("全アクチュエータのトルクをオン"));

  m_errorType = m_pDynamixel->setSpeedAndPosition(m_initialSpeed, m_initialPosition);
  if (m_errorType != ETNone) {
    m_rtcError = true;  //OpenRTM-aist-1.2.0のバグ回避
    return RTC::RTC_ERROR;
  }
  RTC_INFO(("初期位置・初期速度設定"));

  //出力ポート用の変数のサイズ設定
  m_presentPosition.data.length(m_NUM_ACTUATOR); 
  m_moving.data.length(m_NUM_ACTUATOR);

  m_countCommunicationError = 0;

  m_ready = true;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Dynamixel::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_INFO(("onDeactivated()"));

  if (m_ready) {
#if 0 //トルクオフにするとかえって危ないので何もしない
    //全てのダイナミクセルのトルクをオフにする
    m_errorType = m_pDynamixel->setAllTorqueOff();
    if (m_errorType != ETNone) {
      return RTC::RTC_ERROR;
    }
    RTC_INFO(("全アクチュエータのトルクをオフに設定"));
#endif
    m_pDynamixel->clearPort();
  }

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Dynamixel::onExecute(RTC::UniqueId ec_id)
{
  if (m_rtcError) return RTC::RTC_ERROR; //OpenRTM-aist-1.2.0のバグ回避

  // ゲインの設定
  if (m_pGainIn.isNew()) {
    m_pGainIn.read();
    vector<uint16_t> gain;
    for (int i = 0; i < m_NUM_ACTUATOR; i++) {
      gain.push_back(m_pGain.data[i]);
    }
    m_errorType = m_pDynamixel->setPGain(gain);
    if (m_errorType != ETNone) {
      return RTC::RTC_ERROR;
    }
  }

  if (m_movingSpeedIn.isNew()) {
    m_movingSpeedIn.read();
    for (int i = 0; i<m_NUM_ACTUATOR; i++) {
      m_speed[i] = m_movingSpeed.data[i];
    }
  }

  if(m_goalPositionIn.isNew()) {
    m_goalPositionIn.read();
    vector<uint16_t> position;
    for (int i = 0; i<m_NUM_ACTUATOR; i++) {
      position.push_back(m_goalPosition.data[i]);
    }

    m_errorType = m_pDynamixel->setSpeedAndPosition(m_speed, position);
    if (m_errorType != ETNone) {
      return RTC::RTC_ERROR;
    }
  }

  //movingの処理
  vector<uint16_t> moving;
  m_errorType = m_pDynamixel->getMoving(moving);
  if (m_errorType != ETNone && m_errorType != ETCommunication) {
    return RTC::RTC_ERROR;
  }
  //ETCommunicationの場合はエラー状態に遷移させないがポート出力しない
  if (m_errorType == ETCommunication) {
    m_countCommunicationError++;
  } else {
    for (int i = 0; i < m_NUM_ACTUATOR; i++) {
      m_moving.data[i] = moving[i];
    }
    m_movingOut.write();
    m_countCommunicationError = 0;
  }

  // 現在位置の処理
  vector<uint16_t> position;
  m_errorType = m_pDynamixel->getPosition(position);
  if (m_errorType != ETNone && m_errorType != ETCommunication) {
    return RTC::RTC_ERROR;
  }
  //ETCommunicationの場合はエラー状態に遷移させないがポート出力しない
  if (m_errorType == ETCommunication) {
    m_countCommunicationError++;
  } else {
    for (int i = 0; i < m_NUM_ACTUATOR; i++) {
      m_presentPosition.data[i] = position[i];
    }
    m_presentPositionOut.write();
    m_countCommunicationError = 0;
  }

  if (m_countCommunicationError > 10) {
    RTC_ERROR(("通信エラー連続発生"));
    return RTC::RTC_ERROR;
  }

  return RTC::RTC_OK;
}


RTC::ReturnCode_t Dynamixel::onAborting(RTC::UniqueId ec_id)
{
  RTC_INFO(("onAborting()"));
  if (m_ready) {
    vector<uint16_t> e;
    m_pDynamixel->getErrors(e);
    for (int i = 0; i < m_NUM_ACTUATOR; i++) {
      m_moving.data[i] = e[i];
    }
  }
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Dynamixel::onError(RTC::UniqueId ec_id)
{
  //要検討：エラー状態の時に何を送り続けるか？
  if (m_errorType == ETDxlError) {
    m_movingOut.write();
  }

  return RTC::RTC_OK;
}

RTC::ReturnCode_t Dynamixel::onReset(RTC::UniqueId ec_id)
{
  RTC_INFO(("onReset()"));

  if (m_errorType == ETDxlError) {
    try {
      m_pDynamixel->reset(m_initialSpeed, m_initialPosition);
    }
    catch (exception &e) {
      RTC_ERROR((e.what()));
    }
  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Dynamixel::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Dynamixel::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

template <typename T>
bool Dynamixel::stringToVector(std::string s, std::vector<T> &v)
{
  size_t n = v.size();
  if (n == 0) {
    return false;
  }
  istringstream iss(s);
  T a;
  bool read = true;
  for (size_t i = 0; i < v.size(); i++) {
    if (read) {
      T tmp;
      iss >> tmp;
      if (iss) {
        a = tmp;
      } else {
        if (i == 0) {
          return false;
        }
        read = false;
      }
    }
    v[i] = a;
  }
  return true;
}

extern "C"
{
 
  void DynamixelInit(RTC::Manager* manager)
  {
    coil::Properties profile(dynamixel_spec);
    manager->registerFactory(profile,
                             RTC::Create<Dynamixel>,
                             RTC::Delete<Dynamixel>);
  }
  
};


