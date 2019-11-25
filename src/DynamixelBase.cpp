#include "DynamixelBase.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <exception>
#include <stdexcept> // std::runtime_error
#include <thread>
#include <chrono>
using namespace std;

DynamixelBase::DynamixelBase(std::string model, int numActuator, std::string devicename, int baudrate)
{
  if (model == "AX12") {
    m_dtype = AX12;
  } else if (model == "XL320") {
    m_dtype = XL320;
  } else if (model == "XM430") {
    m_dtype = XM430;
  } else {
    throw invalid_argument((model + "には対応していません").c_str());
  }

  if (m_dtype == AX12) {
    m_PROTOCOL_VERSION = 1.0;
    m_ADDR_GOAL_POSITION = 30;
    m_GOAL_POSITION_MIN = 0;
    m_GOAL_POSITION_MAX = 1023;
    m_ADDR_MOVING_SPEED = 32;
    m_MOVING_SPEED_MIN = 0;
    m_MOVING_SPEED_MAX = 1023; //Joint Mode
    m_ADDR_GROUP_SYNC_WRITE = m_ADDR_GOAL_POSITION;
    m_LEN_GROUP_SYNC_WRITE = 4;
    m_ADDR_TORQUE_LIMIT = 34;
    m_TORQUE_LIMIT_MIN = 0;
    m_TORQUE_LIMIT_MAX = 1023;
    m_ADDR_TORQUE_ENABLE = 24;
    m_ADDR_PGAIN = -1;
    m_PGAIN_MIN = -1;
    m_PGAIN_MAX = -1;
    m_CW_COMPLIANCE_SLOPE = 28;
    m_CCW_COMPLIANCE_SLOPE = 29;
    m_COMPLIANCE_SLOPE_MIN = 0;
    m_COMPLIANCE_SLOPE_MAX = 254;
    m_ADDR_PRESENT_POSITION = 36;
    m_LEN_PRESENT_POSITION = 2;
    m_ADDR_MOVING = 46;
    m_LEN_MOVING = 1;
  } else if (m_dtype == XL320) {
    m_PROTOCOL_VERSION = 2.0;
    m_ADDR_GOAL_POSITION = 30;
    m_GOAL_POSITION_MIN = 0;
    m_GOAL_POSITION_MAX = 1023;
    m_ADDR_MOVING_SPEED = 32;
    m_MOVING_SPEED_MIN = 0;
    m_MOVING_SPEED_MAX = 247;
    m_ADDR_GROUP_SYNC_WRITE = m_ADDR_GOAL_POSITION;
    m_LEN_GROUP_SYNC_WRITE = 4;
    m_ADDR_TORQUE_LIMIT = 35;
    m_TORQUE_LIMIT_MIN = 0;
    m_TORQUE_LIMIT_MAX = 1023;
    m_ADDR_TORQUE_ENABLE = 24;
    m_ADDR_PGAIN = 29;
    m_PGAIN_MIN = 0;
    m_PGAIN_MAX = 254;
    m_CW_COMPLIANCE_SLOPE = -1;
    m_CCW_COMPLIANCE_SLOPE = -1;
    m_COMPLIANCE_SLOPE_MIN = -1;
    m_COMPLIANCE_SLOPE_MAX = -1;
    m_ADDR_PRESENT_POSITION = 37;
    m_LEN_PRESENT_POSITION = 2;
    m_ADDR_MOVING = 49;
    m_LEN_MOVING = 1;
  } else if (m_dtype == XM430) {
    m_PROTOCOL_VERSION = 2.0;
    m_ADDR_GOAL_POSITION = 116;
    m_GOAL_POSITION_MIN = 0;
    m_GOAL_POSITION_MAX = 4095;
    m_ADDR_MOVING_SPEED = 112;  //Profile Velocity
    m_MOVING_SPEED_MIN = 0; //Profile Velocity
    m_MOVING_SPEED_MAX = 32767; //Profile Velocity
    m_ADDR_GROUP_SYNC_WRITE = m_ADDR_MOVING_SPEED;
    m_LEN_GROUP_SYNC_WRITE = 8;
    m_ADDR_TORQUE_LIMIT = -1; //XM430では不使用
    m_TORQUE_LIMIT_MIN = -1; //XM430では不使用
    m_TORQUE_LIMIT_MAX = -1; //XM430では不使用
    m_ADDR_TORQUE_ENABLE = 64;
    m_ADDR_PGAIN = 84; //Position P Gain
    m_PGAIN_MIN = 0; //Position P Gain
    m_PGAIN_MAX = 16383; //Position P Gain
    m_CW_COMPLIANCE_SLOPE = -1;
    m_CCW_COMPLIANCE_SLOPE = -1;
    m_COMPLIANCE_SLOPE_MIN = -1;
    m_COMPLIANCE_SLOPE_MAX = -1;
    m_ADDR_PRESENT_POSITION = 132;
    m_LEN_PRESENT_POSITION = 4;
    m_ADDR_MOVING = 122;
    m_LEN_MOVING = 1;
  }
  m_TORQUE_ON = 1;
  m_TORQUE_OFF = 0;

  if (numActuator < 1) {
    throw invalid_argument("numActuatorが小さすぎます");
  }
  m_NUM_ACTUATOR = numActuator;

  m_portHandler = nullptr;
  m_packetHandler = nullptr;
  m_groupSyncWrite = nullptr;
  m_groupSyncReadPosition = nullptr;
  m_groupSyncReadMoving = nullptr;

  // PortHandlerインスタンスを初期化（シリアルポートのデバイス名を指定）
  if (m_portHandler == nullptr) {
    m_portHandler = dynamixel::PortHandler::getPortHandler(devicename.c_str());
  }

  // PacketHandlerインスタンスを初期化（プロトコルのバージョンを指定）
  if (m_packetHandler == nullptr) {
    m_packetHandler = dynamixel::PacketHandler::getPacketHandler(m_PROTOCOL_VERSION);
  }

  // シリアルポートをオープン
  if (!m_portHandler->openPort()) {
    throw runtime_error("シリアルポートのオープンに失敗");
  }

  // ボーレートを設定
  if (!m_portHandler->setBaudRate(baudrate)) {
    throw runtime_error("ボーレートの変更に失敗");
  }

  // サーボのIDとの対応関係
  m_id.resize(m_NUM_ACTUATOR);
  for (int i = 0; i < m_NUM_ACTUATOR; i++) {
    m_id[i] = i + 1;
  }

  if (initSetSpeedAndPosition() != ETNone) {
    throw runtime_error("initSetSpeedAndPosition()に失敗");
  }

  if (initGetPosition() != ETNone) {
    throw runtime_error("initGetPosition()に失敗");
  }

  if (initGetMoving() != ETNone) {
    throw runtime_error("initGetMoving()に失敗");
  }

  // Pゲインを保持するvectorの初期化
  m_pGainPrev.resize(m_NUM_ACTUATOR);
  for (int i = 0; i < m_NUM_ACTUATOR; i++) {
    m_pGainPrev[i] = 0;
  }

  m_zeros.resize(m_NUM_ACTUATOR);
  for (int i = 0; i < m_NUM_ACTUATOR; i++) {
    m_zeros[i] = 0;
  }
  m_errors = m_zeros;
}

