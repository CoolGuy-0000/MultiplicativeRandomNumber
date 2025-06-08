import matplotlib.pyplot as plt
import matplotlib.animation as animation
import subprocess
import collections


animation_running = False
ani = None

fig, ax = plt.subplots()
x_len = 100
y_range = [0, 65535]
line, = ax.plot(list(range(x_len)), [0] * x_len)
ax.set_ylim(y_range)
ax.set_title("Multiplicative Random Number Output")
ax.set_xlabel("Time")
ax.set_ylabel("Value")

data_buffer = collections.deque([0] * x_len, maxlen=x_len)

process = subprocess.Popen(['./Math02'], stdout=subprocess.PIPE, bufsize=1, universal_newlines=True)

def update_graph(frame):
    global animation_running
    if not animation_running:
        return line,

    try:
        
        line_data = process.stdout.readline()
        if line_data:
            rand_value = int(line_data.strip())
            data_buffer.append(rand_value)
            line.set_ydata(data_buffer) 
            ax.draw_artist(ax.patch)
            ax.draw_artist(line)
            fig.canvas.blit(ax.bbox)
            return line,
        else:
            print("C program finished or no more data.")
            ani.event_source.stop()
            return line,
    except ValueError:
        print(f"Could not convert data to int: {line_data.strip()}")
        return line,
    except Exception as e:
        print(f"Error reading from C program: {e}")
        ani.event_source.stop() 
        return line,

def on_key_press(event):
    global animation_running
    global ani

    if event.key == ' ': 
        if animation_running:
            ani.event_source.stop()
            print("Animation paused.")
        else:
            ani.event_source.start()
            print("Animation resumed.")
        animation_running = not animation_running

ani = animation.FuncAnimation(fig, update_graph, interval=1, blit=True)
fig.canvas.mpl_connect('key_press_event', on_key_press)

plt.show()

process.terminate()