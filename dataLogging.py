import serial
import pandas as pd
import numpy as np
import time
import datetime

PORT = "COM7"
BAUD = 9600
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

    programTimeNP = np.array(programTime, dtype=np.float32) / 1000
    relativeTimeNP = np.array(relativeTime, dtype=np.float32) / 1000
    torqueNP = np.array(torque, dtype=np.float32)
    unitNP = np.array(unit)

    torqueOZ_IN = np.empty(len(torqueNP))
    torqueGF_CM = np.empty(len(torqueNP))
    torqueIN_LB = np.empty(len(torqueNP))
    torqueMN_M  = np.empty(len(torqueNP))

    for i in range(len(torqueNP)):
        if unit[i] in 'ozin':
            torqueOZ_IN[i] = torqueNP[i]
            torqueGF_CM[i] = torqueOZ_IN[i] * 72.007790632
            torqueIN_LB[i] = torqueOZ_IN[i] * 0.0624999993
            torqueMN_M[i] = torqueOZ_IN[i] * 7.061552

        elif unit[i] in 'gfcm':
            torqueGF_CM[i] = torqueNP[i]
            torqueOZ_IN[i] = torqueGF_CM[i] * 0.0138873862
            torqueIN_LB[i] = torqueGF_CM[i] * 0.0008679616
            torqueMN_M[i] = torqueGF_CM[i] * 0.0980665

        elif unit[i] in 'inlb':
            torqueIN_LB[i] = torqueNP[i]
            torqueOZ_IN[i] = torqueIN_LB[i] * 16.000000189
            torqueGF_CM[i] = torqueIN_LB[i] * 1152.1246637
            torqueMN_M[i] = torqueIN_LB[i] * 112.98483333

        elif unit[i] in 'mNm':
            torqueMN_M[i] = torqueNP[i]
            torqueOZ_IN[i] = torqueMN_M[i] * 0.1416119289
            torqueGF_CM[i] = torqueMN_M[i] * 10.19716213
            torqueIN_LB[i] = torqueMN_M[i] * 0.0088507455
        

    df = pd.DataFrame({
        'programTime (s)' : programTimeNP, 
        'relativeTime (s)' : relativeTimeNP, 
        'torque (ozin)' : torqueOZ_IN, 
        'torque (gfcm)' : torqueGF_CM, 
        'torque (inlb)' : torqueIN_LB, 
        'torque (mNm)' : torqueMN_M
        })
    
    print(df)

    timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")

    writer = pd.ExcelWriter(f"data/excel/torque_{timestamp}.xlsx", engine='xlsxwriter')
    df.to_excel(writer, sheet_name='Sheet1', index=False)
    workbook = writer.book
    worksheet = writer.sheets['Sheet1']
    chart = workbook.add_chart({'type':'scatter'})
    chart.add_series({
        'categories': ['Sheet1', 0, 0, len(programTimeNP)-1, 0],
        'values': ['Sheet1', 1, 2, len(torqueOZ_IN)-1,2],
        'marker': {
            'type': 'diamond', 
            'size': 2,
            'border': {'color': 'black'},
            'fill': {'color': 'black'}
        },
        'name': 'torque (ozin)'
    })
    chart.add_series({
        'categories': ['Sheet1', 0, 0, len(programTimeNP)-1, 0],
        'values': ['Sheet1', 1, 3, len(torqueGF_CM)-1,3],
        'marker': {
            'type': 'diamond', 
            'size': 2,
            'border': {'color': 'red'},
            'fill': {'color': 'red'}
        },
        'name': 'torque (gfcm)'
    })
    chart.add_series({
        'categories': ['Sheet1', 0, 0, len(programTimeNP)-1, 0],
        'values': ['Sheet1', 1, 4, len(torqueIN_LB)-1,4],
        'marker': {
            'type': 'diamond', 
            'size': 2,
            'border': {'color': 'blue'},
            'fill': {'color': 'blue'}
        },
        'name': 'torque (inlb)'
    })
    chart.add_series({
        'categories': ['Sheet1', 0, 0, len(programTimeNP)-1, 0],
        'values': ['Sheet1', 1, 5, len(torqueMN_M)-1,5],
        'marker': {
            'type': 'diamond', 
            'size': 2,
            'border': {'color': 'green'},
            'fill': {'color': 'green'}
        },
        'name': 'torque (mNm)'
    })
    chart.set_x_axis({'name': 'Program Time (s)'})
    chart.set_y_axis({'name': 'Torque'})
    chart.set_size({'x_scale': 1.5, 'y_scale': 2})
    worksheet.insert_chart('H2', chart)
    writer.close()

    df.to_csv(f"data/csv/torque_{timestamp}.csv", index=False)
    print(f'df written to data folder as torque_{timestamp}')
