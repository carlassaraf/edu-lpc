
namespace EDU_LPC
{
    partial class EDU_LPC
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.gpioPanel = new System.Windows.Forms.Panel();
            this.gpioToggle = new System.Windows.Forms.Button();
            this.gpioSet = new System.Windows.Forms.Button();
            this.gpioClear = new System.Windows.Forms.Button();
            this.gpioValueLbl = new System.Windows.Forms.Label();
            this.gpioLabel = new System.Windows.Forms.Label();
            this.gpioCmbox = new System.Windows.Forms.ComboBox();
            this.adcPanel = new System.Windows.Forms.Panel();
            this.lm35F = new System.Windows.Forms.RadioButton();
            this.lm35C = new System.Windows.Forms.RadioButton();
            this.lm35ValueLbl = new System.Windows.Forms.Label();
            this.lm35Lbl = new System.Windows.Forms.Label();
            this.adcValueLbl = new System.Windows.Forms.Label();
            this.adcCmbox = new System.Windows.Forms.ComboBox();
            this.adcLabel = new System.Windows.Forms.Label();
            this.dacPanel = new System.Windows.Forms.Panel();
            this.dacTriangular = new System.Windows.Forms.RadioButton();
            this.dacSine = new System.Windows.Forms.RadioButton();
            this.dacOn = new System.Windows.Forms.CheckBox();
            this.dacFrequency = new System.Windows.Forms.TextBox();
            this.dacValueLbl = new System.Windows.Forms.Label();
            this.dacValue = new System.Windows.Forms.HScrollBar();
            this.dacLbl = new System.Windows.Forms.Label();
            this.comPanel = new System.Windows.Forms.Panel();
            this.comConnect = new System.Windows.Forms.Button();
            this.comList = new System.Windows.Forms.ComboBox();
            this.comLabel = new System.Windows.Forms.Label();
            this.comChecker = new System.Windows.Forms.Timer(this.components);
            this.configPanel = new System.Windows.Forms.Panel();
            this.configBtn = new System.Windows.Forms.Button();
            this.configFunction = new System.Windows.Forms.ComboBox();
            this.configPIO = new System.Windows.Forms.ComboBox();
            this.configLabel = new System.Windows.Forms.Label();
            this.pwmPanel = new System.Windows.Forms.Panel();
            this.pwmConfig = new System.Windows.Forms.Button();
            this.pwmDutyLbl = new System.Windows.Forms.Label();
            this.pwmFreqLbl = new System.Windows.Forms.Label();
            this.pwmDuty = new System.Windows.Forms.TextBox();
            this.pwmFreq = new System.Windows.Forms.TextBox();
            this.pwmCmbox = new System.Windows.Forms.ComboBox();
            this.pwmLabel = new System.Windows.Forms.Label();
            this.i2cPanel = new System.Windows.Forms.Panel();
            this.bmpRead = new System.Windows.Forms.Button();
            this.bmpTemp = new System.Windows.Forms.Label();
            this.bmpPres = new System.Windows.Forms.Label();
            this.bmpLabel = new System.Windows.Forms.Label();
            this.i2cLabel = new System.Windows.Forms.Label();
            this.dacChannels = new System.Windows.Forms.ComboBox();
            this.gpioPanel.SuspendLayout();
            this.adcPanel.SuspendLayout();
            this.dacPanel.SuspendLayout();
            this.comPanel.SuspendLayout();
            this.configPanel.SuspendLayout();
            this.pwmPanel.SuspendLayout();
            this.i2cPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // gpioPanel
            // 
            this.gpioPanel.BackColor = System.Drawing.SystemColors.Desktop;
            this.gpioPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.gpioPanel.Controls.Add(this.gpioToggle);
            this.gpioPanel.Controls.Add(this.gpioSet);
            this.gpioPanel.Controls.Add(this.gpioClear);
            this.gpioPanel.Controls.Add(this.gpioValueLbl);
            this.gpioPanel.Controls.Add(this.gpioLabel);
            this.gpioPanel.Controls.Add(this.gpioCmbox);
            this.gpioPanel.Location = new System.Drawing.Point(23, 209);
            this.gpioPanel.Name = "gpioPanel";
            this.gpioPanel.Size = new System.Drawing.Size(166, 96);
            this.gpioPanel.TabIndex = 0;
            // 
            // gpioToggle
            // 
            this.gpioToggle.Location = new System.Drawing.Point(101, 62);
            this.gpioToggle.Name = "gpioToggle";
            this.gpioToggle.Size = new System.Drawing.Size(50, 25);
            this.gpioToggle.TabIndex = 5;
            this.gpioToggle.Text = "Toggle";
            this.gpioToggle.UseVisualStyleBackColor = true;
            this.gpioToggle.Click += new System.EventHandler(this.gpioToggle_Click);
            // 
            // gpioSet
            // 
            this.gpioSet.Location = new System.Drawing.Point(57, 62);
            this.gpioSet.Name = "gpioSet";
            this.gpioSet.Size = new System.Drawing.Size(38, 25);
            this.gpioSet.TabIndex = 4;
            this.gpioSet.Text = "Set";
            this.gpioSet.UseVisualStyleBackColor = true;
            this.gpioSet.Click += new System.EventHandler(this.gpioSet_Click);
            // 
            // gpioClear
            // 
            this.gpioClear.Location = new System.Drawing.Point(13, 62);
            this.gpioClear.Name = "gpioClear";
            this.gpioClear.Size = new System.Drawing.Size(39, 25);
            this.gpioClear.TabIndex = 3;
            this.gpioClear.Text = "Clear";
            this.gpioClear.UseVisualStyleBackColor = true;
            this.gpioClear.Click += new System.EventHandler(this.gpioClear_Click);
            // 
            // gpioValueLbl
            // 
            this.gpioValueLbl.AutoSize = true;
            this.gpioValueLbl.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.gpioValueLbl.Location = new System.Drawing.Point(81, 39);
            this.gpioValueLbl.Name = "gpioValueLbl";
            this.gpioValueLbl.Size = new System.Drawing.Size(70, 13);
            this.gpioValueLbl.TabIndex = 2;
            this.gpioValueLbl.Text = "UNDEFINED";
            // 
            // gpioLabel
            // 
            this.gpioLabel.AutoSize = true;
            this.gpioLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold);
            this.gpioLabel.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.gpioLabel.Location = new System.Drawing.Point(13, 8);
            this.gpioLabel.Name = "gpioLabel";
            this.gpioLabel.Size = new System.Drawing.Size(53, 20);
            this.gpioLabel.TabIndex = 1;
            this.gpioLabel.Text = "GPIO";
            // 
            // gpioCmbox
            // 
            this.gpioCmbox.FormattingEnabled = true;
            this.gpioCmbox.Location = new System.Drawing.Point(13, 36);
            this.gpioCmbox.Name = "gpioCmbox";
            this.gpioCmbox.Size = new System.Drawing.Size(55, 21);
            this.gpioCmbox.TabIndex = 0;
            this.gpioCmbox.Text = "GPIOn";
            this.gpioCmbox.SelectedIndexChanged += new System.EventHandler(this.gpioCmbox_SelectedIndexChanged);
            // 
            // adcPanel
            // 
            this.adcPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.adcPanel.Controls.Add(this.lm35F);
            this.adcPanel.Controls.Add(this.lm35C);
            this.adcPanel.Controls.Add(this.lm35ValueLbl);
            this.adcPanel.Controls.Add(this.lm35Lbl);
            this.adcPanel.Controls.Add(this.adcValueLbl);
            this.adcPanel.Controls.Add(this.adcCmbox);
            this.adcPanel.Controls.Add(this.adcLabel);
            this.adcPanel.Location = new System.Drawing.Point(204, 20);
            this.adcPanel.Name = "adcPanel";
            this.adcPanel.Size = new System.Drawing.Size(166, 122);
            this.adcPanel.TabIndex = 1;
            // 
            // lm35F
            // 
            this.lm35F.AutoSize = true;
            this.lm35F.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.lm35F.Location = new System.Drawing.Point(123, 77);
            this.lm35F.Name = "lm35F";
            this.lm35F.Size = new System.Drawing.Size(35, 17);
            this.lm35F.TabIndex = 6;
            this.lm35F.TabStop = true;
            this.lm35F.Text = "°F";
            this.lm35F.UseVisualStyleBackColor = true;
            this.lm35F.CheckedChanged += new System.EventHandler(this.lm35F_CheckedChanged);
            // 
            // lm35C
            // 
            this.lm35C.AutoSize = true;
            this.lm35C.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.lm35C.Location = new System.Drawing.Point(89, 77);
            this.lm35C.Name = "lm35C";
            this.lm35C.Size = new System.Drawing.Size(36, 17);
            this.lm35C.TabIndex = 5;
            this.lm35C.TabStop = true;
            this.lm35C.Text = "°C";
            this.lm35C.UseVisualStyleBackColor = true;
            this.lm35C.CheckedChanged += new System.EventHandler(this.lm35C_CheckedChanged);
            // 
            // lm35ValueLbl
            // 
            this.lm35ValueLbl.AutoSize = true;
            this.lm35ValueLbl.BackColor = System.Drawing.SystemColors.Control;
            this.lm35ValueLbl.Location = new System.Drawing.Point(51, 78);
            this.lm35ValueLbl.Name = "lm35ValueLbl";
            this.lm35ValueLbl.Size = new System.Drawing.Size(31, 13);
            this.lm35ValueLbl.TabIndex = 4;
            this.lm35ValueLbl.Text = "UND";
            // 
            // lm35Lbl
            // 
            this.lm35Lbl.AutoSize = true;
            this.lm35Lbl.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.lm35Lbl.Location = new System.Drawing.Point(15, 78);
            this.lm35Lbl.Name = "lm35Lbl";
            this.lm35Lbl.Size = new System.Drawing.Size(34, 13);
            this.lm35Lbl.TabIndex = 3;
            this.lm35Lbl.Text = "LM35";
            // 
            // adcValueLbl
            // 
            this.adcValueLbl.AutoSize = true;
            this.adcValueLbl.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.adcValueLbl.Location = new System.Drawing.Point(81, 45);
            this.adcValueLbl.Name = "adcValueLbl";
            this.adcValueLbl.Size = new System.Drawing.Size(70, 13);
            this.adcValueLbl.TabIndex = 2;
            this.adcValueLbl.Text = "UNDEFINED";
            // 
            // adcCmbox
            // 
            this.adcCmbox.FormattingEnabled = true;
            this.adcCmbox.Location = new System.Drawing.Point(13, 42);
            this.adcCmbox.Name = "adcCmbox";
            this.adcCmbox.Size = new System.Drawing.Size(55, 21);
            this.adcCmbox.TabIndex = 1;
            this.adcCmbox.Text = "ADCn";
            this.adcCmbox.SelectedIndexChanged += new System.EventHandler(this.adcCmbox_SelectedIndexChanged);
            // 
            // adcLabel
            // 
            this.adcLabel.AutoSize = true;
            this.adcLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold);
            this.adcLabel.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.adcLabel.Location = new System.Drawing.Point(13, 11);
            this.adcLabel.Name = "adcLabel";
            this.adcLabel.Size = new System.Drawing.Size(46, 20);
            this.adcLabel.TabIndex = 0;
            this.adcLabel.Text = "ADC";
            // 
            // dacPanel
            // 
            this.dacPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.dacPanel.Controls.Add(this.dacChannels);
            this.dacPanel.Controls.Add(this.dacTriangular);
            this.dacPanel.Controls.Add(this.dacSine);
            this.dacPanel.Controls.Add(this.dacOn);
            this.dacPanel.Controls.Add(this.dacFrequency);
            this.dacPanel.Controls.Add(this.dacValueLbl);
            this.dacPanel.Controls.Add(this.dacValue);
            this.dacPanel.Controls.Add(this.dacLbl);
            this.dacPanel.Location = new System.Drawing.Point(204, 148);
            this.dacPanel.Name = "dacPanel";
            this.dacPanel.Size = new System.Drawing.Size(166, 157);
            this.dacPanel.TabIndex = 2;
            // 
            // dacTriangular
            // 
            this.dacTriangular.AutoSize = true;
            this.dacTriangular.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.dacTriangular.Location = new System.Drawing.Point(79, 131);
            this.dacTriangular.Name = "dacTriangular";
            this.dacTriangular.Size = new System.Drawing.Size(72, 17);
            this.dacTriangular.TabIndex = 6;
            this.dacTriangular.Text = "Triangular";
            this.dacTriangular.UseVisualStyleBackColor = true;
            this.dacTriangular.CheckedChanged += new System.EventHandler(this.dacTriangular_CheckedChanged);
            // 
            // dacSine
            // 
            this.dacSine.AutoSize = true;
            this.dacSine.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.dacSine.Location = new System.Drawing.Point(13, 131);
            this.dacSine.Name = "dacSine";
            this.dacSine.Size = new System.Drawing.Size(46, 17);
            this.dacSine.TabIndex = 5;
            this.dacSine.Text = "Sine";
            this.dacSine.UseVisualStyleBackColor = true;
            this.dacSine.CheckedChanged += new System.EventHandler(this.dacSine_CheckedChanged);
            // 
            // dacOn
            // 
            this.dacOn.AutoSize = true;
            this.dacOn.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.dacOn.Location = new System.Drawing.Point(111, 105);
            this.dacOn.Name = "dacOn";
            this.dacOn.Size = new System.Drawing.Size(40, 17);
            this.dacOn.TabIndex = 4;
            this.dacOn.Text = "Off";
            this.dacOn.UseVisualStyleBackColor = true;
            this.dacOn.CheckedChanged += new System.EventHandler(this.dacOn_CheckedChanged);
            // 
            // dacFrequency
            // 
            this.dacFrequency.Location = new System.Drawing.Point(13, 103);
            this.dacFrequency.Name = "dacFrequency";
            this.dacFrequency.Size = new System.Drawing.Size(92, 20);
            this.dacFrequency.TabIndex = 3;
            this.dacFrequency.Text = "Frequency";
            // 
            // dacValueLbl
            // 
            this.dacValueLbl.AutoSize = true;
            this.dacValueLbl.Font = new System.Drawing.Font("Segoe UI", 10F);
            this.dacValueLbl.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.dacValueLbl.Location = new System.Drawing.Point(119, 71);
            this.dacValueLbl.Name = "dacValueLbl";
            this.dacValueLbl.Size = new System.Drawing.Size(17, 19);
            this.dacValueLbl.TabIndex = 2;
            this.dacValueLbl.Text = "0";
            // 
            // dacValue
            // 
            this.dacValue.Location = new System.Drawing.Point(13, 71);
            this.dacValue.Maximum = 1023;
            this.dacValue.Name = "dacValue";
            this.dacValue.Size = new System.Drawing.Size(103, 20);
            this.dacValue.TabIndex = 1;
            this.dacValue.Scroll += new System.Windows.Forms.ScrollEventHandler(this.dacValue_Scroll);
            // 
            // dacLbl
            // 
            this.dacLbl.AutoSize = true;
            this.dacLbl.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold);
            this.dacLbl.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.dacLbl.Location = new System.Drawing.Point(13, 11);
            this.dacLbl.Name = "dacLbl";
            this.dacLbl.Size = new System.Drawing.Size(46, 20);
            this.dacLbl.TabIndex = 0;
            this.dacLbl.Text = "DAC";
            // 
            // comPanel
            // 
            this.comPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.comPanel.Controls.Add(this.comConnect);
            this.comPanel.Controls.Add(this.comList);
            this.comPanel.Controls.Add(this.comLabel);
            this.comPanel.Location = new System.Drawing.Point(23, 20);
            this.comPanel.Name = "comPanel";
            this.comPanel.Size = new System.Drawing.Size(166, 72);
            this.comPanel.TabIndex = 3;
            // 
            // comConnect
            // 
            this.comConnect.Location = new System.Drawing.Point(75, 37);
            this.comConnect.Name = "comConnect";
            this.comConnect.Size = new System.Drawing.Size(75, 21);
            this.comConnect.TabIndex = 2;
            this.comConnect.Text = "Connect";
            this.comConnect.UseVisualStyleBackColor = true;
            this.comConnect.Click += new System.EventHandler(this.comConnect_Click);
            // 
            // comList
            // 
            this.comList.FormattingEnabled = true;
            this.comList.Location = new System.Drawing.Point(14, 37);
            this.comList.Name = "comList";
            this.comList.Size = new System.Drawing.Size(56, 21);
            this.comList.TabIndex = 1;
            this.comList.Text = "COMn";
            // 
            // comLabel
            // 
            this.comLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold);
            this.comLabel.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.comLabel.Location = new System.Drawing.Point(10, 9);
            this.comLabel.Name = "comLabel";
            this.comLabel.Size = new System.Drawing.Size(60, 20);
            this.comLabel.TabIndex = 0;
            this.comLabel.Text = "COM";
            // 
            // comChecker
            // 
            this.comChecker.Enabled = true;
            this.comChecker.Interval = 1000;
            this.comChecker.Tick += new System.EventHandler(this.comChecker_Tick);
            // 
            // configPanel
            // 
            this.configPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.configPanel.Controls.Add(this.configBtn);
            this.configPanel.Controls.Add(this.configFunction);
            this.configPanel.Controls.Add(this.configPIO);
            this.configPanel.Controls.Add(this.configLabel);
            this.configPanel.Location = new System.Drawing.Point(23, 98);
            this.configPanel.Name = "configPanel";
            this.configPanel.Size = new System.Drawing.Size(166, 105);
            this.configPanel.TabIndex = 4;
            // 
            // configBtn
            // 
            this.configBtn.Location = new System.Drawing.Point(45, 68);
            this.configBtn.Name = "configBtn";
            this.configBtn.Size = new System.Drawing.Size(75, 23);
            this.configBtn.TabIndex = 3;
            this.configBtn.Text = "Configure";
            this.configBtn.UseVisualStyleBackColor = true;
            this.configBtn.Click += new System.EventHandler(this.configBtn_Click);
            // 
            // configFunction
            // 
            this.configFunction.FormattingEnabled = true;
            this.configFunction.Location = new System.Drawing.Point(93, 41);
            this.configFunction.Name = "configFunction";
            this.configFunction.Size = new System.Drawing.Size(57, 21);
            this.configFunction.TabIndex = 2;
            this.configFunction.Text = "Function";
            // 
            // configPIO
            // 
            this.configPIO.FormattingEnabled = true;
            this.configPIO.Location = new System.Drawing.Point(15, 41);
            this.configPIO.Name = "configPIO";
            this.configPIO.Size = new System.Drawing.Size(72, 21);
            this.configPIO.TabIndex = 1;
            this.configPIO.Text = "PIO0_n";
            this.configPIO.SelectedIndexChanged += new System.EventHandler(this.configPIO_SelectedIndexChanged);
            // 
            // configLabel
            // 
            this.configLabel.AutoSize = true;
            this.configLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold);
            this.configLabel.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.configLabel.Location = new System.Drawing.Point(11, 11);
            this.configLabel.Name = "configLabel";
            this.configLabel.Size = new System.Drawing.Size(77, 20);
            this.configLabel.TabIndex = 0;
            this.configLabel.Text = "CONFIG";
            // 
            // pwmPanel
            // 
            this.pwmPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pwmPanel.Controls.Add(this.pwmConfig);
            this.pwmPanel.Controls.Add(this.pwmDutyLbl);
            this.pwmPanel.Controls.Add(this.pwmFreqLbl);
            this.pwmPanel.Controls.Add(this.pwmDuty);
            this.pwmPanel.Controls.Add(this.pwmFreq);
            this.pwmPanel.Controls.Add(this.pwmCmbox);
            this.pwmPanel.Controls.Add(this.pwmLabel);
            this.pwmPanel.Location = new System.Drawing.Point(387, 20);
            this.pwmPanel.Name = "pwmPanel";
            this.pwmPanel.Size = new System.Drawing.Size(166, 141);
            this.pwmPanel.TabIndex = 5;
            // 
            // pwmConfig
            // 
            this.pwmConfig.Location = new System.Drawing.Point(93, 44);
            this.pwmConfig.Name = "pwmConfig";
            this.pwmConfig.Size = new System.Drawing.Size(57, 21);
            this.pwmConfig.TabIndex = 8;
            this.pwmConfig.Text = "Config";
            this.pwmConfig.UseVisualStyleBackColor = true;
            this.pwmConfig.Click += new System.EventHandler(this.pwmConfig_Click);
            // 
            // pwmDutyLbl
            // 
            this.pwmDutyLbl.AutoSize = true;
            this.pwmDutyLbl.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.pwmDutyLbl.Location = new System.Drawing.Point(93, 86);
            this.pwmDutyLbl.Name = "pwmDutyLbl";
            this.pwmDutyLbl.Size = new System.Drawing.Size(46, 13);
            this.pwmDutyLbl.TabIndex = 7;
            this.pwmDutyLbl.Text = "Duty [%]";
            // 
            // pwmFreqLbl
            // 
            this.pwmFreqLbl.AutoSize = true;
            this.pwmFreqLbl.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.pwmFreqLbl.Location = new System.Drawing.Point(12, 86);
            this.pwmFreqLbl.Name = "pwmFreqLbl";
            this.pwmFreqLbl.Size = new System.Drawing.Size(79, 13);
            this.pwmFreqLbl.TabIndex = 6;
            this.pwmFreqLbl.Text = "Frequency [Hz]";
            // 
            // pwmDuty
            // 
            this.pwmDuty.Location = new System.Drawing.Point(93, 101);
            this.pwmDuty.Name = "pwmDuty";
            this.pwmDuty.Size = new System.Drawing.Size(57, 20);
            this.pwmDuty.TabIndex = 5;
            this.pwmDuty.Text = "Duty";
            // 
            // pwmFreq
            // 
            this.pwmFreq.Location = new System.Drawing.Point(14, 102);
            this.pwmFreq.Name = "pwmFreq";
            this.pwmFreq.Size = new System.Drawing.Size(73, 20);
            this.pwmFreq.TabIndex = 4;
            this.pwmFreq.Text = "Frequency";
            // 
            // pwmCmbox
            // 
            this.pwmCmbox.FormattingEnabled = true;
            this.pwmCmbox.Location = new System.Drawing.Point(14, 44);
            this.pwmCmbox.Name = "pwmCmbox";
            this.pwmCmbox.Size = new System.Drawing.Size(73, 21);
            this.pwmCmbox.TabIndex = 2;
            this.pwmCmbox.Text = "PWMn";
            // 
            // pwmLabel
            // 
            this.pwmLabel.AutoSize = true;
            this.pwmLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold);
            this.pwmLabel.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.pwmLabel.Location = new System.Drawing.Point(10, 11);
            this.pwmLabel.Name = "pwmLabel";
            this.pwmLabel.Size = new System.Drawing.Size(50, 20);
            this.pwmLabel.TabIndex = 1;
            this.pwmLabel.Text = "PWM";
            // 
            // i2cPanel
            // 
            this.i2cPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.i2cPanel.Controls.Add(this.bmpRead);
            this.i2cPanel.Controls.Add(this.bmpTemp);
            this.i2cPanel.Controls.Add(this.bmpPres);
            this.i2cPanel.Controls.Add(this.bmpLabel);
            this.i2cPanel.Controls.Add(this.i2cLabel);
            this.i2cPanel.Location = new System.Drawing.Point(387, 167);
            this.i2cPanel.Name = "i2cPanel";
            this.i2cPanel.Size = new System.Drawing.Size(166, 138);
            this.i2cPanel.TabIndex = 6;
            // 
            // bmpRead
            // 
            this.bmpRead.Location = new System.Drawing.Point(39, 84);
            this.bmpRead.Name = "bmpRead";
            this.bmpRead.Size = new System.Drawing.Size(75, 32);
            this.bmpRead.TabIndex = 10;
            this.bmpRead.Text = "Read";
            this.bmpRead.UseVisualStyleBackColor = true;
            this.bmpRead.Click += new System.EventHandler(this.bmpRead_Click);
            // 
            // bmpTemp
            // 
            this.bmpTemp.AutoSize = true;
            this.bmpTemp.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.bmpTemp.Location = new System.Drawing.Point(68, 58);
            this.bmpTemp.Name = "bmpTemp";
            this.bmpTemp.Size = new System.Drawing.Size(70, 13);
            this.bmpTemp.TabIndex = 9;
            this.bmpTemp.Text = "UNDEFINED";
            // 
            // bmpPres
            // 
            this.bmpPres.AutoSize = true;
            this.bmpPres.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.bmpPres.Location = new System.Drawing.Point(68, 41);
            this.bmpPres.Name = "bmpPres";
            this.bmpPres.Size = new System.Drawing.Size(70, 13);
            this.bmpPres.TabIndex = 8;
            this.bmpPres.Text = "UNDEFINED";
            // 
            // bmpLabel
            // 
            this.bmpLabel.AutoSize = true;
            this.bmpLabel.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.bmpLabel.Location = new System.Drawing.Point(14, 41);
            this.bmpLabel.Name = "bmpLabel";
            this.bmpLabel.Size = new System.Drawing.Size(48, 13);
            this.bmpLabel.TabIndex = 7;
            this.bmpLabel.Text = "BMP180";
            // 
            // i2cLabel
            // 
            this.i2cLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold);
            this.i2cLabel.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.i2cLabel.Location = new System.Drawing.Point(13, 8);
            this.i2cLabel.Name = "i2cLabel";
            this.i2cLabel.Size = new System.Drawing.Size(60, 20);
            this.i2cLabel.TabIndex = 7;
            this.i2cLabel.Text = "I2C";
            // 
            // dacChannels
            // 
            this.dacChannels.FormattingEnabled = true;
            this.dacChannels.Location = new System.Drawing.Point(13, 40);
            this.dacChannels.Name = "dacChannels";
            this.dacChannels.Size = new System.Drawing.Size(56, 21);
            this.dacChannels.TabIndex = 7;
            this.dacChannels.Text = "DACn";
            this.dacChannels.SelectedIndexChanged += new System.EventHandler(this.dacChannels_SelectedIndexChanged);
            // 
            // EDU_LPC
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Desktop;
            this.ClientSize = new System.Drawing.Size(579, 324);
            this.Controls.Add(this.i2cPanel);
            this.Controls.Add(this.pwmPanel);
            this.Controls.Add(this.configPanel);
            this.Controls.Add(this.comPanel);
            this.Controls.Add(this.dacPanel);
            this.Controls.Add(this.adcPanel);
            this.Controls.Add(this.gpioPanel);
            this.Name = "EDU_LPC";
            this.Text = "EDU LPC";
            this.Load += new System.EventHandler(this.Form_Load);
            this.gpioPanel.ResumeLayout(false);
            this.gpioPanel.PerformLayout();
            this.adcPanel.ResumeLayout(false);
            this.adcPanel.PerformLayout();
            this.dacPanel.ResumeLayout(false);
            this.dacPanel.PerformLayout();
            this.comPanel.ResumeLayout(false);
            this.configPanel.ResumeLayout(false);
            this.configPanel.PerformLayout();
            this.pwmPanel.ResumeLayout(false);
            this.pwmPanel.PerformLayout();
            this.i2cPanel.ResumeLayout(false);
            this.i2cPanel.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel gpioPanel;
        private System.Windows.Forms.Label gpioLabel;
        private System.Windows.Forms.ComboBox gpioCmbox;
        private System.Windows.Forms.Button gpioToggle;
        private System.Windows.Forms.Button gpioSet;
        private System.Windows.Forms.Button gpioClear;
        private System.Windows.Forms.Label gpioValueLbl;
        private System.Windows.Forms.Panel adcPanel;
        private System.Windows.Forms.RadioButton lm35F;
        private System.Windows.Forms.RadioButton lm35C;
        private System.Windows.Forms.Label lm35ValueLbl;
        private System.Windows.Forms.Label lm35Lbl;
        private System.Windows.Forms.Label adcValueLbl;
        private System.Windows.Forms.ComboBox adcCmbox;
        private System.Windows.Forms.Label adcLabel;
        private System.Windows.Forms.Panel dacPanel;
        private System.Windows.Forms.RadioButton dacTriangular;
        private System.Windows.Forms.RadioButton dacSine;
        private System.Windows.Forms.CheckBox dacOn;
        private System.Windows.Forms.TextBox dacFrequency;
        private System.Windows.Forms.Label dacValueLbl;
        private System.Windows.Forms.HScrollBar dacValue;
        private System.Windows.Forms.Label dacLbl;
        private System.Windows.Forms.Panel comPanel;
        private System.Windows.Forms.Button comConnect;
        private System.Windows.Forms.ComboBox comList;
        private System.Windows.Forms.Label comLabel;
        private System.Windows.Forms.Timer comChecker;
        private System.Windows.Forms.Panel configPanel;
        private System.Windows.Forms.Button configBtn;
        private System.Windows.Forms.ComboBox configFunction;
        private System.Windows.Forms.ComboBox configPIO;
        private System.Windows.Forms.Label configLabel;
        private System.Windows.Forms.Panel pwmPanel;
        private System.Windows.Forms.Label pwmLabel;
        private System.Windows.Forms.Label pwmDutyLbl;
        private System.Windows.Forms.Label pwmFreqLbl;
        private System.Windows.Forms.TextBox pwmDuty;
        private System.Windows.Forms.TextBox pwmFreq;
        private System.Windows.Forms.ComboBox pwmCmbox;
        private System.Windows.Forms.Button pwmConfig;
        private System.Windows.Forms.Panel i2cPanel;
        private System.Windows.Forms.Button bmpRead;
        private System.Windows.Forms.Label bmpTemp;
        private System.Windows.Forms.Label bmpPres;
        private System.Windows.Forms.Label bmpLabel;
        private System.Windows.Forms.Label i2cLabel;
        private System.Windows.Forms.ComboBox dacChannels;
    }
}

