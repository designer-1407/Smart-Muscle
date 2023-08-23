from urllib.request import urlopen
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

NUMBER_OF_DEVICES = 4

df = pd.DataFrame(columns=['time', 'sensor_1', 'bat_1', 'sensor_2', 'bat_2', 'sensor_3', 'bat_3', 'sensor_4', 'bat_4', 'sensor_5', 'bat_5'])

sensor_columns = ['sensor_1', 'sensor_2', 'sensor_3', 'sensor_4', 'sensor_5']
plt.tight_layout()
plt.ion()
fig, axes = plt.subplots(nrows=NUMBER_OF_DEVICES, ncols=1, figsize=(8, 20))

lines = []
for i in range(NUMBER_OF_DEVICES):
    ax = axes[i]
    #ax.set_xlabel('Time ms')
    ax.set_ylabel('Data')
    ax.set_title(f'EMG {i+1}')
    ax.legend(['Data'], loc='upper right')
    line, = ax.plot(df[sensor_columns[i]])
    lines.append(line)

def get_data(url):
    page = urlopen(url)

    html_bytes = page.read()
    data = str(html_bytes.decode("utf-8"))

    lines_of_data = data.split(";")
    data = [lines_of_data[0]]
    for line in lines_of_data[1:-1]:
        data.append(line.split('|'))

    data = {
        'time': [int(data[0])],
        'sensor_1': [int(data[1][1])],
        'bat_1': [data[1][2]],
        'sensor_2': [int(data[2][1])],
        'bat_2': [data[2][2]],
        'sensor_3': [int(data[3][1])],
        'bat_3': [data[3][2]],
        'sensor_4': [int(data[4][1])],
        'bat_4': [data[4][2]],
        'sensor_5': [int(data[5][1])],
        'bat_5': [data[5][2]]
    }

    return data

def update_df(df):
    data = get_data("http://192.168.4.1/emg")
    df = pd.concat([df, pd.DataFrame(data)])

    if(len(df) > 150):
        df = df[len(df) - 150:]

    return df

while True:
    df = update_df(df)

    for i in range(NUMBER_OF_DEVICES):
        line = lines[i]
        ax = axes[i]
        ax.set_ylim(df[sensor_columns[i]].min() - 100, df[sensor_columns[i]].max() + 100)
        #ax.set_ylim(0, 1024)
        ax.set_xlim(0, 150)
        line.set_ydata(df[sensor_columns[i]])
        line.set_xdata(np.arange(df.shape[0]))

    plt.draw()
    plt.pause(0.01)
