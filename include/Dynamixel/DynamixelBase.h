#ifndef DYNAMIXELBASE_H
#define DYNAMIXELBASE_H

#include <dynamixel_sdk.h>
#include <string>
#include <vector>

enum ErrorType {
  ETNone = 0,
  ETPortHandler = 2, //PortHandler
  ETCommunication = 4, //dxl_comm_result
  ETDxlError = 8, //dxl_error
  ETAddParam = 16, //dxl_addparam_result
  ETOther = 32, //以外
};

class DynamixelBase {
public:
  DynamixelBase() {}
  DynamixelBase(std::string model, int numActuator, std::string devicename, int baudrate);
  ~DynamixelBase();
  void clearPort() { m_portHandler->clearPort(); }
  void reset(const std::vector<uint16_t> &speed, const std::vector<uint16_t> &position);
  ErrorType setSpeedAndPosition(const std::vector<uint16_t> &speed, const std::vector<uint16_t> &position);
  ErrorType setAllTorqueSmall();
  ErrorType setAllTorqueMax();
  ErrorType setAllTorqueOn();
  ErrorType setAllTorqueOff();
  ErrorType setPGain(const std::vector<uint16_t> &gain);
  ErrorType getPosition(std::vector<uint16_t> & position);
  ErrorType getMoving(std::vector<uint16_t> &moving);
  void getErrors(std::vector<uint16_t> &errors);

private:
  int m_NUM_ACTUATOR;
  std::vector<uint8_t> m_id;
  std::vector<uint8_t> m_pGainPrev;
  dynamixel::PortHandler *m_portHandler;
  dynamixel::PacketHandler *m_packetHandler;
  dynamixel::GroupSyncWrite *m_groupSyncWrite;
  dynamixel::GroupSyncRead *m_groupSyncReadPosition;
  dynamixel::GroupSyncRead *m_groupSyncReadMoving;
  ErrorType m_errorType;
  enum DynamixelType { AX12, XL320, XM430 };
  DynamixelType m_dtype;
  float m_PROTOCOL_VERSION;
  uint16_t m_ADDR_GOAL_POSITION;
  uint16_t m_GOAL_POSITION_MIN;
  uint16_t m_GOAL_POSITION_MAX;
  uint16_t m_MOVING_SPEED_MIN;
  uint16_t m_MOVING_SPEED_MAX;
  uint16_t m_ADDR_GROUP_SYNC_WRITE;
  uint16_t m_LEN_GROUP_SYNC_WRITE;
  uint16_t m_ADDR_TORQUE_LIMIT;
  uint16_t m_TORQUE_LIMIT_MIN;
  uint16_t m_TORQUE_LIMIT_MAX;
  uint16_t m_LEN_TORQUE_LIMIT;
  uint16_t m_ADDR_TORQUE_ENABLE;
  uint16_t m_ADDR_PGAIN;
  uint16_t m_PGAIN_MIN;
  uint16_t m_PGAIN_MAX;
  uint16_t m_CW_COMPLIANCE_SLOPE;
  uint16_t m_CCW_COMPLIANCE_SLOPE;
  uint16_t m_COMPLIANCE_SLOPE_MIN;
  uint16_t m_COMPLIANCE_SLOPE_MAX;
  uint16_t m_ADDR_MOVING_SPEED;
  uint16_t m_INIT_SPEED;
  uint16_t m_INIT_POSITION;
  uint16_t m_ADDR_PRESENT_POSITION;
  uint16_t m_LEN_PRESENT_POSITION;
  uint16_t m_ADDR_MOVING;
  uint16_t m_LEN_MOVING;
  uint8_t m_TORQUE_ON;
  uint8_t m_TORQUE_OFF;

  std::vector<uint16_t> m_errors;
  std::vector<uint16_t> m_zeros;

  ErrorType initSetSpeedAndPosition();
  ErrorType initGetPosition();
  ErrorType initGetMoving();
  ErrorType writeTxRxAll(int byte, uint16_t address, uint16_t data);

  template <typename T>
  void sat(T &x, T min, T max)
  {
    if (x > max) { x = max; } else if (x < min) { x = min; }
  }

};

#endif // CDYNAMIXELBASE_H
