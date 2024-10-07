# Shift register to UDP

## Example net_config.h

To make this work, you'll have to configure the WIFI connection.

Create your own `src/net_config.h` file. It should look something like this:

```c
#pragma once

const char *WLAN_SSID = "...";
const char *WLAN_PWD = "...";
```

## Troubleshooting

If the measurements are unstable, try to increase the `CLOCK_DELAY_US` constant in `shift_reg.h`.