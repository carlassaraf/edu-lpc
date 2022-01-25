import socket
from enum import Enum

class Codes(Enum):

    CONFIG = 1,
    GPIO_CLEAR = 2,
    GPIO_SET = 3,
    GPIO_TOGGLE = 4,
    GPIO_READ = 5,
    ADC_READ = 6,
    LM35_C = 7,
    LM35_F = 8,
    DAC_SET = 9,
    DAC_SINE = 10,
    DAC_TRIANGULAR = 11,
    DAC_WAVE = 12,
    PWM_CONFIG = 13,
    BMP_READ = 14

class Functions(Enum):

    INPUT = 0,
    OUTPUT = 1,
    ADC = 2,
    DAC = 3,
    PWM = 4


class EDU_LPC_SOCK:

    def __init__(self, ip : str):

        self._server_ip = ip

        @property
        def server_ip(self):
            return self._server_ip

        @server_ip.setter
        def server_ip(self, ip):
            self._server_ip = ip


    def config(self, pin : int, function : Functions) -> None:

        self._connect()
        buffer = self._build_buffer([
            Codes.CONFIG.value[0],
            pin,
            function.value
        ])
        self._send(buffer)


    def gpio_clear(self, pin : int) -> None:

        self._connect()
        buffer = self._build_buffer([
            Codes.GPIO_CLEAR.value[0],
            pin
        ])
        self._send(buffer)  


    def gpio_set(self, pin : int) -> None:

        self._connect()
        buffer = self._build_buffer([
            Codes.GPIO_SET.value[0],
            pin
        ])
        self._send(buffer)  


    def gpio_toggle(self, pin : int) -> None:

        self._connect()
        buffer = self._build_buffer([
            Codes.GPIO_TOGGLE.value[0],
            pin
        ])
        self._send(buffer)        


    def dac_set(self, channel : int, value : int) -> None:

        self._connect()
        high = value >> 8
        low = value & 255
        buffer = self._build_buffer([
            Codes.DAC_SET.value[0],
            channel,
            high,
            low
        ])
        self._send(buffer)


    def dac_sine(self, channel : int, frequency : int) -> None:

        self._dac_build(
            Codes.DAC_SINE.value[0],
            channel,
            frequency
        )


    def dac_triangular(self, channel : int, frequency : int) -> None:
        
        self._dac_build(
            Codes.DAC_TRIANGULAR.value[0],
            channel,
            frequency
        )


    def dac_wave(self, channel : int, enable : bool) -> None:

        self._connect()
        buffer = self._build_buffer([
            Codes.DAC_WAVE.value[0],
            channel,
            1 if enable else 0
        ])
        self._send(buffer)


    def pwm_config(self, channel : int, frequency : int, duty : int) -> None:

        self._connect()
        high = frequency >> 8
        low = frequency & 255
        buffer = self._build_buffer([
            Codes.PWM_CONFIG.value[0],
            channel,
            high,
            low,
            duty
        ])
        self._send(buffer)


    def _dac_build(self, cmd : int, channel : int, frequency : int) -> None:

        self._connect()
        high = frequency >> 8
        low = frequency & 255
        buffer = self._build_buffer([
            cmd,
            channel,
            high,
            low
        ])
        self._send(buffer)


    def _build_buffer(self, buffer : list) -> list:

        header = len(buffer) + 1
        checksum = 0
        for b in buffer:
            checksum += b

        checksum = checksum & 255
        checksum = 255 - checksum

        buffer.insert(0, header)
        buffer.append(checksum)
        return buffer


    def _connect(self) -> None:

        self._client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._client.connect((self._server_ip, 8000))

    
    def _send(self, buffer : list) -> None:

        for byte in buffer:
            self._client.send(byte.to_bytes(1, 'big'))

        self._client.close()