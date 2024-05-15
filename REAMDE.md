# Shift register to UDP

## Example net_config.h

```c
#pragma once

const char *WLAN_SSID = "...";
const char *WLAN_PWD = "...";

const char *UDP_ADDRESS = "192.168.0.10";
const int UDP_PORT = 3333;
```

## Example UDP listener Python code

```python
import socket

UDP_IP = "0.0.0.0"
UDP_PORT = 3333


def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))

    while True:
        data, addr = sock.recvfrom(1024)
        print(f"received: {data} from {addr}")


if __name__ == '__main__':
    main()
```