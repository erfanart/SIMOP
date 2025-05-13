import asyncio
from pymodbus.client import AsyncModbusTcpClient
from flask import Flask, render_template
from flask_socketio import SocketIO, emit
import logging

# Configure logging
logging.basicConfig()
log = logging.getLogger()
log.setLevel(logging.DEBUG)

# Flask app and SocketIO for real-time communication
app = Flask(__name__)
socketio = SocketIO(app)

# Polling the Modbus slave to get real-time data
async def poll_sensors():
    async with AsyncModbusTcpClient('localhost', port=5020) as client:
        while True:
            response = await client.read_holding_registers(0, 10)
            if not response.isError():
                sensor_values = response.registers
                # Emit the sensor values to all connected clients
                socketio.emit('sensor_data', {'sensor_values': sensor_values})
            await asyncio.sleep(2)  # Poll every 2 seconds

@app.route('/')
def index():
    return render_template('index.html')

# Run Flask app with SocketIO
def run_web_app():
    socketio.run(app,host='0.0.0.0', port=6000, debug=True, use_reloader=False)

if __name__ == '__main__':
    # Start the event loop for polling the Modbus data
    loop = asyncio.get_event_loop()
    loop.create_task(poll_sensors())
    run_web_app()

