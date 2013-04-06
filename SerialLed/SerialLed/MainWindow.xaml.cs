using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO.Ports;
using System.Threading;
using System.Windows.Threading;

namespace SerialLed
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        SerialPort port;
        DispatcherTimer theTimer;
        bool onState;
        int onTime, offTime;

        public MainWindow()
        {
            InitializeComponent();
            port = new SerialPort("COM7");
            port.Open();
            onState = false;
        }

        private void button2_Click(object sender, RoutedEventArgs e)
        {
            if (this.theTimer == null)
            {
                this.theTimer = new DispatcherTimer();
                this.theTimer.Tick += new EventHandler(timer_Tick);
            }
            if (this.theTimer.IsEnabled == true)
            {
                this.theTimer.Stop();
            }

            // Configure interval
            int tryOnTime=0;
            if (true == int.TryParse(this.textBoxOn.Text, out tryOnTime))
            {
                this.onTime = tryOnTime;
            }
            int tryOffTime=0;
            if (int.TryParse(this.textBoxOff.Text, out tryOffTime) == true)
            {
                this.offTime= tryOffTime;
            }
            this.theTimer.Interval = TimeSpan.FromMilliseconds(1);
            this.theTimer.Start();
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            if (this.onState == false)
            {
                button2.Background = Brushes.Blue;
                this.onState = true;
                this.theTimer.Interval = TimeSpan.FromMilliseconds(this.onTime);
            }
            else
            {
                button2.Background = Brushes.Gray;
                this.onState = false;
                this.theTimer.Interval = TimeSpan.FromMilliseconds(this.offTime);
            }
            port.DtrEnable = this.onState;
        }
    }
}