DynamixelBase::~DynamixelBase()
{
  if (m_groupSyncWrite != nullptr) {
    delete m_groupSyncWrite;
  }
  if (m_groupSyncReadPosition != nullptr) {
    delete m_groupSyncReadPosition;
  }
  if (m_groupSyncReadMoving != nullptr) {
    delete m_groupSyncReadMoving;
  }
  if (m_portHandler != nullptr) {
    m_portHandler->closePort();
  }
}

void DynamixelBase::reset(const std::vector<uint16_t> &speed, const std::vector<uint16_t> &position)
{
  if (m_dtype == AX12) {
    while (true) {
      m_errorType = setAllTorqueSmall();
#if 1
      cerr << "全アクチュエータのトルク制限を10%に設定" << endl;
#endif
      if (m_errorType == ETNone) {
        break;
      } else {
#if 1
        cerr << "m_errorType: " << m_errorType << endl;
#endif
        if (m_errorType != ETDxlError) {
          throw runtime_error("Dynamixel以外のエラー");
        }
        //ETDxlErrorの場合は再試行
      }
      this_thread::sleep_for(chrono::milliseconds(1000)); //1秒停止
    }
  } else if (m_dtype == XM430 || m_dtype == XL320) {
    bool commError = false;
    bool dxlError = false;
    for (int i = 0; i < m_NUM_ACTUATOR; i++) {
      uint8_t dxl_error = 0;
      int dxl_comm_result
        = m_packetHandler->reboot(m_portHandler, m_id[i], &dxl_error);
      if (dxl_comm_result != COMM_SUCCESS) {
#if 1
        cerr << int(m_id[i]) << ": " << m_packetHandler->getTxRxResult(dxl_comm_result) << endl;
#endif
        m_errors[i] |= ETCommunication;
        commError = true;
      } else if (dxl_error != 0) {
#if 1
        cerr << int(m_id[i]) << ": " << m_packetHandler->getRxPacketError(dxl_error) << endl;
#endif
        m_errors[i] |= ETDxlError;
        dxlError = true;
      }
    }
    if (commError) {
      throw runtime_error("通信エラーでリブート失敗");
    }
    if (dxlError) {
      throw runtime_error("Dynamixelエラーでリブート失敗");
    }
  }

  //オーバロードエラーが起きた場合を想定し，その状態から抜けるために目標を初期位置に設定
  m_errorType = setSpeedAndPosition(speed, position);
  if (m_errorType != ETNone) {
    throw runtime_error("初期位置・速度設定でエラー");
  }

  //全てのダイナミクセルのトルクをオフにする
  m_errorType = setAllTorqueOff();
  if (m_errorType != ETNone) {
    throw runtime_error("トルクオフでエラー");
  }
}

