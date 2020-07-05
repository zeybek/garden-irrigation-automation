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
using System.Windows.Forms.VisualStyles;

namespace ComputerControl
{
    public partial class Form1 : Form
    {
        SerialPort ArduinoPort = new SerialPort();
        private bool Role1_Status, Role2_Status = false;

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            ArduinoPort.PortName = "COM4";
            ArduinoPort.Open();
            Role1_Control_Button.ForeColor = Color.White;
            Role1_Control_Button.BackColor = Color.Red;
            button8.ForeColor = Color.White;
            button8.BackColor = Color.Red;
            ArduinoStatus.AppendText("Merhaba");
        }

        private void button7_Click(object sender, EventArgs e)
        {
            ArduinoPort.Write("1");
            Role1_Status = !Role1_Status;
            if (Role1_Status)
            {
                Role1_Control_Button.ForeColor = Color.White;
                Role1_Control_Button.BackColor = Color.Green;
            }
            else
            {
                Role1_Control_Button.ForeColor = Color.White;
                Role1_Control_Button.BackColor = Color.Red;
            }
        }

        private void ArduinoTimer_Tick(object sender, EventArgs e)
        {
            String[] ArduinoValues = ArduinoPort.ReadExisting().Split(' ');
            Temperature.Text = ArduinoValues[0] + "°C";
            Humidity.Text = "%" + ArduinoValues[1];
            //ArduinoStatus.AppendText("\nMerhaba");
        }

        private void button8_Click(object sender, EventArgs e)
        {
            ArduinoPort.Write("2");
            Role2_Status = !Role2_Status;
            if (Role2_Status)
            {
                button8.ForeColor = Color.White;
                button8.BackColor = Color.Green;
            }
            else
            {
                button8.ForeColor = Color.White;
                button8.BackColor = Color.Red;
            }
        }
    }
}
