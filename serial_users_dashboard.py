import ast
import queue
import threading
import tkinter as tk
from typing import Any, Optional
from tkinter import messagebox
from tkinter import ttk

try:
	import serial  # type: ignore[import-not-found]
except Exception:
	serial = None


SERIAL_PORT = "COM6"
BAUD_RATE = 115200
STEP_GOAL = 2000


class UserPanel(ttk.LabelFrame):
	def __init__(self, master, user_id):
		super().__init__(master, text=f"User {user_id}", padding=10)
		self.user_id = user_id
		self.current_steps = tk.IntVar(value=0)
		self.progress_value = tk.DoubleVar(value=0.0)
		self.calories_text = tk.StringVar(value="Calories: -")
		self.weight_var = tk.StringVar()
		self.height_var = tk.StringVar()

		ttk.Label(self, text="Steps:").grid(row=0, column=0, sticky="w")
		ttk.Label(self, textvariable=self.current_steps, font=("Segoe UI", 14, "bold")).grid(
			row=0, column=1, sticky="w", padx=(6, 0)
		)

		self.progress = ttk.Progressbar(
			self,
			orient="horizontal",
			mode="determinate",
			maximum=STEP_GOAL,
			variable=self.progress_value,
			length=260,
		)
		self.progress.grid(row=1, column=0, columnspan=3, pady=(8, 4), sticky="ew")

		ttk.Label(self, text=f"Goal: {STEP_GOAL} steps").grid(
			row=2, column=0, columnspan=3, sticky="w"
		)

		ttk.Separator(self, orient="horizontal").grid(
			row=3, column=0, columnspan=3, sticky="ew", pady=8
		)

		ttk.Label(self, text="Weight (kg):").grid(row=4, column=0, sticky="w")
		ttk.Entry(self, textvariable=self.weight_var, width=10).grid(row=4, column=1, sticky="w")

		ttk.Label(self, text="Height (cm):").grid(row=5, column=0, sticky="w", pady=(4, 0))
		ttk.Entry(self, textvariable=self.height_var, width=10).grid(
			row=5, column=1, sticky="w", pady=(4, 0)
		)

		ttk.Button(self, text="Calculate Calories", command=self.on_calculate_calories).grid(
			row=6, column=0, columnspan=2, sticky="w", pady=(8, 0)
		)
		ttk.Label(self, textvariable=self.calories_text).grid(
			row=7, column=0, columnspan=3, sticky="w", pady=(6, 0)
		)

		ttk.Label(self, text="Received step packets:").grid(row=8, column=0, columnspan=3, sticky="w", pady=(10, 4))
		self.step_listbox = tk.Listbox(self, height=8)
		self.step_listbox.grid(row=9, column=0, columnspan=3, sticky="nsew")

		self.columnconfigure(2, weight=1)
		self.rowconfigure(9, weight=1)

	def update_steps(self, steps):
		self.current_steps.set(steps)
		self.progress_value.set(min(steps, STEP_GOAL))
		self.step_listbox.insert(tk.END, str(steps))
		self.step_listbox.yview_moveto(1.0)

	def on_calculate_calories(self):
		try:
			weight = float(self.weight_var.get())
			height = float(self.height_var.get())
		except ValueError:
			messagebox.showerror("Invalid input", f"Please enter valid weight/height for User {self.user_id}.")
			return

		calories = self.calculate_calories_burned(
			steps=self.current_steps.get(),
			weight_kg=weight,
			height_cm=height,
		)

		if calories is None:
			self.calories_text.set("Calories: (calculation not implemented yet)")
			return

		self.calories_text.set(f"Calories: {calories:.2f} kcal")
        
	def calculate_calories_burned(self, steps, weight_kg, height_cm):
		step_length = 0.415*height_cm/100
		distance = steps*step_length/1000
		MET = 3.5
		speed = 4
		return ((MET*weight_kg*3.5)/200)*(distance*60/speed)
	

class SerialUsersDashboard(tk.Tk):
	def __init__(self):
		super().__init__()
		self.title("Users Steps Dashboard")
		self.geometry("860x560")
		self.minsize(760, 520)

		self.data_queue = queue.Queue()
		self.stop_event = threading.Event()
		self.serial_thread = None
		self.ser: Optional[Any] = None

		self.step_history = {1: [], 2: []}

		self.status_text = tk.StringVar(value=f"Connecting to {SERIAL_PORT} @ {BAUD_RATE}...")
		ttk.Label(self, textvariable=self.status_text).pack(anchor="w", padx=10, pady=(10, 6))

		content = ttk.Frame(self)
		content.pack(fill="both", expand=True, padx=10, pady=(0, 10))
		content.columnconfigure(0, weight=1)
		content.columnconfigure(1, weight=1)
		content.rowconfigure(0, weight=1)

		self.user_panels = {
			1: UserPanel(content, 1),
			2: UserPanel(content, 2),
		}
		self.user_panels[1].grid(row=0, column=0, sticky="nsew", padx=(0, 8))
		self.user_panels[2].grid(row=0, column=1, sticky="nsew", padx=(8, 0))

		self.protocol("WM_DELETE_WINDOW", self.on_close)
		self.start_serial_reader()
		self.after(100, self.process_queue)

	def start_serial_reader(self):
		if serial is None:
			self.status_text.set("pyserial is not installed. Install it with: pip install pyserial")
			return

		try:
			self.ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
		except Exception as exc:
			self.status_text.set(f"Could not open {SERIAL_PORT}: {exc}")
			return

		self.status_text.set(f"Connected to {SERIAL_PORT} @ {BAUD_RATE}")
		self.serial_thread = threading.Thread(target=self.read_serial_loop, daemon=True)
		self.serial_thread.start()

	def read_serial_loop(self):
		ser = self.ser
		if ser is None:
			return

		while not self.stop_event.is_set():
			try:
				line = ser.readline()
				if not line:
					continue

				decoded = line.decode("utf-8", errors="ignore").strip()
				if not decoded:
					continue

				packet = self.parse_packet(decoded)
				if packet is None:
					continue

				user_id = packet.get("User")
				steps = packet.get("Steps")
				if user_id not in (1, 2):
					continue
				if steps is None:
					continue
				if not isinstance(steps, int):
					try:
						steps = int(steps)
					except (TypeError, ValueError):
						continue

				self.data_queue.put((user_id, steps))
			except Exception:
				continue

	@staticmethod
	def parse_packet(text):
		# Input is expected as Python-style dict string, e.g. {'User': 2, 'Steps': 123}.
		try:
			parsed = ast.literal_eval(text)
		except Exception:
			return None
		if not isinstance(parsed, dict):
			return None
		return parsed

	def process_queue(self):
		while True:
			try:
				user_id, steps = self.data_queue.get_nowait()
			except queue.Empty:
				break

			self.step_history[user_id].append(steps)
			self.user_panels[user_id].update_steps(steps)

		if not self.stop_event.is_set():
			self.after(100, self.process_queue)

	def on_close(self):
		self.stop_event.set()
		try:
			if self.ser and self.ser.is_open:
				self.ser.close()
		except Exception:
			pass
		self.destroy()


if __name__ == "__main__":
	app = SerialUsersDashboard()
	app.mainloop()
