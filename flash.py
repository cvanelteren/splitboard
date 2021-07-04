import subprocess
devs = [f"/dev/ttyUSB{idx}" for idx in range(2)]

for dev in devs:
    print(f"Flashing {dev}")
    subprocess.call(f"pio run -t upload --upload-port {dev}".split())

