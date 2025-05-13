import asyncio
from pymodbus.client import AsyncModbusTcpClient
import logging

# Configure logging
logging.basicConfig()
log = logging.getLogger()
log.setLevel(logging.DEBUG)

# Asynchronous Modbus client example
async def run_client():
    # Create an AsyncModbusTcpClient instance
    async with AsyncModbusTcpClient('localhost', port=5020) as client:

        # Reading holding registers (FC03)
        response = await client.read_holding_registers(0, 10)
        if response.isError():
            print("Error reading holding registers")
        else:
            print(f"Holding Registers: {response.registers}")

        # Writing to a coil (FC05)
        await client.write_coil(0, True)

        # Reading coils (FC01)
        response = await client.read_coils(0, 10)
        if response.isError():
            print("Error reading coils")
        else:
            print(f"Coils: {response.bits}")

# Run the client
if __name__ == "__main__":
    asyncio.run(run_client())

