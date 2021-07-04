import subprocess, click

@click.command()
@click.option("-d", "--device", default = 'all')
def flash_all(device):
    devs = [f"/dev/ttyUSB{idx}" for idx in range(2)]
    try:
        device = f"/dev/ttyUSB{int(d)}"
    except:
        print("Not an integer")
    if device != 'all':
        devs = [device]
    for dev in devs:
        print(f"Flashing {dev}")
        subprocess.call(f"pio run -t upload --upload-port {dev}".split())
if __name__ == "__main__":
    flash_all()