ErrorType DynamixelBase::setSpeedAndPosition(const std::vector<uint16_t> &speed, const std::vector<uint16_t> &position)
{
  m_errors = m_zeros;
  // sync writeのパラメータ設定
  m_groupSyncWrite->clearParam();
  for (int i = 0; i < m_NUM_ACTUATOR; i++) {
    uint16_t p = position[i];
    sat(p, m_GOAL_POSITION_MIN, m_GOAL_POSITION_MAX);
    uint16_t  s = speed[i];
    sat(s, m_MOVING_SPEED_MIN, m_MOVING_SPEED_MAX);
    uint8_t param[8];
    if (m_dtype == AX12 || m_dtype == XL320) {
      param[0] = DXL_LOBYTE(p);
      param[1] = DXL_HIBYTE(p);
      param[2] = DXL_LOBYTE(s);
      param[3] = DXL_HIBYTE(s);
    } else if (m_dtype == XM430) {
      param[0] = DXL_LOBYTE(DXL_LOWORD(s));
      param[1] = DXL_HIBYTE(DXL_LOWORD(s));
      param[2] = DXL_LOBYTE(DXL_HIWORD(s));
      param[3] = DXL_HIBYTE(DXL_HIWORD(s));
      param[4] = DXL_LOBYTE(DXL_LOWORD(p));
      param[5] = DXL_HIBYTE(DXL_LOWORD(p));
      param[6] = DXL_LOBYTE(DXL_HIWORD(p));
      param[7] = DXL_HIBYTE(DXL_HIWORD(p));
    }
    bool dxl_addparam_result
      = m_groupSyncWrite->addParam(m_id[i], param);
    if (dxl_addparam_result != true) {
#if 1
      cerr << int(m_id[i]) << ": m_groupSyncWrite->addparam() 失敗" << endl;
#endif
      m_errors[i] |= ETAddParam;
      return ETAddParam;
    }
  }

  // sync write
  int count;
  const int countMax = 10;
  for (count = 1; count <= countMax; count++) {
    int dxl_comm_result = m_groupSyncWrite->txPacket();
    if (dxl_comm_result == COMM_SUCCESS) break;
#if 1
    cerr << count << ":  m_groupSyncWrite->txPacket()"
      << m_packetHandler->getTxRxResult(dxl_comm_result) << endl;
#endif
    this_thread::sleep_for(chrono::milliseconds(100)); //0.1秒停止
  }
  if (count > countMax) {
    return ETCommunication;
  }
#if 0
  RTC_INFO(("m_groupSyncWrite->txPacket()成功"));
#endif
  return ETNone;
}

