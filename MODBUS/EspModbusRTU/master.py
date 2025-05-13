from pymodbus.client import ModbusSerialClient
import time

# Modbus RTU Client Setup
client = ModbusSerialClient(
    # method='rtu',
    port='/dev/ttyAMA2',  # UART2 on RPi5
    baudrate=115200,
    timeout=1
)

# ESP8266 Slave ID
SLAVE_ID = 1

# Register Addresses (from ESP code)
TEMP_REG = 100  # Temperature (0.1°C)
HUM_REG  = 101  # Humidity (0.1%)

def read_modbus():
    try:
        # Connect to Modbus slave
        if not client.connect():
            print("Failed to connect to Modbus slave")
            return

        # Read temperature (1 register, address 100)
        temp = client.read_holding_registers(address=TEMP_REG, count=1, slave=SLAVE_ID)
        # Read humidity (1 register, address 101)
        hum = client.read_holding_registers(address=HUM_REG, count=1, slave=SLAVE_ID)

        if not temp.isError() and not hum.isError():
            temperature = temp.registers[0] / 10.0  # Convert 0.1°C to °C
            humidity = hum.registers[0] / 10.0      # Convert 0.1% to %
            print(f"Temperature: {temperature}°C, Humidity: {humidity}%")
        else:
            print("Modbus read error")

    except Exception as e:
        print(f"Error: {e}")
    finally:
        client.close()

if __name__ == "__main__":
    while True:
        read_modbus()
        time.sleep(2)  # Poll every 2 seconds