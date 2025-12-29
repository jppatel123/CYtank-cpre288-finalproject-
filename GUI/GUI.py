import sys
import socket
from PyQt5.QtWidgets import QApplication, QMainWindow, QTextEdit, QVBoxLayout, QWidget, QPushButton, QLabel, QHBoxLayout, QGroupBox
from PyQt5.QtCore import QThread, pyqtSignal, QEvent
from PyQt5.QtGui import QFont, QColor, QPalette
import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import numpy as np
import pygame

HOST = "192.168.1.1"
PORT = 288

class CyBotSocketThread(QThread):
    message_received = pyqtSignal(str)

    def __init__(self, host, port):
        super().__init__()
        self.host = host
        self.port = port
        self.running = True

    def run(self):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((self.host, self.port))
            cybot = sock.makefile('rwb', buffering=1)
            while self.running:
                line = cybot.readline().decode().strip()
                if not line:
                    continue
                self.message_received.emit(line)
        except Exception as e:
            self.message_received.emit(f"Socket error: {e}")

    def stop(self):
        self.running = False
        self.quit()
        self.wait()

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("CyBot PyQt5 GUI")
        self.resize(1200, 700)  # Larger window

        # --- Robot position/path state ---
        self.COLS, self.ROWS, self.BLOCK_SIZE = 7, 4, 32.0  # Bigger field blocks
        self.robot_x = self.BLOCK_SIZE / 2
        self.robot_y = self.BLOCK_SIZE / 2
        self.robot_heading = 90.0
        self.robot_path_x = [self.robot_x]
        self.robot_path_y = [self.robot_y]
        self.scan_data = []
        self.detected_objects = []
        self.clicked_points = []  # Store user clicks

        # --- Controls Panel ---
        controls_widget = QWidget()
        controls_layout = QVBoxLayout()
        controls_widget.setLayout(controls_layout)
        controls_layout.addWidget(QLabel("<b>Status:</b>", parent=controls_widget))
        self.status_label = QLabel("Status: Disconnected")
        controls_layout.addWidget(self.status_label)
        self.connect_btn = QPushButton("Connect to CyBot")
        controls_layout.addWidget(self.connect_btn)
        self.connect_btn.clicked.connect(self.start_socket_thread)

        # Movement Controls
        move_group = QGroupBox("Movement Controls (W/A/S/D or buttons)")
        move_layout = QHBoxLayout()
        self.btn_w = QPushButton("W\n↑\nForward")
        self.btn_a = QPushButton("A\n←\nLeft")
        self.btn_s = QPushButton("S\n↓\nBack")
        self.btn_d = QPushButton("D\n→\nRight")
        for btn in [self.btn_w, self.btn_a, self.btn_s, self.btn_d]:
            btn.setFixedSize(80, 60)
            move_layout.addWidget(btn)
        move_group.setLayout(move_layout)
        controls_layout.addWidget(move_group)
        self.btn_w.clicked.connect(lambda: self.send_movement_command('w'))
        self.btn_a.clicked.connect(lambda: self.send_movement_command('a'))
        self.btn_s.clicked.connect(lambda: self.send_movement_command('s'))
        self.btn_d.clicked.connect(lambda: self.send_movement_command('d'))

        # Scan and Sound
        scan_sound_layout = QHBoxLayout()
        self.btn_scan = QPushButton("SCAN 180°")
        self.btn_scan.setFixedSize(120, 50)
        self.btn_scan.clicked.connect(self.send_scan_command)
        scan_sound_layout.addWidget(self.btn_scan)
        self.btn_q = QPushButton("Q\nShoot Sound")
        self.btn_q.setFixedSize(100, 60)
        self.btn_q.clicked.connect(self.play_shoot_sound)
        scan_sound_layout.addWidget(self.btn_q)
        self.btn_t = QPushButton("T\nSend UART1")
        self.btn_t.setFixedSize(100, 60)
        self.btn_t.clicked.connect(lambda: self.send_movement_command('t'))
        scan_sound_layout.addWidget(self.btn_t)
        controls_layout.addLayout(scan_sound_layout)

        # Text area for events
        self.text_area = QTextEdit()
        self.text_area.setReadOnly(True)
        controls_layout.addWidget(QLabel("<b>Event Log</b>"))
        controls_layout.addWidget(self.text_area, stretch=1)
        controls_layout.addStretch(1)

        # --- Plots Panel ---
        plots_widget = QWidget()
        plots_layout = QVBoxLayout()
        plots_widget.setLayout(plots_layout)
        plots_layout.addWidget(QLabel("<b>Radar Plot</b>"))
        self.radar_fig = Figure(figsize=(6.5, 6.5))
        self.radar_canvas = FigureCanvas(self.radar_fig)
        self.radar_ax = self.radar_fig.add_subplot(111, projection='polar')
        self.init_radar_plot()
        plots_layout.addWidget(self.radar_canvas)
        plots_layout.addWidget(QLabel("<b>Test Field Plot</b>"))
        self.field_fig = Figure(figsize=(7, 5))  # Larger test field
        self.field_canvas = FigureCanvas(self.field_fig)
        self.field_ax = self.field_fig.add_subplot(111)
        self.init_field_plot()
        plots_layout.addWidget(self.field_canvas, stretch=1)

        # --- Main Splitter Layout ---
        from PyQt5.QtWidgets import QSplitter
        splitter = QSplitter()
        splitter.addWidget(controls_widget)
        splitter.addWidget(plots_widget)
        splitter.setSizes([350, 850])
        self.setCentralWidget(splitter)

        # Keyboard shortcuts
        self.key_map = {'w': self.btn_w, 'a': self.btn_a, 's': self.btn_s, 'd': self.btn_d, 'q': self.btn_q, 'm': self.btn_scan}
        self.installEventFilter(self)

        # Try to init pygame mixer
        try:
            pygame.mixer.init()
            self.sound_available = True
        except Exception as e:
            self.sound_available = False
            self.text_area.append(f"[SOUND ERROR] {e}")

        self.socket_thread = None

        # Connect mouse click event to field canvas
        self.field_canvas.mpl_connect('button_press_event', self.on_field_click)

    def init_radar_plot(self):
        self.radar_ax.clear()
        self.radar_ax.set_theta_zero_location('E')
        self.radar_ax.set_theta_direction(1)
        self.radar_ax.set_thetamin(0)
        self.radar_ax.set_thetamax(180)
        self.radar_ax.set_ylim(0, 100)
        self.radar_ax.set_yticks([20, 40, 60, 80, 100])
        self.radar_ax.set_yticklabels(['20cm', '40cm', '60cm', '80cm', '100cm'])
        self.radar_ax.set_xticks(np.radians([0, 30, 60, 90, 120, 150, 180]))
        self.radar_ax.set_xticklabels(['0°', '30°', '60°', '90°', '120°', '150°', '180°'])
        self.radar_ax.set_title('Radar Scan', fontsize=12, fontweight='bold')
        self.radar_ax.grid(True, alpha=0.3)
        self.radar_canvas.draw()

    def update_radar_plot(self, scan_data=None, objects=None):
        self.init_radar_plot()
        if scan_data:
            angles_rad = [np.radians(d[0]) for d in scan_data]
            ir_distances = [d[1] for d in scan_data]
            self.radar_ax.plot(angles_rad, ir_distances, 'g-', linewidth=1, alpha=0.5, label='IR Scan')
        if objects:
            for obj in objects:
                angle_rad = np.radians(obj['angle'])
                distance = obj['distance']
                width = obj['width']
                is_target = obj['is_target']
                color = 'blue' if is_target else 'red'
                marker_size = 100 if is_target else 150
                self.radar_ax.scatter([angle_rad], [distance], c=color, s=marker_size, alpha=0.7, edgecolors='black', linewidths=2, zorder=5)
                self.radar_ax.annotate(f'{width:.1f}cm', xy=(angle_rad, distance), xytext=(3, 3), textcoords='offset points', fontsize=7, color=color)
        self.radar_ax.scatter([np.radians(90)], [0], c='green', s=150, marker='^', label='CyBot', zorder=10)
        self.radar_canvas.draw()

    def init_field_plot(self):
        self.field_ax.clear()
        COLS, ROWS, BLOCK_SIZE = self.COLS, self.ROWS, self.BLOCK_SIZE
        field_width = COLS * BLOCK_SIZE
        field_height = ROWS * BLOCK_SIZE
        self.field_ax.set_xlim(0, field_width)
        self.field_ax.set_ylim(0, field_height)
        self.field_ax.set_aspect('equal')
        self.field_ax.set_xlabel('X (cm)', fontsize=9)
        self.field_ax.set_ylabel('Y (cm)', fontsize=9)
        self.field_ax.set_title('Test Field', fontsize=11, fontweight='bold')
        for row in range(ROWS):
            for col in range(COLS):
                x = col * BLOCK_SIZE
                y = row * BLOCK_SIZE
                color = 'white' if (row + col) % 2 == 0 else '#f0f0f0'
                from matplotlib.patches import Rectangle
                rect = Rectangle((x, y), BLOCK_SIZE, BLOCK_SIZE, facecolor=color, edgecolor='black', linewidth=1)
                self.field_ax.add_patch(rect)
        for i in range(COLS + 1):
            self.field_ax.axvline(i * BLOCK_SIZE, color='black', linewidth=1.5)
        for i in range(ROWS + 1):
            self.field_ax.axhline(i * BLOCK_SIZE, color='black', linewidth=1.5)
        self.field_canvas.draw()

    def on_field_click(self, event):
        if event.inaxes != self.field_ax:
            return
        x, y = event.xdata, event.ydata
        # Check for shift+click (remove dot)
        is_shift = False
        if hasattr(event, 'key') and event.key == 'shift':
            is_shift = True
        elif hasattr(event, 'guiEvent') and event.guiEvent is not None:
            try:
                from PyQt5.QtCore import Qt
                if event.guiEvent.modifiers() & Qt.ShiftModifier:
                    is_shift = True
            except Exception:
                pass
        if is_shift:
            # Remove nearest dot if shift+click near it
            remove_idx = None
            min_dist = 25  # pixels
            click_disp = self.field_ax.transData.transform((x, y))
            for i, (px, py) in enumerate(self.clicked_points):
                dot_disp = self.field_ax.transData.transform((px, py))
                dist = np.hypot(dot_disp[0] - click_disp[0], dot_disp[1] - click_disp[1])
                if dist < min_dist:
                    min_dist = dist
                    remove_idx = i
            if remove_idx is not None:
                px, py = self.clicked_points[remove_idx]
                self.update_field_plot(self.robot_x, self.robot_y, self.robot_heading, self.robot_path_x, self.robot_path_y, highlight_dot=(px, py))
                QApplication.processEvents()
                import time
                time.sleep(0.15)
                removed = self.clicked_points.pop(remove_idx)
                self.text_area.append(f"<b style='color:orange'>[POINT REMOVED] ({removed[0]:.1f}, {removed[1]:.1f})</b>")
                self.update_field_plot(self.robot_x, self.robot_y, self.robot_heading, self.robot_path_x, self.robot_path_y)
            return  # Do not add a dot on shift+click
        # Normal click: add new dot
        self.clicked_points.append((x, y))
        self.text_area.append(f"<b style='color:purple'>[POINT] Clicked at ({x:.1f}, {y:.1f})</b>")
        self.update_field_plot(self.robot_x, self.robot_y, self.robot_heading, self.robot_path_x, self.robot_path_y)

    def update_field_plot(self, x, y, heading, path_x, path_y, highlight_dot=None):
        self.init_field_plot()
        from matplotlib.patches import Circle
        self.field_ax.plot(path_x, path_y, 'b-', linewidth=2, alpha=0.6, label='Path')
        robot_circle = Circle((x, y), radius=8, facecolor='lime', edgecolor='green', linewidth=2, zorder=10)
        self.field_ax.add_patch(robot_circle)
        arrow_length = 10
        arrow_rad = np.radians(heading - 90)
        dx = arrow_length * np.cos(arrow_rad)
        dy = arrow_length * np.sin(arrow_rad)
        self.field_ax.arrow(x, y, dx, dy, head_width=4, head_length=3, fc='darkgreen', ec='black', linewidth=1.5, zorder=11)
        col = int(x / 24.0)
        row = int(y / 24.0)
        field_height = 4 * 24.0
        pos_text = f'Pos: ({x:.1f}, {y:.1f}) cm\nGrid: Col{col} Row{row}\nHeading: {heading:.1f}°'
        self.field_ax.text(5, field_height - 5, pos_text, fontsize=8, verticalalignment='top', bbox=dict(boxstyle='round', facecolor='yellow', alpha=0.8))
        # Draw user clicked points
        for px, py in self.clicked_points:
            if highlight_dot and abs(px - highlight_dot[0]) < 1e-6 and abs(py - highlight_dot[1]) < 1e-6:
                self.field_ax.plot(px, py, 'yo', markersize=14, zorder=21)  # highlight in yellow
            else:
                self.field_ax.plot(px, py, 'ro', markersize=10, zorder=20)
        self.field_canvas.draw()

    def start_socket_thread(self):
        self.status_label.setText("Status: Connecting...")
        self.socket_thread = CyBotSocketThread(HOST, PORT)
        self.socket_thread.message_received.connect(self.handle_message)
        self.socket_thread.start()
        self.status_label.setText("Status: Connected")

    def handle_message(self, msg):
        # Parse scan/object data and update radar plot
        if msg.startswith("OBJECT"):
            # OBJECT angle distance width
            parts = msg.split()
            if len(parts) >= 4:
                obj = {
                    'angle': float(parts[1]),
                    'distance': float(parts[2]),
                    'width': float(parts[3]),
                    'is_target': float(parts[3]) < 6.0
                }
                self.detected_objects.append(obj)
            self.text_area.append(f"<b style='color:blue'>[OBJECT] {msg}</b>")
            self.update_radar_plot(self.scan_data, self.detected_objects)
        elif msg.startswith("BUMP"):
            self.text_area.append(f"<b style='color:red'>[BUMP] {msg}</b>")
            # Move CyBot back by 2 units in GUI test field
            rad = np.radians(self.robot_heading - 90)
            self.robot_x -= 3 * np.cos(rad)
            self.robot_y -= 3 * np.sin(rad)
            self.robot_x = max(0, min(self.robot_x, self.COLS * self.BLOCK_SIZE))
            self.robot_y = max(0, min(self.robot_y, self.ROWS * self.BLOCK_SIZE))
            self.robot_path_x.append(self.robot_x)
            self.robot_path_y.append(self.robot_y)
            self.update_field_plot(self.robot_x, self.robot_y, self.robot_heading, self.robot_path_x, self.robot_path_y)
        elif msg.startswith("TAPE"):
            self.text_area.append(f"<b style='color:orange'>[TAPE] {msg}</b>")
        elif msg == "END":
            # End of scan, update radar plot
            self.update_radar_plot(self.scan_data, self.detected_objects)
            self.scan_data = []
            self.detected_objects = []
        else:
            # Try to parse scan data: angle ir_distance ping_distance
            parts = msg.split()
            if len(parts) == 3:
                try:
                    angle = int(parts[0])
                    ir_dist = float(parts[1])
                    ping_dist = float(parts[2])
                    self.scan_data.append((angle, ir_dist, ping_dist))
                except ValueError:
                    self.text_area.append(msg)
            else:
                self.text_area.append(msg)

    def send_movement_command(self, cmd):
        # Update robot position/path locally (predictive, like Tkinter version)
        if cmd.lower() == 'w':
            rad = np.radians(self.robot_heading - 90)
            self.robot_x += 9.7 * np.cos(rad)
            self.robot_y += 9.7 * np.sin(rad)
        elif cmd.lower() == 's':
            rad = np.radians(self.robot_heading - 90)
            self.robot_x -= 9.7 * np.cos(rad)
            self.robot_y -= 9.7 * np.sin(rad)
        elif cmd.lower() == 'a':
            self.robot_heading += 18.0
            if self.robot_heading >= 360:
                self.robot_heading -= 360
        elif cmd.lower() == 'd':
            self.robot_heading -= 18.0
            if self.robot_heading < 0:
                self.robot_heading += 360
        # Clamp position
        self.robot_x = max(0, min(self.robot_x, self.COLS * self.BLOCK_SIZE))
        self.robot_y = max(0, min(self.robot_y, self.ROWS * self.BLOCK_SIZE))
        self.robot_path_x.append(self.robot_x)
        self.robot_path_y.append(self.robot_y)
        self.update_field_plot(self.robot_x, self.robot_y, self.robot_heading, self.robot_path_x, self.robot_path_y)
        # Send movement command to CyBot if connected
        if self.socket_thread and self.socket_thread.isRunning():
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.connect((HOST, PORT))
                sock.sendall((cmd + '\n').encode())
                sock.close()
                self.text_area.append(f"<b style='color:green'>[CMD] Sent: {cmd}</b>")
            except Exception as e:
                self.text_area.append(f"<b style='color:red'>[CMD ERROR] {e}</b>")
        else:
            self.text_area.append("<b style='color:red'>[CMD ERROR] Not connected</b>")

    def send_scan_command(self):
        self.send_movement_command('m')
        self.text_area.append("[CMD] Sent: m (SCAN 180°)")

    def play_shoot_sound(self):
        if not self.sound_available:
            self.text_area.append("<b style='color:red'>[SOUND ERROR] pygame not available</b>")
            return
        sound_files = ['audio.mp3', 'shoot.mp3', 'gunshot.mp3', 'laser.mp3']
        import os
        sound_path = None
        for filename in sound_files:
            if os.path.exists(filename):
                sound_path = filename
                break
        if sound_path is None:
            self.text_area.append("<b style='color:red'>[SOUND ERROR] No sound file found</b>")
            return
        try:
            pygame.mixer.music.load(sound_path)
            pygame.mixer.music.play()
            self.text_area.append(f"<b style='color:black'>[SOUND] Playing: {sound_path}</b>")
        except Exception as e:
            self.text_area.append(f"<b style='color:red'>[SOUND ERROR] {e}</b>")

def run_app():
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    app.exec_()

if __name__ == "__main__":
    run_app()