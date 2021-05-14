## Table Styling in Markdown

<style>
.pinMap {
    width: 70%;
    text-align: center;
}
.pinMap th {
    background: grey;
    word-wrap: break-word;
    text-align: center;
}

/* Board reserved pins */
.pinMap tr:nth-child(3) { background: lightgray; }
.pinMap tr:nth-child(4) { background: lightgray; }
.pinMap tr:nth-child(5) { background: lightgray; }
.pinMap tr:nth-child(6) { background: lightgray; }
.pinMap tr:nth-child(8) { background: lightgray; }
.pinMap tr:nth-child(33) { background: lightgray; }
.pinMap tr:nth-child(34) { background: lightgray; }
.pinMap tr:nth-child(35) { background: lightgray; }

/* GPIO - Output pins */
.pinMap tr:nth-child(9) { background: yellow; }
.pinMap tr:nth-child(10) { background: yellow; }
.pinMap tr:nth-child(13) { background: yellow; }
.pinMap tr:nth-child(16) { background: yellow; }
.pinMap tr:nth-child(17) { background: yellow; }
.pinMap tr:nth-child(27) { background: yellow; }
.pinMap tr:nth-child(28) { background: yellow; }
.pinMap tr:nth-child(29) { background: yellow; }

/* GPIO - Input pins */
.pinMap tr:nth-child(19) { background: aquamarine; }
.pinMap tr:nth-child(20) { background: aquamarine; }
.pinMap tr:nth-child(21) { background: aquamarine; }
.pinMap tr:nth-child(22) { background: aquamarine; }
.pinMap tr:nth-child(23) { background: aquamarine; }
.pinMap tr:nth-child(24) { background: aquamarine; }

/* ACMP */
.pinMap tr:nth-child(1) { background: purple; }
.pinMap tr:nth-child(2) { background: purple; }

/* ADC */
.pinMap tr:nth-child(7) { background: orange; }
.pinMap tr:nth-child(14) { background: orange; }
.pinMap tr:nth-child(15) { background: orange; }

/* I2C Pins */
.pinMap tr:nth-child(11) { background: blue; }
.pinMap tr:nth-child(12) { background: blue; }

/* USART Pins */
.pinMap tr:nth-child(25) { background: green; }
.pinMap tr:nth-child(26) { background: green; }
.pinMap tr:nth-child(31) { background: green; }
.pinMap tr:nth-child(32) { background: green; }

/* DAC Outputs */
.pinMap tr:nth-child(18) { background: red; }
.pinMap tr:nth-child(30) { background: red; }


</style>

<div class="pinMap">

| Pin name | Board pin number | EDU board function | Board function |
| -------- | ---------------- | ------------------ | -------------- |
| PIO0_00 | 39 | ACMP_I1 | |
| PIO0_01 | 38 | ACMP_I2 | |
| PIO0_02 | 37 | - | SWDIO |
| PIO0_03 | 36 | - | SWCLK |
| PIO0_04 | 35 | - | USER |
| PIO0_05 | 9/34 | - | RESET |
| PIO0_06 | 33 | ADC1 | |
| PIO0_07 | 32 | - | ADC0/POT |
| PIO0_08 | 19/21 | GPIO |  |
| PIO0_09 | 18/22 | GPIO |  |
| PIO0_10 | 23 | SCL |  |
| PIO0_11 | 24 | SDA |  |
| PIO0_12 | 25 | GPIO | ISP |
| PIO0_13 | 26 | ADC_10 |  |
| PIO0_14 | 27 | ADC_2 |  |
| PIO0_15 | 28 | GPIO |  |
| PIO0_16 | 1 | GPIO |  |
| PIO0_17 | 2 | DACOUT_0 |  |
| PIO0_18 | 3 | GPIO |  |
| PIO0_19 | 4 | GPIO |  |
| PIO0_20 | 5 | GPIO |  |
| PIO0_21 | 6 | GPIO |  |
| PIO0_22 | 7 | GPIO |  |
| PIO0_23 | 8 | GPIO |  |
| PIO0_24 | 10 | RXD0 | RXD0 |
| PIO0_25 | 11 | TXD0 | TXD0 |
| PIO0_26 | 12 | GPIO |  |
| PIO0_27 | 13 | GPIO |  |
| PIO0_28 | 14 | GPIO |  |
| PIO0_29 | 15 | DCOUT_1 |  |
| PIO0_30 | 16 | RXD1 |  |
| PIO0_31 | 17 | TXD1 |  |
| PIO1_00 | 31 |  | GREEN LED |
| PIO1_01 | 30 |  | BLUE LED |
| PIO1_02 | 29 |  | RED LED |

</div>
