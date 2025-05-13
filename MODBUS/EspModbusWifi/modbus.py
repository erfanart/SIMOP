from datetime import datetime
import asyncio
from pymodbus.client import AsyncModbusTcpClient
from elasticsearch import Elasticsearch

# Replace with your actual API key
api_key = "NHp1akxwSUJFYXFLa1JaQ3hpQkU6OTc1b2dfcWdSRC0yRDBldU9oMmtrUQ=="  # Example:
es = Elasticsearch(
    "https://10.130.3.200:9200",
    api_key=api_key,
    verify_certs=False
)
 
async def poll_sensors():
    async with AsyncModbusTcpClient('10.160.0.26', port=502) as client:
        while True:
            # Read 2 registers starting from register 100 (for temp and humidity)
            response = await client.read_holding_registers(100, 2)

            if not response.isError():  # Check if there's no error
                sensor_values = response.registers

                # Extract temperature and humidity
                temperature = sensor_values[0] / 10.0  # Temperature is in register 100
                humidity = sensor_values[1] / 10.0     # Humidity is in register 101

                # Display the results
                text = f"Temperature: {temperature} °C, Humidity: {humidity} %"
                message = {
                     "Temperature": temperature,
                     "Humidity": humidity,
		             "date": datetime.now(),
                     "tag": "dht11"
                }
                response = es.index(index="iiot", body=message)
                print(response)

            await asyncio.sleep(0.1)  # Poll every 100 milliseconds

if __name__ == '__main__':
    asyncio.run(poll_sensors())
