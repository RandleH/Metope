import serial
import argparse
import os
import asyncio
import serial_asyncio
import sys


parser = argparse.ArgumentParser()
parser.add_argument("--baudrate", "-b", type=int, default=115200             , help="Device Communication Baudrate. eg: 9600, 115200")
parser.add_argument("--port",     "-p", type=str, default="tty.usbserial-A50", help="System Device Port. Find the tty using `grep /dev/tty*`")
parser.add_argument("--timeout",  "-t", type=int, default=1,                   help="Timeout in seconds.")
parser.add_argument("--logfile",  "-l", type=str, default="",                  help="Log to file.")
(params, unknown_args) = parser.parse_known_args()


stdout = sys.stdout
if params.logfile:
  foutput = open( params.logfile, "w")
  sys.stdout = foutput
else:
  foutput = None



async def user_input_writer(writer):
  """
  Waits for user input and writes it to the serial port.
  """
  print("Type your message and press Enter. 'exit' to quit.")
  while True:
    try:
      # Use asyncio.to_thread to run the blocking input() function
      user_message = await asyncio.to_thread(input)
      if user_message.lower() == 'exit' or user_message.lower() == 'quit':
        break
      
      # Encode and send the message
      writer.write(user_message.encode('utf-8') + b'\n')
      await writer.drain() # Wait for the write to complete
    except asyncio.CancelledError:
      print("User input task cancelled.")
      break
    except Exception as e:
      print(f"An error occurred during user input: {e}")
      break

async def serial_reader(reader):
  """
  Reads data from the serial port and prints it.
  """
  print("Reading from serial port...")
  while True:
    try:
      line = await reader.readline()
      if line:
        decoded_line = line.decode('utf-8', errors='ignore').strip()
        print(f"| {decoded_line}")
    except asyncio.CancelledError:
      print("Serial reader task cancelled.")
      break
    except Exception as e:
      print(f"An error occurred during serial read: {e}")
      break


async def main():
  try:
    reader, writer = await serial_asyncio.open_serial_connection(url=os.path.join("/dev", params.port), baudrate=params.baudrate)
    
    # Create tasks for reading and writing concurrently
    reader_task = asyncio.create_task(serial_reader(reader))
    writer_task = asyncio.create_task(user_input_writer(writer))

    # Wait for the writer_task to finish (when user types 'exit')
    await writer_task
    
    # Cancel the reader_task gracefully
    reader_task.cancel()
    await reader_task # Wait for the cancellation to propagate

    # Close the serial connection
    writer.close()
    await writer.wait_closed()
    print("Serial connection closed.")
      
  except ConnectionRefusedError:
    print(f"Error: Could not connect to serial port {params.port}. Is the device connected and a program using it?")
  except Exception as e:
    print(f"An unexpected error occurred: {e}")



if __name__=="__main__":
  try:
    asyncio.run(main())
  except:
    pass
  finally:
    sys.stdout = stdout
    if params.logfile:
      foutput.close()