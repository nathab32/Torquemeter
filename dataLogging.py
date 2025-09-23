import serial
import pandas as pd
import numpy as np
import time
import datetime

PORT = "COM7"
BAUD = 115200
SERIAL_TIMEOUT = 1 # timeout for serial port data
DATA_TIMEOUT = 5 # time until program ends if no data detected

ser = serial.Serial(PORT, BAUD, timeout=SERIAL_TIMEOUT)
time.sleep(2) # wait for Arduino reset

programTime = []
relativeTime = []
torque = []
unit = []

try:
    last_received_time = time.time()

    while True:
        line = ser.readline()
        if not line:
            if time.time() - last_received_time > DATA_TIMEOUT:
                print('Data timeout activated')
                break
            continue

        last_received_time = time.time()

        text = line.decode("utf-8", "replace").strip()
        if text.startswith('<') and text.endswith('>'):
            text = text.lstrip('<').rstrip('>').split(',')

            programTime.append(int(text[0]))
            relativeTime.append(int(text[1]))
            torque.append(float(text[2]))
            unit.append(text[3])

        elif 'exit' in text:
            print('Exit detected')
            break

        print(text)
except KeyboardInterrupt:
    pass
finally:
    ser.close()
    print('Serial closed')

    programTimeNP = np.array(programTime, dtype=np.uint32)
    relativeTimeNP = np.array(relativeTime, dtype=np.uint32)
    torqueNP = np.array(torque, dtype=np.float32)
    unitNP = np.array(unit)

    df = pd.DataFrame({
        'programTime' : programTimeNP, 
        'relativeTime' : relativeTimeNP, 
        'torque' : torqueNP, 
        'unit' : unitNP
        })
    
    print(df)

    timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    df.to_excel(f"data/excel/torque_{timestamp}.xlsx")
    df.to_csv(f"data/csv/torque_{timestamp}.csv")
    print(f'df written to data folder as torque_{timestamp}')
