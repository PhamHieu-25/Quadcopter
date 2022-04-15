using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Csharp_Arduino_Control_One_LED
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            button_open.Enabled = true;
            button_close.Enabled = false;
            button_turnON.Enabled = false;
            button_turnOFF.Enabled = false;
            progressBar1.Value = 0;
            animatedLED1.BackColor = Color.DarkRed;
            pictureBox1.Image = Properties.Resources.DarkRed;
            comboBox_baudRate.Text = "9600";
            string[] portLists = SerialPort.GetPortNames();
            comboBox_comPort.Items.AddRange(portLists);
        }

        private void button_open_Click(object sender, EventArgs e)
        {
            try
            {
                serialPort1.PortName = comboBox_comPort.Text;
                serialPort1.BaudRate = Convert.ToInt32(comboBox_baudRate.Text);
                serialPort1.Open();

                button_open.Enabled = false;
                button_close.Enabled = true;
                button_turnON.Enabled = true;
                button_turnOFF.Enabled = true;
                progressBar1.Value = 100;
            }
            catch(Exception error)
            {
                MessageBox.Show(error.Message);
            }
        }

        private void button_close_Click(object sender, EventArgs e)
        {
            if(serialPort1.IsOpen)
            {
                try
                {
                    serialPort1.Close();

                    button_open.Enabled = true;
                    button_close.Enabled = false;
                    button_turnON.Enabled = false;
                    button_turnOFF.Enabled = false;
                    progressBar1.Value = 0;
                }
                catch(Exception error)
                {
                    MessageBox.Show(error.Message);
                }
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if(serialPort1.IsOpen)
            {
                try
                {
                    serialPort1.Close();
                }
                catch(Exception error)
                {
                    MessageBox.Show(error.Message);
                }
            }
        }

        private void button_turnON_Click(object sender, EventArgs e)
        {
            if(serialPort1.IsOpen)
            {
                try
                {
                    serialPort1.Write("ON#");

                    animatedLED1.BackColor = Color.Red;
                    pictureBox1.Image = Properties.Resources.Red;
                }
                catch(Exception error)
                {
                    MessageBox.Show(error.Message);
                }
            }
        }

        private void button_turnOFF_Click(object sender, EventArgs e)
        {
            if(serialPort1.IsOpen)
            {
                try
                {
                    serialPort1.Write("OFF#");

                    animatedLED1.BackColor = Color.DarkRed;
                    pictureBox1.Image = Properties.Resources.DarkRed;
                }
                catch(Exception error)
                {
                    MessageBox.Show(error.Message);
                }
            }
        }

        private void animatedLED1_Click(object sender, EventArgs e)
        {

        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void progressBar1_Click(object sender, EventArgs e)
        {

        }
    }
}