ErrorType DynamixelBase::setAllTorqueSmall()
{
  if (m_dtype == XM430) {
    return ETNone;
  }
  return writeTxRxAll(2, m_ADDR_TORQUE_LIMIT, m_TORQUE_LIMIT_MAX / 10);//10%に設定
}

ErrorType DynamixelBase::setAllTorqueMax()
{
  if (m_dtype == XM430) {
    return ETNone;
  }
  return writeTxRxAll(2, m_ADDR_TORQUE_LIMIT, m_TORQUE_LIMIT_MAX);
}

ErrorType DynamixelBase::setAllTorqueOn()
{
  return writeTxRxAll(1, m_ADDR_TORQUE_ENABLE, m_TORQUE_ON);
}

ErrorType DynamixelBase::setAllTorqueOff()
{
  return writeTxRxAll(1, m_ADDR_TORQUE_ENABLE, m_TORQUE_OFF);
}

ErrorType DynamixelBase::setPGain(const std::vector<uint16_t> &gain)
{
  //TODO: エラーチェックを追加
  for (int i = 0; i < m_NUM_ACTUATOR; i++) {
    if (gain[i] != m_pGainPrev[i]) {
      //前回と値が違えば変更
      if (gain[i] == 0) {
        //0ならば特別にトルクオフと意味付け
#if 1
        cerr << int(m_id[i]) << ": トルクオフ" << endl;
#endif
        m_packetHandler->write1ByteTxRx(m_portHandler, m_id[i], m_ADDR_TORQUE_ENABLE, m_TORQUE_OFF, 0);
      } else {
        if (m_pGainPrev[i] == 0) {
          //前回トルクオフならばトルクオンにする
          m_packetHandler->write1ByteTxRx(m_portHandler, m_id[i], m_ADDR_TORQUE_ENABLE, m_TORQUE_ON, 0);
        }
        if (m_dtype == AX12) {
          //Compliance Slopeは値が大きいほど柔らかくなるので，0～254を254～0に反転する．
          uint8_t slope = 254 - gain[i];
          sat(slope, uint8_t(m_COMPLIANCE_SLOPE_MIN), uint8_t(m_COMPLIANCE_SLOPE_MAX));
          m_packetHandler->write1ByteTxRx(m_portHandler, m_id[i], m_CW_COMPLIANCE_SLOPE, slope, 0);
          m_packetHandler->write1ByteTxRx(m_portHandler, m_id[i], m_CCW_COMPLIANCE_SLOPE, slope, 0);
        } else if (m_dtype == XL320) {
          uint8_t g = gain[i];
          sat(g, uint8_t(m_PGAIN_MIN), uint8_t(m_PGAIN_MAX));
          m_packetHandler->write1ByteTxRx(m_portHandler, m_id[i], m_ADDR_PGAIN, g, 0);
        } else if (m_dtype == XM430) {
          uint16_t g = gain[i];
          sat(g, m_PGAIN_MIN, m_PGAIN_MAX);
          m_packetHandler->write2ByteTxRx(m_portHandler, m_id[i], m_ADDR_PGAIN, g, 0);
        }
      }
    }
    m_pGainPrev[i] = gain[i];
  }
  return ETNone;
}

