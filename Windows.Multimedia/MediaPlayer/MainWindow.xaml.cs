using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.IO;
using Windows.Multimedia;

namespace MediaPlayer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        string m_savePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "test.wav");

        public MainWindow()
        {
            InitializeComponent();
        }


        private void tbRecord_Checked(object sender, RoutedEventArgs e)
        {
            WindowsRecord.BeginRecord(m_savePath);
        }

        private void tbRecord_Unchecked(object sender, RoutedEventArgs e)
        {
            WindowsRecord.EndRecord();
        }
    }
}
