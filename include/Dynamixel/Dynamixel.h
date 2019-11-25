// -*- C++ -*-
/*!
 * @file  Dynamixel.h
 * @brief Component for Multiple Dynamixels
 * @date  $Date$
 *
 * $Id$
 */

#ifndef DYNAMIXEL_H
#define DYNAMIXEL_H

#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/idl/ExtendedDataTypesSkel.h>
#include <rtm/idl/InterfaceDataTypesSkel.h>

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">
#include "BasicDataTypeStub.h"

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="port_stub_h">
// </rtc-template>

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>

#include "DynamixelBase.h"

using namespace RTC;

/*!
 * @class Dynamixel
 * @brief Component for Multiple Dynamixels
 *
 */
class Dynamixel
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  Dynamixel(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~Dynamixel();

  // <rtc-template block="public_attribute">
  
  // </rtc-template>

  // <rtc-template block="public_operation">
  
  // </rtc-template>

  /***
   *
   * The initialize action (on CREATED->ALIVE transition)
   * formaer rtc_init_entry() 
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onInitialize();

  /***
   *
   * The finalize action (on ALIVE->END transition)
   * formaer rtc_exiting_entry()
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onFinalize();

  /***
   *
   * The startup action when ExecutionContext startup
   * former rtc_starting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id);

  /***
   *
   * The shutdown action when ExecutionContext stop
   * former rtc_stopping_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id);

  /***
   *
   * The activated action (Active state entry action)
   * former rtc_active_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);

  /***
   *
   * The deactivated action (Active state exit action)
   * former rtc_active_exit()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

  /***
   *
   * The execution action that is invoked periodically
   * former rtc_active_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

  /***
   *
   * The aborting action when main logic error occurred.
   * former rtc_aborting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id);

  /***
   *
   * The error action in ERROR state
   * former rtc_error_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onError(RTC::UniqueId ec_id);

  /***
   *
   * The reset action that is invoked resetting
   * This is same but different the former rtc_init_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
   virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id);
  
  /***
   *
   * The state update action that is invoked after onExecute() action
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id);

  /***
   *
   * The action that is invoked when execution context's rate is changed
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   * 
   * 
   */
  // virtual RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id);


 protected:
  // <rtc-template block="protected_attribute">
  
  // </rtc-template>

  // <rtc-template block="protected_operation">
  
  // </rtc-template>

  // Configuration variable declaration
  // <rtc-template block="config_declare">
  /*!
   * 
   * - Name:  NUM_ACTUATOR
   * - DefaultValue: 5
   */
  short int m_NUM_ACTUATOR;
  /*!
   * 
   * - Name:  DEVICENAME
   * - DefaultValue: COM9
   */
  std::string m_DEVICENAME;
  /*!
   * 
   * - Name:  BAUDRATE
   * - DefaultValue: 1000000
   */
  int m_BAUDRATE;
  /*!
   * 
   * - Name:  MODEL
   * - DefaultValue: AX12
   */
  std::string m_MODEL;
  /*!
   * 
   * - Name:  initialPositionString
   * - DefaultValue: 512
   */
  std::string m_initialPositionString;
  /*!
   * 
   * - Name:  initialSpeedString
   * - DefaultValue: 30
   */
  std::string m_initialSpeedString;

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  RTC::TimedUShortSeq m_goalPosition;
  /*!
   */
  InPort<RTC::TimedUShortSeq> m_goalPositionIn;
  RTC::TimedUShortSeq m_movingSpeed;
  /*!
   */
  InPort<RTC::TimedUShortSeq> m_movingSpeedIn;
  RTC::TimedUShortSeq m_pGain;
  /*!
   */
  InPort<RTC::TimedUShortSeq> m_pGainIn;
  
  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  RTC::TimedUShortSeq m_presentPosition;
  /*!
   */
  OutPort<RTC::TimedUShortSeq> m_presentPositionOut;
  RTC::TimedUShortSeq m_moving;
  /*!
   */
  OutPort<RTC::TimedUShortSeq> m_movingOut;
  
  // </rtc-template>

  // CORBA Port declaration
  // <rtc-template block="corbaport_declare">
  
  // </rtc-template>

  // Service declaration
  // <rtc-template block="service_declare">
  
  // </rtc-template>

  // Consumer declaration
  // <rtc-template block="consumer_declare">
  
  // </rtc-template>

 private:
  // <rtc-template block="private_attribute">
  
  // </rtc-template>

  // <rtc-template block="private_operation">
  
  // </rtc-template>

   DynamixelBase *m_pDynamixel;

   ErrorType m_errorType;

   bool m_ready;

   std::vector<uint16_t> m_initialSpeed;
   std::vector<uint16_t> m_initialPosition;
   std::vector<uint16_t> m_speed;

   int m_countCommunicationError;

   template <typename T>
   bool stringToVector(std::string s, std::vector<T> &v);

   bool m_rtcError;  //OpenRTM-aist-1.2.0のバグ回避
};


extern "C"
{
  DLL_EXPORT void DynamixelInit(RTC::Manager* manager);
};

#endif // DYNAMIXEL_H
