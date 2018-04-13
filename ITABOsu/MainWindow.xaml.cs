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
using System.Windows.Shapes;
using Bridge;
using System.Windows.Threading;

using System.Windows.Forms;
using System.ComponentModel;
using KeyboardHelper;
using System.IO;

namespace ITABOsu
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly BackgroundWorker worker = new BackgroundWorker();
        private KeyboardListener kListener = new KeyboardListener();
        private TABOsuBridge mainTABOsuCore = new TABOsuBridge();

        private bool botActivated;
        private bool osuIsRunning;
        private bool bindActivateEdit;
        private Key bindedActivationButton;

        public MainWindow()
        {
            InitializeComponent();

            bindedActivationButton = System.Windows.Input.Key.Q;

            worker.DoWork += worker_DoWork;
            worker.RunWorkerCompleted += worker_RunWorkerCompleted;
            worker.RunWorkerAsync();

            kListener.KeyDown += new RawKeyEventHandler(KListener_KeyDown);

            mainTABOsuCore.SetOsuPathWrapper("A:\\Games\\osu!");
            ListDirectory(SongViewer, "A:\\Games\\osu!\\Songs");
        }

        void CheckForOsuRunning()
        {
            bool result = mainTABOsuCore.SetupOsuInfoWrapper();
            osuIsRunning = result;
            this.Dispatcher.Invoke(() =>
            {
                IsOsuRunningLabel.Content = "Osu is Running: " + (result ? "true" : "false");
                IsOsuRunningLabel.Foreground = result ? Brushes.Green : Brushes.Red;
            });
        }

        void KListener_KeyDown(object sender, RawKeyEventArgs args)
        {
            // Setting keybindings
            if (bindActivateEdit)
            {
                BindActive.Content = args.Key.ToString();

                bindedActivationButton = args.Key;
                bindActivateEdit = false;
            }
            else if (args.Key == bindedActivationButton)
            {
                botActivated = !botActivated;
                IsBotActiveLabel.Content = "Bot Active: " + (botActivated ? "true" : "false");
                IsBotActiveLabel.Foreground = botActivated ? Brushes.Green : Brushes.Red;

                BindActive.IsEnabled = !botActivated;

                if (botActivated)
                {
                    worker.RunWorkerAsync();
                }
                else
                {
                    mainTABOsuCore.StopBotWrapper();
                    worker.CancelAsync();
                }
            }
        }

        private void worker_DoWork(object sender, DoWorkEventArgs e)
        {
            if (osuIsRunning)
            {
                // Running bot
                mainTABOsuCore.StartBotWrapper();
            }
            else
            {
                // Checking for osu! process
                while (!osuIsRunning)
                    CheckForOsuRunning();
            }
        }

        private void worker_RunWorkerCompleted(object sender, 
                                               RunWorkerCompletedEventArgs e)
        { }

        private void BindActive_Click(object sender, RoutedEventArgs e)
        {
            bindActivateEdit = true;
            BindActive.Content = "Press any button";
        }

        private void TABOsuWindow_Closed(object sender, EventArgs e)
        {
            worker.Dispose();
        }

        private void ListDirectory(System.Windows.Controls.TreeView treeView, string path)
        {
            treeView.Items.Clear();
            var rootDirectoryInfo = new DirectoryInfo(path);
            treeView.Items.Add(CreateDirectoryNode(rootDirectoryInfo));
        }

        // Searches all songs in folder
        private static TreeViewItem CreateDirectoryNode(DirectoryInfo directoryInfo)
        {
            var directoryNode = new TreeViewItem { Header = directoryInfo.Name };

            foreach (var directory in directoryInfo.GetDirectories())
                directoryNode.Items.Add(CreateDirectoryNode(directory));

            foreach (var file in directoryInfo.GetFiles())
                if (file.Extension == ".osu")
                    directoryNode.Items.Add(new TreeViewItem { Header = file.Name, Tag = file.FullName });

            return directoryNode;

        }

        // Setting osu root path folder
        private void SetPathToOsuButton_Click(object sender, RoutedEventArgs e)
        {
            FolderBrowserDialog folderBrowser = new FolderBrowserDialog();
            folderBrowser.ShowDialog();

            // Validate check for osu songs folder
            if (System.IO.Directory.Exists(folderBrowser.SelectedPath + "\\Songs"))
            {
                mainTABOsuCore.SetOsuPathWrapper(folderBrowser.SelectedPath);
                pathLabel.Text = folderBrowser.SelectedPath;
                ListDirectory(SongViewer, folderBrowser.SelectedPath + "\\Songs");
            }
        }

        // Selecting Song
        private void SongViewer_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            if (((TreeViewItem)SongViewer.SelectedValue).Tag != null)
            {
                bool result = mainTABOsuCore.LoadSongWrapper(((TreeViewItem)SongViewer.SelectedValue).Tag.ToString());
                IsMapLoadedLabel.Content = "Map Loaded: " + (result ? "true" : "false");
                IsMapLoadedLabel.Foreground = result ? Brushes.Green : Brushes.Red;
            }
        }

        private void BotRelaxButton_Checked(object sender, RoutedEventArgs e)
        {
            mainTABOsuCore.SetAutoHitWrapper((bool)BotRelaxButton.IsChecked);
        }

        private void BotAimButton_Checked(object sender, RoutedEventArgs e)
        {
            mainTABOsuCore.SetAutoAimWrapper((bool)BotAimButton.IsChecked);
        }

        private void BotSpinAutomatic_Checked(object sender, RoutedEventArgs e)
        {
            mainTABOsuCore.SetAutoSpinWrapper((bool)BotAimButton.IsChecked);
        }
    }
}