ErrorType DynamixelBase::getPosition(std::vector<uint16_t> & position)
{
  m_errors = m_zeros;
  position.clear();
  bool commError = false;
  bool dxlError = false;
  if (m_dtype == AX12) {
    for (int i = 0; i < m_NUM_ACTUATOR; i++) {
      uint16_t present_position;
      uint8_t dxl_error = 0;
      int dxl_comm_result
        = m_packetHandler->read2ByteTxRx(m_portHandler, m_id[i],
          m_ADDR_PRESENT_POSITION, &present_position, &dxl_error);
      if (dxl_comm_result != COMM_SUCCESS) {
#if 1
        cerr << int(m_id[i]) << ": " << m_packetHandler->getTxRxResult(dxl_comm_result) << endl;
#endif
        m_errors[i] |= ETCommunication;
        commError = true;
      } else if (dxl_error != 0) {
#if 1
        cerr << int(m_id[i]) << ": " << m_packetHandler->getRxPacketError(dxl_error) << endl;
#endif
        m_errors[i] |= ETDxlError;
        dxlError = true;
      } else {
        position.push_back(present_position);
      }
    }
  } else if (m_dtype == XL320 || m_dtype == XM430) {
    int dxl_comm_result = m_groupSyncReadPosition->txRxPacket();
    if (dxl_comm_result != COMM_SUCCESS) {
#if 1
      cerr << "m_groupSyncReadPosition->txRxPacket(): " << m_packetHandler->getTxRxResult(dxl_comm_result) << endl;
#endif
      commError = true;
    } else {
      for (int i = 0; i < m_NUM_ACTUATOR; i++) {
        uint8_t dxl_error = 0;
        if (m_groupSyncReadPosition->getError(m_id[i], &dxl_error)) {
#if 1
          cerr << int(m_id[i]) << ": " << m_packetHandler->getRxPacketError(dxl_error) << endl;
#endif
          dxlError = true;
        } else {
          bool result = m_groupSyncReadPosition->isAvailable(m_id[i], m_ADDR_PRESENT_POSITION, m_LEN_PRESENT_POSITION);
          if (!result) {
#if 1
            cerr << int(m_id[i]) << ": m_groupSyncReadPosition->isAvailable() 失敗" << endl;
#endif
            dxlError = true;
          } else {
            uint16_t present_position
              = m_groupSyncReadPosition->getData(m_id[i], m_ADDR_PRESENT_POSITION, m_LEN_PRESENT_POSITION);
            position.push_back(present_position);
          }
        }
      }
    }
  }
  if (commError) {
    return ETCommunication;
  }
  if (dxlError) {
    return ETDxlError;
  }
  return ETNone;
}

ErrorType DynamixelBase::getMoving(std::vector<uint16_t> &moving)
{
  m_errors = m_zeros;
  moving.clear();
  bool commError = false;
  bool dxlError = false;
  if (m_dtype == AX12) {
    for (int i = 0; i < m_NUM_ACTUATOR; i++) {
      uint8_t m;
      uint8_t dxl_error = 0;
      int dxl_comm_result
        = m_packetHandler->read1ByteTxRx(m_portHandler, m_id[i], m_ADDR_MOVING, &m, &dxl_error);
      if (dxl_comm_result != COMM_SUCCESS) {
#if 1
        cerr << int(m_id[i]) << ": " << m_packetHandler->getTxRxResult(dxl_comm_result) << endl;
#endif
        m_errors[i] |= ETCommunication;
        commError = true;
      } else if (dxl_error != 0) {
#if 1
        cerr << int(m_id[i]) << ": " << m_packetHandler->getRxPacketError(dxl_error) << endl;
#endif
        m_errors[i] |= ETDxlError;
        dxlError = true;
      } else {
        moving.push_back(m);
      }
    }
  } else if (m_dtype == XL320 || m_dtype == XM430) {
    int dxl_comm_result = m_groupSyncReadMoving->txRxPacket();
    if (dxl_comm_result != COMM_SUCCESS) {
#if 1
      cerr << "m_groupSyncReadMoving->txRxPacket(): " << m_packetHandler->getTxRxResult(dxl_comm_result) << endl;
#endif
      commError = true;
    } else {
      for (int i = 0; i < m_NUM_ACTUATOR; i++) {
        uint8_t dxl_error = 0;
        uint8_t m;
        if (m_groupSyncReadMoving->getError(m_id[i], &dxl_error)) {
#if 1
          cerr << int(m_id[i]) << ": " << m_packetHandler->getRxPacketError(dxl_error) << endl;
#endif
          dxlError = true;
          m = 3;
        } else {
          bool result = m_groupSyncReadMoving->isAvailable(m_id[i], m_ADDR_MOVING, m_LEN_MOVING);
          if (!result) {
#if 1
            cerr << int(m_id[i]) << ": m_groupSyncReadMoving->isAvailable() 失敗" << endl;
#endif
            dxlError = true;
            m = 3;
          } else {
            m = m_groupSyncReadMoving->getData(m_id[i], m_ADDR_MOVING, m_LEN_MOVING);
          }
        }
        moving.push_back(m);
      }
    }
  }

  if (commError) {
    return ETCommunication;
  }
  if (dxlError) {
    return ETDxlError;
  }
  return ETNone;
}

