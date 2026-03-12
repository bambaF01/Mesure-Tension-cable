import csv
import os
import threading
import time
from datetime import datetime
import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import queue
import sys
import glob

import serial
import serial.tools.list_ports

BAUDRATE = 115200
DEFAULT_CSV_DIR = "donnees_csv"
KEYWORDS = ["ESP32", "CP210", "CH340", "USB Serial", "UART", "Silicon Labs"]
USB_KEYWORDS = [
    "USB",
    "CP210",
    "CH340",
    "Silicon Labs",
    "USB Serial",
    "FTDI",
    "FT232",
    "CDC",
    "ACM",
    "usbmodem",
    "usbserial",
]


def is_usb_port(device, desc, manu, hwid):
    text = f"{desc} {manu} {hwid}".lower()
    if any(k.lower() in text for k in USB_KEYWORDS):
        return True
    if "vid:pid" in text or "usb" in text:
        return True
    if not sys.platform.startswith("win"):
        if device.startswith("/dev/ttyUSB") or device.startswith("/dev/ttyACM"):
            return True
        if "/dev/cu.usb" in device or "/dev/tty.usb" in device:
            return True
    return False


def list_ports(show_all=False):
    raw = []
    ports = list(serial.tools.list_ports.comports(include_links=True))
    for port in ports:
        raw.append((
            port.device,
            port.description or "",
            port.manufacturer or "",
            getattr(port, "hwid", "") or "",
        ))

    # Windows: include COM ports from registry if available
    if sys.platform.startswith("win"):
        try:
            import serial.tools.list_ports_windows as lpw
            for port in lpw.comports():
                raw.append((
                    port.device,
                    port.description or "",
                    port.manufacturer or "",
                    getattr(port, "hwid", "") or "",
                ))
        except Exception:
            pass

    # Fallback: enumerate existing device files on Unix-like systems
    if not raw and not sys.platform.startswith("win"):
        patterns = []
        if sys.platform == "darwin":
            patterns = ["/dev/cu.*", "/dev/tty.*"]
        else:
            patterns = [
                "/dev/ttyUSB*",
                "/dev/ttyACM*",
                "/dev/ttyS*",
                "/dev/ttyAMA*",
                "/dev/ttyTHS*",
                "/dev/rfcomm*",
                "/dev/serial/by-id/*",
            ]
        for pattern in patterns:
            for path in glob.glob(pattern):
                if os.path.exists(path):
                    raw.append((path, "Port serie", "", ""))

    items = []
    seen = set()
    for device, desc, manu, hwid in raw:
        if not show_all and not is_usb_port(device, desc, manu, hwid):
            continue
        if device in seen:
            continue
        seen.add(device)
        label = f"{device} - {desc} {manu}".strip()
        items.append({
            "device": device,
            "label": label,
            "description": desc,
            "manufacturer": manu,
        })
    return items


def auto_detect_port(ports):
    for port in ports:
        text = f"{port['description']} {port['manufacturer']}".lower()
        if any(k.lower() in text for k in KEYWORDS):
            return port["device"]
    return None


