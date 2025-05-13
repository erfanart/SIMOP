import asyncio
from pymodbus.server.async_io import ModbusTcpServer
from pymodbus.datastore import ModbusServerContext, ModbusSlaveContext, ModbusSequentialDataBlock
from pymodbus.device import ModbusDeviceIdentification
import logging

# Configure logging
logging.basicConfig()
log = logging.getLogger()
log.setLevel(logging.DEBUG)

# Define Modbus server datastore (slave)
store = ModbusSlaveContext(
    di=ModbusSequentialDataBlock(0, [21] * 100),  # Discrete Inputs
    co=ModbusSequentialDataBlock(0, [19] * 100),  # Coils
    hr=ModbusSequentialDataBlock(0, [25] * 100),  # Holding Registers
    ir=ModbusSequentialDataBlock(0, [10] * 100)   # Input Registers
)

# Define server context (single slave)
context = ModbusServerContext(slaves=store, single=True)

# Define device identity
identity = ModbusDeviceIdentification()
identity.VendorName = 'PyModbus'
identity.ProductCode = 'PM'
identity.VendorUrl = 'http://github.com/bashwork/pymodbus/'
identity.ProductName = 'PyModbus Server'
identity.ModelName = 'PyModbus Server'
identity.MajorMinorRevision = '1.0'

# Start Modbus TCP server asynchronously
async def run_server():
    server = ModbusTcpServer(context, identity=identity, address=("0.0.0.0", 5020))
    await server.serve_forever()  # Start serving

if __name__ == "__main__":
    asyncio.run(run_server())

