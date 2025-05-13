sudo  bash -i -c "arduino-cli compile  --fqbn esp8266:esp8266:nodemcuv2 EspModbusRTU.ino && arduino-cli upload -p /dev/ttyAMA0  --fqbn esp8266:esp8266:nodemcuv2 EspModbusRTU.ino"
sudo arduino-cli monitor -p /dev/ttyAMA0 --config baudrate=115200
g++ -c ModbusRTUMaster.cc -o ModbusRTUMaster.o -lmodbus  && g++ main.cc ModbusRTUMaster.o -o modbus -lmodbus