class Recorder:
    def __init__(self, event_queue):
        self.event_queue = event_queue
        self.stop_event = threading.Event()
        self.thread = None
        self.ser = None
        self.csvfile = None
        self.writer = None
        self.csv_path = None

    def start(self, port, out_dir):
        if self.thread and self.thread.is_alive():
            raise RuntimeError("Recording already running")

        os.makedirs(out_dir, exist_ok=True)
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.csv_path = os.path.join(out_dir, f"donnees_forces_{timestamp}.csv")

        self.ser = serial.Serial(port, BAUDRATE, timeout=1)
        self.csvfile = open(self.csv_path, "w", newline="")
        self.writer = csv.writer(self.csvfile)
        self.writer.writerow(["Force_1_N", "Force_2_N", "Force_3_N", "Force_4_N"])

        self.stop_event.clear()
        self.thread = threading.Thread(target=self._loop, daemon=True)
        self.thread.start()
        self._emit("status", f"Connected to {port}")
        self._emit("status", f"Writing: {self.csv_path}")

    def _loop(self):
        try:
            while not self.stop_event.is_set():
                if self.ser.in_waiting > 0:
                    line = self.ser.readline().decode("utf-8", errors="ignore").strip()
                    if "\t" in line:
                        values = line.split("\t")
                        if len(values) == 4:
                            self.writer.writerow(values)
                            self.csvfile.flush()
                            self._emit("values", values)
                else:
                    time.sleep(0.01)
        except Exception as exc:
            self._emit("status", f"Error: {exc}")
        finally:
            self._close()

    def stop(self):
        self.stop_event.set()
        if self.thread:
            self.thread.join(timeout=2)
        self._close()
        self._emit("status", "Recording stopped")

    def _close(self):
        if self.ser and self.ser.is_open:
            try:
                self.ser.close()
            except Exception:
                pass
        if self.csvfile:
            try:
                self.csvfile.close()
            except Exception:
                pass

    def _emit(self, kind, payload):
        self.event_queue.put((kind, payload))


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("ESP32 CSV Recorder")
        self.geometry("780x480")
        self.resizable(True, True)

        self.event_queue = queue.Queue()
        self.recorder = Recorder(self.event_queue)
        self.ports = []
        self.port_var = tk.StringVar()
        self.out_dir_var = tk.StringVar(value=DEFAULT_CSV_DIR)
        self.status_var = tk.StringVar(value="Idle")
        self.last_values_var = tk.StringVar(value="-")
        self.show_all_ports_var = tk.BooleanVar(value=False)

        self._build_ui()
        self.refresh_ports()
        self.after(100, self.process_events)

        self.protocol("WM_DELETE_WINDOW", self.on_close)

    def _build_ui(self):
        header = ttk.Label(self, text="ESP32 CSV Recorder", font=("TkDefaultFont", 16, "bold"))
        header.pack(pady=10)

        frame = ttk.Frame(self)
        frame.pack(fill="x", padx=12)

        port_label = ttk.Label(frame, text="Serial port:")
        port_label.grid(row=0, column=0, sticky="w")

        self.port_combo = ttk.Combobox(frame, textvariable=self.port_var, state="readonly", width=50)
        self.port_combo.grid(row=0, column=1, sticky="we", padx=8)

        refresh_btn = ttk.Button(frame, text="Refresh", command=self.refresh_ports)
        refresh_btn.grid(row=0, column=2, padx=4)

        auto_btn = ttk.Button(frame, text="Auto-detect", command=self.auto_detect)
        auto_btn.grid(row=0, column=3, padx=4)

        out_label = ttk.Label(frame, text="CSV folder:")
        out_label.grid(row=1, column=0, sticky="w", pady=8)

        out_entry = ttk.Entry(frame, textvariable=self.out_dir_var, width=50)
        out_entry.grid(row=1, column=1, sticky="we", padx=8)

        browse_btn = ttk.Button(frame, text="Browse", command=self.browse_folder)
        browse_btn.grid(row=1, column=2, padx=4)

        frame.columnconfigure(1, weight=1)

        ports_frame = ttk.Frame(self)
        ports_frame.pack(fill="x", padx=12, pady=(6, 0))

        ports_title = ttk.Label(ports_frame, text="Available ports:")
        ports_title.pack(anchor="w")

        ports_list_frame = ttk.Frame(ports_frame)
        ports_list_frame.pack(fill="x")

        self.ports_list = tk.Listbox(ports_list_frame, height=4)
        self.ports_list.pack(side="left", fill="x", expand=True)
        self.ports_list.bind("<<ListboxSelect>>", self.on_port_select)

        ports_scroll = ttk.Scrollbar(ports_list_frame, orient="vertical", command=self.ports_list.yview)
        ports_scroll.pack(side="right", fill="y")
        self.ports_list.config(yscrollcommand=ports_scroll.set)

        ports_actions = ttk.Frame(ports_frame)
        ports_actions.pack(fill="x", pady=(4, 0))

        show_all_cb = ttk.Checkbutton(
            ports_actions,
            text="Show all ports",
            variable=self.show_all_ports_var,
            command=self.refresh_ports,
        )
        show_all_cb.pack(side="left")

        rescan_btn = ttk.Button(ports_actions, text="Rescan", command=self.refresh_ports)
        rescan_btn.pack(side="right")

        buttons = ttk.Frame(self)
        buttons.pack(fill="x", padx=12, pady=8)

        self.start_btn = ttk.Button(buttons, text="Start", command=self.start_recording)
        self.start_btn.pack(side="left")

        self.stop_btn = ttk.Button(buttons, text="Stop", command=self.stop_recording, state="disabled")
        self.stop_btn.pack(side="left", padx=8)

        status_frame = ttk.Frame(self)
        status_frame.pack(fill="x", padx=12)

        status_label = ttk.Label(status_frame, text="Status:")
        status_label.grid(row=0, column=0, sticky="w")

        status_value = ttk.Label(status_frame, textvariable=self.status_var)
        status_value.grid(row=0, column=1, sticky="w", padx=8)

        values_label = ttk.Label(status_frame, text="Last values:")
        values_label.grid(row=1, column=0, sticky="w", pady=6)

        values_value = ttk.Label(status_frame, textvariable=self.last_values_var)
        values_value.grid(row=1, column=1, sticky="w", padx=8)

        log_label = ttk.Label(self, text="Log:")
        log_label.pack(anchor="w", padx=12, pady=(10, 0))

        self.log_text = tk.Text(self, height=10, wrap="word", state="disabled")
        self.log_text.pack(fill="both", expand=True, padx=12, pady=(0, 12))

    def refresh_ports(self):
        self.ports = list_ports(show_all=self.show_all_ports_var.get())
        labels = [p["label"] for p in self.ports]
        self.port_combo["values"] = labels
        self.ports_list.delete(0, "end")
        for label in labels:
            self.ports_list.insert("end", label)
        if not labels:
            self.ports_list.insert("end", "No ports detected")
        if labels:
            self.port_combo.current(0)
            self.port_var.set(labels[0])
        else:
            self.port_var.set("")
        self.log(f"Ports detected: {len(labels)}")

    def auto_detect(self):
        if not self.ports:
            self.refresh_ports()
        device = auto_detect_port(self.ports)
        if device is None:
            messagebox.showwarning("Auto-detect", "ESP32 not found. Select a port manually.")
            return
        for idx, port in enumerate(self.ports):
            if port["device"] == device:
                self.port_combo.current(idx)
                self.port_var.set(port["label"])
                self.log(f"Auto-detected: {device}")
                return

    def browse_folder(self):
        folder = filedialog.askdirectory()
        if folder:
            self.out_dir_var.set(folder)

    def start_recording(self):
        if self.recorder.thread and self.recorder.thread.is_alive():
            return

        port = self.get_selected_port()
        if not port:
            messagebox.showerror("Start", "Select a serial port first.")
            return

        out_dir = self.out_dir_var.get().strip() or DEFAULT_CSV_DIR
        try:
            self.recorder.start(port, out_dir)
        except Exception as exc:
            messagebox.showerror("Start", str(exc))
            return

        self.status_var.set("Recording")
        self.start_btn.config(state="disabled")
        self.stop_btn.config(state="normal")

    def stop_recording(self):
        self.recorder.stop()
        self.status_var.set("Idle")
        self.start_btn.config(state="normal")
        self.stop_btn.config(state="disabled")

    def update_values(self, values):
        self.last_values_var.set(", ".join(values))

    def process_events(self):
        while True:
            try:
                kind, payload = self.event_queue.get_nowait()
            except queue.Empty:
                break
            if kind == "status":
                self.log(payload)
            elif kind == "values":
                self.update_values(payload)
        self.after(100, self.process_events)

    def log(self, message):
        self.log_text.config(state="normal")
        self.log_text.insert("end", message + "\n")
        self.log_text.see("end")
        self.log_text.config(state="disabled")

    def get_selected_port(self):
        label = self.port_var.get()
        for port in self.ports:
            if port["label"] == label:
                return port["device"]
        return None

    def on_port_select(self, event):
        selection = self.ports_list.curselection()
        if not selection:
            return
        label = self.ports_list.get(selection[0])
        if label in self.port_combo["values"]:
            self.port_var.set(label)

    def on_close(self):
        if self.recorder.thread and self.recorder.thread.is_alive():
            if not messagebox.askyesno("Quit", "Recording is active. Stop and quit?"):
                return
            self.stop_recording()
        self.destroy()


if __name__ == "__main__":
    app = App()
    app.mainloop()
