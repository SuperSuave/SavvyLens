import { DBCMessage, CANFrame, ScriptItem, ConnectionConfig } from '../types';

export const INITIAL_CONNECTIONS: ConnectionConfig[] = [
  {
    id: 'conn-1',
    name: 'Leaf EV Powertrain Bus (GVRET USB)',
    type: 'GVRET',
    status: 'Connected',
    baudRate: 500000,
    port: '/dev/ttyACM0',
    isLogging: true,
  },
  {
    id: 'conn-2',
    name: 'Comfort Bus (SocketCAN can0)',
    type: 'SocketCAN',
    status: 'Disconnected',
    baudRate: 250000,
    port: 'can0',
    isLogging: false,
  },
];

export const INITIAL_DBC_MESSAGES: DBCMessage[] = [
  {
    id: 291,
    hexId: '0x123',
    name: 'EV_Inverter_Status',
    dlc: 8,
    sender: 'VCM',
    signals: [
      { name: 'MotorSpeedRPM', startBit: 0, length: 16, isBigEndian: false, isSigned: true, factor: 1, offset: -2000, min: -2000, max: 8000, unit: 'RPM', receiver: ['Display'] },
      { name: 'InverterTemp', startBit: 16, length: 8, isBigEndian: false, isSigned: false, factor: 1, offset: -40, min: -40, max: 210, unit: '°C', receiver: ['VCM'] },
      { name: 'TorqueCommand', startBit: 24, length: 12, isBigEndian: false, isSigned: true, factor: 0.5, offset: -500, min: -500, max: 1500, unit: 'Nm', receiver: ['Inverter'] }
    ]
  },
  {
    id: 516,
    hexId: '0x204',
    name: 'BMS_Cell_Summary',
    dlc: 8,
    sender: 'BMS',
    signals: [
      { name: 'PackVoltage', startBit: 0, length: 16, isBigEndian: false, isSigned: false, factor: 0.1, offset: 0, min: 0, max: 500, unit: 'V', receiver: ['VCM'] },
      { name: 'PackCurrent', startBit: 16, length: 16, isBigEndian: false, isSigned: true, factor: 0.1, offset: -1000, min: -1000, max: 1000, unit: 'A', receiver: ['VCM'] },
      { name: 'StateOfCharge', startBit: 32, length: 8, isBigEndian: false, isSigned: false, factor: 0.5, offset: 0, min: 0, max: 100, unit: '%', receiver: ['Display'] }
    ]
  },
  {
    id: 792,
    hexId: '0x318',
    name: 'BCM_Door_Status',
    dlc: 4,
    sender: 'BCM',
    signals: [
      { name: 'DriverDoorOpen', startBit: 0, length: 1, isBigEndian: false, isSigned: false, factor: 1, offset: 0, min: 0, max: 1, unit: 'bool', receiver: ['Cluster'] },
      { name: 'PassengerDoorOpen', startBit: 1, length: 1, isBigEndian: false, isSigned: false, factor: 1, offset: 0, min: 0, max: 1, unit: 'bool', receiver: ['Cluster'] },
      { name: 'HeadlightsActive', startBit: 4, length: 2, isBigEndian: false, isSigned: false, factor: 1, offset: 0, min: 0, max: 3, unit: 'state', receiver: ['Body'] }
    ]
  }
];

export const INITIAL_SCRIPTS: ScriptItem[] = [
  {
    id: 'script-1',
    name: 'Burst Repeat (Send 0x123 every 10ms)',
    active: false,
    description: 'Repeatedly sends a custom CAN frame at high frequency for fuzzing or ECU simulation.',
    code: `// SavvyLens JS Automation Script
function setup() {
  log("Starting Burst Repeat script...");
}

function loop() {
  // Send ID 0x123 with payload [0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04]
  sendCanFrame("0x123", [0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04], 0);
  sleep(10);
}`
  },
  {
    id: 'script-2',
    name: 'Byte Sweep Fuzzing',
    active: false,
    description: 'Iterates through all 256 values on byte 0 of ID 0x300 to discover diagnostic responses.',
    code: `// Byte Sweep Fuzzer
let val = 0;
function loop() {
  sendCanFrame("0x300", [val, 0x00, 0x55, 0xAA, 0, 0, 0, 0], 0);
  val = (val + 1) % 256;
  sleep(50);
}`
  }
];

export const MOCK_FRAMES: CANFrame[] = [
  { id: '0x123', decimalId: 291, name: 'EV_Inverter_Status', timestamp: 100.123, bus: 0, dlc: 8, data: [0x48, 0x0D, 0x3C, 0x01, 0xF4, 0x01, 0x50, 0x00], ascii: 'H.<?....', count: 142, direction: 'RX' },
  { id: '0x204', decimalId: 516, name: 'BMS_Cell_Summary', timestamp: 100.135, bus: 0, dlc: 8, data: [0xD0, 0x07, 0x88, 0x13, 0xB4, 0x00, 0x00, 0x00], ascii: '....4...', count: 95, direction: 'RX' },
  { id: '0x318', decimalId: 792, name: 'BCM_Door_Status', timestamp: 100.180, bus: 1, dlc: 4, data: [0x01, 0x00, 0x02, 0xFF], ascii: '....', count: 22, direction: 'RX' },
  { id: '0x450', decimalId: 1104, name: 'ABS_Wheel_Speeds', timestamp: 100.192, bus: 0, dlc: 8, data: [0x50, 0xC3, 0x4F, 0xC3, 0x51, 0xC3, 0x4E, 0xC3], ascii: 'P.O.Q.N.', count: 310, direction: 'RX' },
  { id: '0x7E0', decimalId: 2016, name: 'UDS_Request_Diag', timestamp: 100.250, bus: 0, dlc: 8, data: [0x02, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00], ascii: '........', count: 5, direction: 'TX' },
  { id: '0x7E8', decimalId: 2024, name: 'UDS_Response_Diag', timestamp: 100.275, bus: 0, dlc: 8, data: [0x06, 0x50, 0x03, 0x00, 0x12, 0x34, 0x56, 0x00], ascii: '.P..4V.', count: 5, direction: 'RX' },
];