ErrorType DynamixelBase::initSetSpeedAndPosition()
{
  if (m_groupSyncWrite == nullptr) {
    m_groupSyncWrite =
      new dynamixel::GroupSyncWrite(m_portHandler, m_packetHandler,
        m_ADDR_GROUP_SYNC_WRITE, m_LEN_GROUP_SYNC_WRITE);
  }
  return ETNone;
}

ErrorType DynamixelBase::initGetPosition()
{
  m_errors = m_zeros;
  if (m_dtype == XL320 || m_dtype == XM430) {
    //プロトコル2.0ではGroupSyncReadが使える
    if (m_groupSyncReadPosition == nullptr) {
      m_groupSyncReadPosition =
        new dynamixel::GroupSyncRead(m_portHandler, m_packetHandler,
          m_ADDR_PRESENT_POSITION, m_LEN_PRESENT_POSITION);
    }
    for (int i = 0; i < m_NUM_ACTUATOR; i++) {
      if (!m_groupSyncReadPosition->addParam(m_id[i])) {
#if 1
        cerr << int(m_id[i]) << ": groupSyncReadPosition addparam 失敗" << endl;
#endif
        m_errors[i] |= ETAddParam;
        return ETAddParam;
      }
    }
  }
  return ETNone;
}

ErrorType DynamixelBase::initGetMoving()
{
  m_errors = m_zeros;
  if (m_dtype == XL320 || m_dtype == XM430) {
    //プロトコル2.0ではGroupSyncReadが使える
    if (m_groupSyncReadMoving == nullptr) {
      m_groupSyncReadMoving =
        new dynamixel::GroupSyncRead(m_portHandler, m_packetHandler,
          m_ADDR_MOVING, m_LEN_MOVING);
    }
    for (int i = 0; i < m_NUM_ACTUATOR; i++) {
      if (!m_groupSyncReadMoving->addParam(m_id[i])) {
#if 1
        cerr << int(m_id[i]) << ": groupSyncReadMoving addparam 失敗" << endl;
#endif
        m_errors[i] |= ETAddParam;
        return ETAddParam;
      }
    }
  }
  return ETNone;
}

void DynamixelBase::getErrors(std::vector<uint16_t> &errors)
{
  errors = m_errors;
}

//byte=4は必要？
ErrorType DynamixelBase::writeTxRxAll(int byte, uint16_t address, uint16_t data)
{
  m_errors = m_zeros;
  bool commError = false;
  bool dxlError = false;
  for (int i = 0; i<m_NUM_ACTUATOR; i++) {
    uint8_t dxl_error;
    int dxl_comm_result;
    if (byte == 1) {
      uint8_t d = data;
      dxl_comm_result =
        m_packetHandler->write1ByteTxRx(m_portHandler, m_id[i], address, d, &dxl_error);
    } else if (byte == 2) {
      dxl_comm_result = 
        m_packetHandler->write2ByteTxRx(m_portHandler, m_id[i], address, data, &dxl_error);
    } else {
#if 1
      cerr << int(m_id[i]) << ": byte " << byte << " は想定外" << endl;
#endif
      return ETOther;
    }
    if (dxl_comm_result != COMM_SUCCESS) {
#if 1
      cerr << int(m_id[i]) << ": " << m_packetHandler->getTxRxResult(dxl_comm_result) << endl;
#endif
      m_errors[i] |= ETCommunication;
      commError = true;
    } else if (dxl_error != 0) {
#if 1
      cerr << int(m_id[i]) << ": " << m_packetHandler->getRxPacketError(dxl_error) << endl;
#endif
      m_errors[i] |= ETDxlError;
      dxlError = true;
    } else {
#if 0
      cerr << int(m_id[i]) "%d: 成功", int(m_id[i])));
#endif
    }
  }
  if (commError) {
    return ETCommunication;
  }
  if (dxlError) {
    return ETDxlError;
  }
  return ETNone;
}
