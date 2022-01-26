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
        self._cmd = ""

        @property
        def server_ip(self):
            return self._server_ip

        @server_ip.setter
        def server_ip(self, ip):
            self._server_ip = ip


    def run(self) -> None:
        """
        Main application loop. Handles the interaction and calls the commands.
        args:
            - None.
        return:
            - None.
        """
        # Welcome message
        print("EDU LPC Socket interface v1 - 2022-01-25")
        print()
        print("Ingrese un comando, --exit para salir o --help para la lista de comandos")
        print()

        cmd = ""
        while cmd != "exit":
            self._cmd = input(">>>").lower()
            args = self._cmd.split()
            cmd = args[0].replace("--", "")
            # --help command
            if cmd == "help":
                self.help()
            # GPIO config command
            elif cmd == "config":
                self.cmdConfig(args)
            # GPIO clear command
            elif cmd == "clr":
                self.cmdClr(args)
            # GPIO set command
            elif cmd == "set":
                self.cmdSet(args)
            # GPIO toggle command
            elif cmd == "toggle":
                self.cmdToggle(args)
            # GPIO read command
            elif cmd == "read":
                self.cmdRead(args)
            # ADC read command
            elif cmd == "adc":
                self.cmdAdc(args)
            # LM35 read command
            elif cmd == "lm":
                self.cmdLm(args)
            # DAC set command
            elif cmd == "dac":
                self.cmdDac(args)
            # DAC sine command
            elif cmd == "sine":
                self.cmdSine(args)
            # DAC triangular command
            elif cmd == "triang":
                self.cmdTriang(args)
            # PWM config command
            elif cmd == "pwm":
                self.cmdPwm(args)
            # BMP180 read command
            elif cmd == "bmp":
                self.cmdBmp(args)
            

    def help(self) -> None:
        """
        Prints the command list an examples.
        args: 
            - None.
        return:
            - None.
        """
        cmds = {
            "config" :  "Configura un GPIO disponible como input, output, ADC, DAC o PWM\n" \
                        + "args:  -io numero de PIO a configurar (16-23)\n"                 \
                        + "       -f funcion a configurar (in, out, adc, dac, pwm)\n"       \
                        + "ej: config -io 17 -f PWM", 
            "clr" :     "Hace un clear en un GPIO\n"            \
                        + "args:  -io numero de PIO (16-23)\n"  \
                        + "ej: clr -io 16",
            "set" :     "Hace un set en un PIO\n"               \
                        + "args:  -io numero de PIO (166-23)\n" \
                        + "ej: set -io 18", 
            "toggle" :  "Hace un toggle en un PIO\n"            \
                        + "args:  -io numero de PIO (16-23)\n"  \
                        + "ej: toggle -io 23", 
            "read" :    "Obtiene el valor de un GPIO\n"         \
                        + "args:  -io numero de GPIO (0-7)\n"   \
                        + "ej: read -io 5", 
            "adc" :     "Obtiene el valor de un canal analogico\n"      \
                        + "args:  -ch canal a convertir (0, 1 y 3-9)\n" \
                        + "ej: adc -ch 2", 
            "lm" :      "Obtiene la temperatura medida por el LM35 en grados Celsius o Fahrenheit\n"    \
                        + "args:  -u unidad de temperatura (cent/fahr)"                                 \
                        + "ej: lm -u fahr", 
            "dac" :     "Pone un valor a la salida del DAC solicitado\n"    \
                        + "args:  -ch canal de DAC (0-1)\n"                 \
                        + "       -v valor para el DAC (0-1023)\n"          \
                        + "ej: dac -ch 0 -v 578",
            "sine" :    "Configura una senoidal para el DAC solicitado\n"   \
                        + "args:  -ch canal de DAC (0-1)\n"                 \
                        + "       -f frecuencia de la sinoidal (Hz)\n"      \
                        + "ej: sine -ch 1 -f 100",
            "triang" :  "Configura una triangular para el DAC solicitado\n" \
                        + "args:  -ch canal de DAC (0-1)\n"                 \
                        + "       -f frecuencia de la sinoidal (Hz)\n"      \
                        + "ej: triang -ch 0 -f 250",
            "pwm" :     "Configura una salida de PWM\n"
                        + "args:  -io canal de PWM a configurar (0-3)\n"    \
                        + "       -f frequencia del PWM (Hz)\n"             \
                        + "       -du ciclo de trabajo en % (0-100)\n"      \
                        + "ej: pwm -io 0 -f 2000 -du 25", 
            "bmp" :     "Obtiene el valor de temperatura y presion que mida el BMP180\n"
                        + "args:  None"
        }

        print()
        # Iterate through dictionary and print
        for key in cmds:
            print(f"{key}:")
            print(cmds[key])
            print()


    def cmdConfig(self, args : list):
        """
        Handles the config command.
        args:
            - args: list with the command arguments.
        return:
            - None.
        """
        # Variables to store the useful arguments
        io = 0
        f = 0
        # Iterate through the arguments
        for i, arg in enumerate(args):
            # Look for the -io option
            if arg == "-io":
                # Save the next argument
                io = int(args[i + 1])
            # Look for the -f option
            if arg == "-f":
                # Save the next argument
                f = args[i + 1]
        # Available functions
        functions = ["in", "out", "adc", "dac", "pwm"]
        # Iterate through the functions
        for i, fun in enumerate(functions):
            # Change the function argument to an int
            if fun == f:
                f = i
                break
        # Call the config method
        self.config(io, f)


    def cmdClr(self, args : list):
        """
        Handles the clr command.
        args:
            - args: list with the command arguments.
        return:
            - None.
        """
        # Variables to store the useful arguments
        io = 0
        # Iterate through the arguments
        for i, arg in enumerate(args):
            # Look for the -io option
            if arg == "-io":
                # Save the next argument
                io = int(args[i + 1])
        # Call the GPIO clear method
        self.gpio_clear(io)


    def cmdSet(self, args : list):
        """
        Handles the set command.
        args:
            - args: list with the command arguments.
        return:
            - None.
        """
        # Variables to store the useful arguments
        io = 0
        # Iterate through the arguments
        for i, arg in enumerate(args):
            # Look for the -io option
            if arg == "-io":
                # Save the next argument
                io = int(args[i + 1])
        # Call the GPIO clear method
        self.gpio_set(io)


    def cmdToggle(self, args : list):
        """
        Handles the toggle command.
        args:
            - args: list with the command arguments.
        return:
            - None.
        """
        # Variables to store the useful arguments
        io = 0
        # Iterate through the arguments
        for i, arg in enumerate(args):
            # Look for the -io option
            if arg == "-io":
                # Save the next argument
                io = int(args[i + 1])
        # Call the GPIO clear method
        self.gpio_toggle(io)


    def cmdRead(self, args : list):
        """
        Handles the read command.
        args:
            - args: list with the command arguments.
        return:
            - None.
        """


    def cmdAdc(self, args : list):
        """
        Handles the adc command.
        args:
            - args: list with the command arguments.
        return:
            - None.
        """


    def cmdLm(self, args : list):
        """
        Handles the lm command.
        args:
            - args: list with the command arguments.
        return:
            - None.
        """


    def cmdDac(self, args : list):
        """
        Handles the dac command.
        args:
            - args: list with the command arguments
        return:
            - None.
        """
        # Variables to store the useful arguments
        ch = 0
        v = 0
        # Iterate through the arguments
        for i, arg in enumerate(args):
            # Look for the -ch option
            if arg == "-ch":
                # Save the next argument
                ch = int(args[i + 1])
            # Look for the -v option
            if arg == "-v":
                # Save the next argument
                v = int(args[i + 1])
        # Call the DAC set method
        self.dac_set(ch, v)


    def cmdSine(self, args : list):
        """
        Handles the sine command.
        args:
            - args: list with the command arguments.
        return:
            - None.
        """
        # Variables to store the useful arguments
        ch = 0
        f = 0
        # Iterate through the arguments
        for i, arg in enumerate(args):
            # Look for the -ch option
            if arg == "-ch":
                # Save the next argument
                ch = int(args[i + 1])
            # Look for the -f option
            if arg == "-f":
                # Save the next argument
                f = int(args[i + 1])
        # Call the DAC sine method
        self.dac_sine(ch, f)


    def cmdTriang(self, args : list):
        """
        Handles the triang command.
        args:
            - args: list with the command arguments.
        return:
            - None.
        """
        # Variables to store the useful arguments
        ch = 0
        f = 0
        # Iterate through the arguments
        for i, arg in enumerate(args):
            # Look for the -ch option
            if arg == "-ch":
                # Save the next argument
                ch = int(args[i + 1])
            # Look for the -f option
            if arg == "-f":
                # Save the next argument
                f = int(args[i + 1])
        # Call the DAC triangular method
        self.dac_triangular(ch, f)


    def cmdPwm(self, args : list):
        """
        Handles the pwm command.
        args:
            - args: list with the command arguments.
        return:
            - None.
        """
        # Variables to store the useful arguments
        ch = 0
        f = 0
        du = 0
        # Iterate through the arguments
        for i, arg in enumerate(args):
            # Look for the -ch option
            if arg == "-ch":
                # Save the next argument
                ch = int(args[i + 1])
            # Look for the -f option
            if arg == "-f":
                # Save the next argument
                f = int(args[i + 1])
            # Look for the -du option
            if arg == "-du":
                # Save the next argument
                du = int(args[i + 1])
        # Call the PWM config method
        self.pwm_config(ch, f, du)


    def cmdBmp(self, args : list):
        """
        Handles the bmp command.
        args:
            - args: list with the command arguments.
        return:
            - None.
        """
        pass


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