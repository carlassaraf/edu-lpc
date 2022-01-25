using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace EDU_LPC
{
    public partial class EDU_LPC : Form
    {
        /* GPIO dictionary */
        Dictionary<string, int> gpioNames = new Dictionary<string, int>();
        /* GPIO indexes: PIO0_16, PIO0_17, PIO0_18, PIO0_19, PIO0_20, PIO0_21, PIO0_22, PIO0_23 */
        const int GPIO_START_INDEX = 16;
        /* Flag to check if the size was received */
        private bool first_byte = true;
        /// <summary>
        /// EDU LPC commands
        /// </summary>
        private enum EDU_LPC_CMD : byte
        {
            config = 0x01,              /* Set GPIO function */
            gpio_clear = 0x02,          /* GPIO clear output */
            gpio_set = 0x03,            /* GPIO set output */
            gpio_toggle = 0x04,         /* GPIO toggle output */
            gpio_read = 0x05,           /* GPIO read value and send */
            adc_read = 0x06,            /* ADC read channel and send */
            lm35_c = 0x07,              /* LM35 read and send temperature in Celsius */
            lm35_f = 0x08,              /* LM35 read and send temperature in Fahrenheit */
            dac_set = 0x09,             /* DAC set output value */
            dac_sine = 0x0a,            /* DAC configure sine wave */
            dac_triangular = 0x0b,      /* DAC configure triangular wave */
            dac_wave = 0x0c,            /* DAC enable or disable wave output */
            pwm_config = 0x0d,          /* PWM configure signal */
            bmp_read = 0x0e				/* BMP180 read values and send */
        };
        /// <summary>
        /// EDU LPC GPIO configuration
        /// </summary>
        private enum GPIO_CONFIG : byte
        {
            input = 0x00,       /* GPIO as input */
            output = 0x01,      /* GPIO as output */
            adc = 0x02,         /* GPIO as ADC */
            dac = 0x03,         /* GPIO as DAC */
            pwm = 0x04			/* GPIO as PWM */
        };
        /* Serial port instance */
        static SerialPort serialPort;
        /* Delegate to update labels */
        delegate void setLblText(string text, Label obj);

        /// <summary>
        /// Constructor
        /// </summary>
        public EDU_LPC()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Load method
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Form_Load(object sender, EventArgs e)
        {
            /* Fill GPIO dictionary */
            for (int i = 0; i < 8; i++) { gpioNames.Add("GPIO" + i.ToString(), GPIO_START_INDEX + i); }
            gpioNames.Add("Relay", 14);
            gpioNames.Add("USER BTN", 4);
            gpioNames.Add("BTN1", 8);
            gpioNames.Add("BTN2", 9);
            gpioNames.Add("BTN3", 13);
            gpioNames.Add("RLED", 34);
            gpioNames.Add("GLED", 32);
            gpioNames.Add("BLED", 33);
            /* Load GPIO list */
            foreach(KeyValuePair<string, int> element in gpioNames) { gpioCmbox.Items.Add(element.Key); }
            /* Load ADC list */
            for (int i = 0; i < 10; i++) { if (i != 2) { adcCmbox.Items.Add("ADC" + i.ToString()); } }
            /* Load PIO */
            for (int i = 16; i < 24; i++) { configPIO.Items.Add("PIO0_" + i.ToString()); }
            /* Load PWM channels */
            for (int i = 0; i < 4; i++) { pwmCmbox.Items.Add("PWM" + i.ToString()); }
            /* Load DAC channels */
            for (int i = 0; i < 2; i++) { dacChannels.Items.Add("DAC" + i.ToString()); }
            /* Initialize the serial port */
            serialPort = new SerialPort();
            /* Set the event to receive data */
            serialPort.DataReceived += new SerialDataReceivedEventHandler(serial_receive);
            /* Load available COMs */
            checkCOMS();
        }

        /// <summary>
        /// Send a GPIO read command when a new GPIO is selected
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void gpioCmbox_SelectedIndexChanged(object sender, EventArgs e)
        {
            /* Send command through serial port */
            gpioCmdHandler((byte)EDU_LPC_CMD.gpio_read);
        }

        /// <summary>
        /// Send a GPIO clear command
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void gpioClear_Click(object sender, EventArgs e) { 
            /* Send command through serial port */
            gpioCmdHandler((byte)EDU_LPC_CMD.gpio_clear);
            /* Update value label */
            gpioValueLbl.Text = "LOW";
        }

        /// <summary>
        /// Semd a GPIO set command
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void gpioSet_Click(object sender, EventArgs e) { 
            /* Send command through serial port */
            gpioCmdHandler((byte)EDU_LPC_CMD.gpio_set);
            /* Update value label */
            gpioValueLbl.Text = "HIGH";
        }

        /// <summary>
        /// Send a GPIO toggle command
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void gpioToggle_Click(object sender, EventArgs e) {
            /* Send command through serial port */
            gpioCmdHandler((byte)EDU_LPC_CMD.gpio_toggle);
            /* Update value label */
            if (gpioValueLbl.Text.Equals("HIGH")) { gpioValueLbl.Text = "LOW"; }
            if (gpioValueLbl.Text.Equals("LOW")) { gpioValueLbl.Text = "HIGH"; }
        }

        /// <summary>
        /// Get the index of the selected item in the ComboBox
        /// </summary>
        /// <param name="cmb">ComboBox to be used</param>
        /// <returns>Index of the object</returns>
        private int getCmbObjectIndex(ComboBox cmb)
        {
            /* Get the selected item as a string */
            string selectedItem = cmb.SelectedItem.ToString();
            /* Get the index of the last character */
            int length = selectedItem.Length - 1;
            /* Get the index of the string as an int */
            return Convert.ToInt16(selectedItem[length] - '0');
        }

        /// <summary>
        /// Checks for new COM connections periodically
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void comChecker_Tick(object sender, EventArgs e) { checkCOMS(); }

        /// <summary>
        /// Connects or disconnects to the selected serial port
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void comConnect_Click(object sender, EventArgs e)
        {
            /* Check if there is a port selected */
            if (comList.SelectedIndex < 0)
            {
                MessageBox.Show("Elija un puerto");
                return;
            }
            /* Check if the port is closed */
            if (!serialPort.IsOpen)
            {
                /* Configure and open the serial port */
                serialPort.PortName = comList.SelectedItem.ToString();
                serialPort.BaudRate = 115200;
                serialPort.DataBits = 8;
                serialPort.StopBits = StopBits.One;
                serialPort.Parity = Parity.None;
                serialPort.ReceivedBytesThreshold = 1;
                serialPort.Open();
                /* Change the button text */
                comConnect.Text = "Disconnect";
                /* Stop timer */
                comChecker.Enabled = false;
            }
            /* If port is open */
            else
            {
                /* Close port */
                serialPort.Close();
                /* Change the button text */
                comConnect.Text = "Connect";
                /* Start timer to update the COM list */
                comChecker.Enabled = true;
            }
        }

        /// <summary>
        /// Fil the function ComboBox with the available GPIO functions
        /// when a PIO is selected.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void configPIO_SelectedIndexChanged(object sender, EventArgs e)
        {
            /* PIO special indexes */
            const int PIO0_16_INDEX = 0;
            const int PIO0_17_INDEX = 1;
            const int PIO0_19_INDEX = 4;
            /* Common functions */
            string[] items = { "INPUT", "OUTPUT", "ADC", "PWM" };
            /* Clear the previous functions */
            configFunction.Items.Clear();
            /* Add every common item */
            foreach (string item in items) { configFunction.Items.Add(item); }
            /* Remove ADC function if PIO0_16 */
            if (configPIO.SelectedIndex == PIO0_16_INDEX) { configFunction.Items.Remove("ADC"); }
            /* Add DAC function if PIO0_17 */
            if (configPIO.SelectedIndex == PIO0_17_INDEX) { configFunction.Items.Add("DAC"); }
            /* Remove PWM function for PIO0_20 to PIO0_23 */
            if (configPIO.SelectedIndex > PIO0_19_INDEX) { configFunction.Items.Remove("PWM"); }
            /* Clear the function selected */
            configFunction.Text = "";
        }

        /// <summary>
        /// Send a GPIO config command
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void configBtn_Click(object sender, EventArgs e)
        {
            /* Get the selected PIO index */
            int index = configPIO.SelectedIndex + GPIO_START_INDEX;
            /* Get the selected function as a string */
            string function = configFunction.SelectedItem.ToString();
            /* Prepare the data buffer */
            byte[] buff = { 
                (byte)EDU_LPC_CMD.config,       /* Command */
                (byte)index,                    /* PIO index */
                gpioFunctionIndex(function)     /* Function index */
            };
            /* Send data */
            sendSerialData(buff);
        }

        /// <summary>
        /// Send an ADC read command
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void adcCmbox_SelectedIndexChanged(object sender, EventArgs e)
        {
            /* Get the selected ADC channel */
            int channel = getCmbObjectIndex(adcCmbox);
            /* Prepare the data buffer */
            byte[] buff = { 
                (byte)EDU_LPC_CMD.adc_read, /* Command */
                (byte)channel               /* ADC chanel */
            };
            /* Send data */
            sendSerialData(buff);
        }

        /// <summary>
        /// Send an LM35 read Celsius command
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void lm35C_CheckedChanged(object sender, EventArgs e)
        {
            /* Check if the RadioButton was checked */
            if (lm35C.Checked)
            {
                /* Uncheck the other RadioButton */
                lm35F.Checked = false;
                /* Prepare the data buffer */
                byte[] buff = { 
                    (byte)EDU_LPC_CMD.lm35_c    /* Command */
                };
                /* Send data */
                sendSerialData(buff);
            }
        }

        /// <summary>
        /// Send an LM35 read Fahrenheit command
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void lm35F_CheckedChanged(object sender, EventArgs e)
        {
            /* Check if the RadioButton was checked */
            if (lm35F.Checked)
            {
                /* Uncheck the other RadioButton */
                lm35C.Checked = false;
                /* Prepare the data buffer */
                byte[] buff = { 
                    (byte)EDU_LPC_CMD.lm35_f    /* Command */
                };
                /* Send data */
                sendSerialData(buff);
            }
        }

        /// <summary>
        /// Send a DAC set command
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void dacValue_Scroll(object sender, ScrollEventArgs e)
        {
            /* Check if the scroll event finished and there is a channel selected */
            if (e.Type == ScrollEventType.EndScroll && dacChannels.SelectedIndex > -1)
            {
                /* Set the ScrollBar value to the label */
                dacValueLbl.Text = dacValue.Value.ToString();
                /* Get 2-bit high value */
                byte high = (byte)((dacValue.Value & 0x300) >> 8);
                /* Get 8-bit lower value */
                byte low = (byte)(dacValue.Value & 0xff);
                /* DAC1 selected */
                byte dac_index = (byte)dacChannels.SelectedIndex;
                /* Prepare the data buffer */
                byte[] buff = { 
                    (byte)EDU_LPC_CMD.dac_set,  /* Command */
                    dac_index,                  /* DAC index */
                    high,                       /* High DAC value */
                    low                         /* Low DAC value */
                };
                /* Send data */
                sendSerialData(buff);
            }
        }

        /// <summary>
        /// Send a DAC sine wave command
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void dacSine_CheckedChanged(object sender, EventArgs e)
        {
            /* Check if the RadioButton was checked */
            if (dacSine.Checked == true)
            {
                /* Uncheck the other RadioButton */
                dacTriangular.Checked = false;
                /* Get the data to send */
                dac_wave_type_handler((byte)EDU_LPC_CMD.dac_sine);
            }
        }

        /// <summary>
        /// Send a DAC triangular wave command
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void dacTriangular_CheckedChanged(object sender, EventArgs e)
        {
            /* Check if the RadioButton was checked */
            if (dacTriangular.Checked == true)
            {
                /* Uncheck the other RadioButton */
                dacSine.Checked = false;
                /* Get the data to send */
                dac_wave_type_handler((byte)EDU_LPC_CMD.dac_triangular);
            }
        }

        /// <summary>
        /// Send a DAC wave on/off command
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void dacOn_CheckedChanged(object sender, EventArgs e)
        {
            /* If RadioButton was checked, change text to On */
            if (dacOn.Checked) { dacOn.Text = "On"; }
            /* If RadioButton was not checked, change text to Off */
            else { dacOn.Text = "Off"; }
            /* DAC1 selected */
            byte dac_index = (byte)dacChannels.SelectedIndex;
            /* Prepare data buffer */
            byte[] buff = {
                (byte)EDU_LPC_CMD.dac_wave,
                dac_index,
                dacOn.Checked? (byte)0x01 : (byte)0x00
            };
            /* Send data */
            sendSerialData(buff);
        }

        /// <summary>
        /// Send a GPIO clear, set or toggle command
        /// </summary>
        /// <param name="cmd">
        /// Command to be sent
        /// </param>
        private void gpioCmdHandler(byte cmd)
        {
            /* Check that there is a GPIO selected */
            if (gpioCmbox.SelectedIndex < 0)
            {
                MessageBox.Show("Select a GPIO first!");
                return;
            }
            /* Get the GPIO index */
            int gpio = 0;
            string item = gpioCmbox.SelectedItem.ToString();
            /* Look in the gpioNames dictionary for a match */
            foreach (KeyValuePair<string, int> element in gpioNames)
            {
                /* If the key matches the selected element, get its value */
                if (element.Key == item) { 
                    gpio = element.Value;
                    break;
                }
            }
            /* Prepare data buffer */
            byte[] buff = { 
                cmd,            /* Command */
                (byte)gpio      /* PIO index */
            };
            /* Send data */
            sendSerialData(buff);
        }

        /// <summary>
        /// Gets the GPIO function as an integer
        /// </summary>
        /// <param name="function">
        /// PIO function as a string
        /// </param>
        /// <returns>
        /// PIO function as an integer
        /// </returns>
        private byte gpioFunctionIndex(string function)
        {
            /* Check what function is and return a different integer */
            if (function.Equals("INPUT")) { return (byte)GPIO_CONFIG.input; }
            if (function.Equals("OUTPUT")) { return (byte)GPIO_CONFIG.output; }
            if (function.Equals("ADC")) { return (byte)GPIO_CONFIG.adc; }
            if (function.Equals("DAC")) { return (byte)GPIO_CONFIG.dac; }
            if (function.Equals("PWM")) { return (byte)GPIO_CONFIG.pwm; }
            return 0xff;
        }

        /// <summary>
        /// Adds the header and checksum bytes to the
        /// provided buffer and sends the data through serial.
        /// </summary>
        /// <param name="buffer"></param>
        private void sendSerialData(byte[] buffer)
        {
            /* Check if the serial port is open */
            if (!serialPort.IsOpen)
            {
                /* Alert user */
                MessageBox.Show("Seleccione un puerto para conectarse primero.");
                return;
            }
            /* Calculate length of data */
            int length = buffer.Length + 1;
            /* Create an auxiliary list */
            List<byte> auxBuff = new List<byte>();
            /* Variable to calculate checksum */
            int checksum = 0;
            /* Add length as first byte */
            auxBuff.Add(Convert.ToByte(length));
            /* Copy buffer in list */
            foreach (byte b in buffer)
            {
                /* Acumulate bytes */
                checksum += b;
                /* Add byte to list */
                auxBuff.Add(b);
            }
            /* Get lower 8 bits of the checksum */
            checksum &= 0xff;
            /* Calculate checksum */
            checksum = 0xff - checksum;
            /* Add checksum to list */
            auxBuff.Add(Convert.ToByte(checksum));
            /* Convert list to array */
            byte[] buff = auxBuff.ToArray();
            /* Wait for one byte to trigger event */
            serialPort.ReceivedBytesThreshold = 1;
            /* Set flag to recognize the first byte */
            first_byte = true;
            /* Send data through serial port */
            serialPort.Write(buff, 0, buff.Length);
        }

        /// <summary>
        /// Look for available ports and add them to the ComboBox
        /// </summary>
        private void checkCOMS()
        {
            /* Try to get available ports */
            try
            {
                /* Get port names availables */
                string[] ports = SerialPort.GetPortNames();
                /* Iterate through every name */
                foreach (string port in ports)
                {
                    /* If the port is not in the list, add it */
                    if (!comList.Items.Contains(port)) { comList.Items.Add(port); }
                }
            }
            /* If no ports are available, let the user know */
            catch { MessageBox.Show("No hay puertos disponibles!"); }
        }

        /// <summary>
        /// Serial receive event that triggers when a byte or more aree received
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void serial_receive(object sender, SerialDataReceivedEventArgs e)
        {
            /* Create array to hold the incomming data */
            byte[] buff = new byte[serialPort.ReceivedBytesThreshold];
            /* Read all the bytes received */
            serialPort.Read(buff, 0, serialPort.ReceivedBytesThreshold);
            /* Check if it is the size byte */
            if (first_byte)
            {
                /* Clear flag */
                first_byte = false;
                /* Set the number of bytes to be waited */
                serialPort.ReceivedBytesThreshold = buff[0];
            }
            /* If size byte was already received */
            else
            {
                /* Auxiliary vairbles */
                byte high, low;
                byte presHigh, presMid, presLow;
                float aux;
                /* Check what command it is */
                switch ((EDU_LPC_CMD)buff[0])
                {
                    /* GPIO read command */
                    case EDU_LPC_CMD.gpio_read:
                        /* Get the GPIO value as a string */
                        string val = (buff[1] == 1) ? "High" : "Low";
                        /* Call delegate to update label */
                        update(val, gpioValueLbl);
                        break;
                    /* GPIO ADC read command */
                    case EDU_LPC_CMD.adc_read:
                        /* Get the high and low values */
                        high = buff[1];
                        low = buff[2];
                        /* Build the 12-bit ADC value */
                        int adc = high * 0xff + low;
                        /* Call delagate to update label */
                        update(adc.ToString(), adcValueLbl);
                        break;
                    /* LM35 Celsius and Fahrenheit commands */
                    case EDU_LPC_CMD.lm35_c:
                    case EDU_LPC_CMD.lm35_f:
                        /* Get the integer and decimal temperature values */
                        high = buff[1];
                        low = buff[2];
                        /* Build temperature */
                        aux = high + ((float)low) / 100;
                        /* Call delegate to update label */
                        update(aux.ToString(), lm35ValueLbl);
                        break;
                    /* BMP180 read command */
                    case EDU_LPC_CMD.bmp_read:
                        /* Get the high, mid and low pressure values */
                        presHigh = buff[1];
                        presMid = buff[2];
                        presLow = buff[3];
                        /* Get the integer and decimal temperature values */
                        high = buff[4];
                        low = buff[5];
                        /* Build the pressure value */
                        Int32 pres = (presHigh * 0xffff) + (presMid * 0xff) + presLow;
                        /* Build the temperature value */
                        aux = high + ((float)low) / 100;
                        /* Call delegate to update both labels */
                        update(Convert.ToString(pres) + " Pa", bmpPres);
                        update(Convert.ToString(aux) + " °C", bmpTemp);
                        break;
                }
            }
        }

        /// <summary>
        /// Calls the delegate to update the label
        /// </summary>
        /// <param name="text">
        /// Text to be set in the label
        /// </param>
        /// <param name="obj">
        /// Label to be updated
        /// </param>
        private void update(string text, Label obj)
        {
            /* Check if an invoke is required fofr the object */
            if (obj.InvokeRequired)
            {
                /* Create a delegate */
                setLblText d = new setLblText(update);
                /* Invoke the method */
                this.Invoke(d, new object[] { text, obj });
            }
            /* When the object is in the same thread, update the text */
            else { obj.Text = text; }
        }

        /// <summary>
        /// Collects the data necessary to send to the DAC
        /// </summary>
        /// <param name="cmd">
        /// DAC command to be executed
        /// </param>
        private void dac_wave_type_handler(byte cmd)
        {
            /* Check if a frequency was entered */
            if (dacFrequency.Text == "")
            {
                /* Alert the user */
                MessageBox.Show("Ingrese una frequencia entera entre 1 y 1KHz primero");
                return;
            }
            /* Try to get the frequency */
            try
            {
                /* Get the frequency from the TextBox */
                int freq = Convert.ToInt32(dacFrequency.Text);
                /* Validate the frequency */
                if (freq < 1 || freq > 1000) { throw new FormatException(); }
                /* Get frequency high value */
                byte high = (byte)((freq & 0x300) >> 8);
                /* Get frequency low value */
                byte low = (byte)(freq & 0xff);
                /* Selected DAC channel */
                byte dac_index = (byte)dacChannels.SelectedIndex;
                /* Build data buffer */
                byte[] buff = { 
                    cmd,        /* Command */
                    dac_index,  /* DAC channel */
                    high,       /* High frequency value */
                    low         /* Low frequency value */
                };
                /* Send data */
                sendSerialData(buff);
            }
            /* If an invalid frequency or format was typed, alert the user */
            catch (FormatException) { MessageBox.Show("Ingrese una frequencia entera entre 1 y 1KHz"); }
        }

        /// <summary>
        /// Send a PWM config command
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void pwmConfig_Click(object sender, EventArgs e)
        {
            /* Check both duty and frequency fields were typed */
            if (pwmFreq.Text == "" || pwmDuty.Text == "")
            {
                /* Alert the user */
                MessageBox.Show("Llene el campo de frecuencia y ciclo de actividad primero");
                return;
            }
            /* Try to get frequency and duty valyes */
            try
            {
                /* Check a PWM channel was selected */
                if (pwmCmbox.SelectedIndex < 0)
                {
                    /* Alert the user */
                    MessageBox.Show("Elija un PWM primero");
                    return;
                }
                /* Get the PWM channel */
                byte index = (byte)pwmCmbox.SelectedIndex;
                /* Get the frequency */
                int freq = Convert.ToInt32(pwmFreq.Text);
                /* Get the high frequency value */
                byte high = (byte)((freq & 0xff00) >> 8);
                /* Get the low frequency value */
                byte low = (byte)(freq & 0xff);
                /* Get the duty value */
                byte duty = Convert.ToByte(pwmDuty.Text);
                /* Build data buffer */
                byte[] buff = { 
                    (byte)EDU_LPC_CMD.pwm_config,   /* Command */
                    index,                          /* PWM channel */
                    high,                           /* High frequency value */
                    low,                            /* Low frequency value */
                    duty                            /* Duty cycle value */
                };
                sendSerialData(buff);
            }
            /* If an invalid frequency/duty or format was typed, alert the user */
            catch (FormatException) { MessageBox.Show("Ingrese una frequencia entera entre 1 y 1KHz y un ciclo de trabajo entre 0 y 100"); }
        }

        /// <summary>
        /// Send a BMP180 read command
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void bmpRead_Click(object sender, EventArgs e)
        {
            /* Build data buffer */
            byte[] buff = { 
                (byte)EDU_LPC_CMD.bmp_read  /* Command */
            };
            /* Send data */
            sendSerialData(buff);
        }

        /// <summary>
        /// Resets the RadioButtons and CheckBox
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void dacChannels_SelectedIndexChanged(object sender, EventArgs e)
        {
            dacSine.Checked = false;
            dacTriangular.Checked = false;
            dacOn.Checked = false;
        }
    }
}
