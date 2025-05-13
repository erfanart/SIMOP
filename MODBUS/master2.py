import asyncio
from pymodbus.client import AsyncModbusTcpClient
from elasticsearch import Elasticsearch

# Replace with your actual API key
api_key = "NHp1akxwSUJFYXFLa1JaQ3hpQkU6OTc1b2dfcWdSRC0yRDBldU9oMmtrUQ=="  # Example: "BASE64_ENCODED_API_KEY"

# Connect to the Elasticsearch cluster using the API key
es = Elasticsearch(
    "https://10.130.3.200:9200",
    api_key=api_key,
    verify_certs=False
)

# Define the document you want to send
async def poll_sensors():
    async with AsyncModbusTcpClient('localhost', port=5020) as client:
        while True:
            response = await client.read_holding_registers(0, 1)
            if not response.isError():
                sensor_values = response.registers
                message = {
                    "Value": sensor_values,  # Replace this with your message content
                    "Sensor": "Temprature"
                }
                response = es.index(index="iiot", body=message)
                
                # Emit the sensor values to all connected clients
                #socketio.emit('sensor_data', {'sensor_values': sensor_values})
                print("send data to ELK:",response)
            await asyncio.sleep(2)  # Poll every 2 seconds

if __name__ == '__main__':
    asyncio.run(poll_sensors())
    #loop = asyncio.get_event_loop()
    #loop.create_task(poll_sensors())
