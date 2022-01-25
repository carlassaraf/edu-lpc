import uos, machine
import gc
gc.collect()

import network
from credentials import ssid, password

sta_if = network.WLAN(network.STA_IF)   # Set up STA_IF interface
sta_if.active(True)                     # Activate interface

if not sta_if.isconnected():
    print("Connecting to network...")
    sta_if.connect(ssid, password)
    while not sta_if.isconnected():
        pass

print("Network config: ", sta_if.ifconfig())