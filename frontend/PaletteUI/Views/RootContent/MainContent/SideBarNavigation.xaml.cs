using CommunityToolkit.Mvvm.Input;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using PaletteUI.Core;
using PaletteUI.Core.Repositories;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using System.Diagnostics;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace PaletteUI.Views.RootContent.MainContent
{
    public sealed partial class SideBarNavigation : UserControl
    {

        public static readonly DependencyProperty DirectoryViewModelProperty = DependencyProperty.Register(nameof(DirectoryViewModel), typeof(TagViewModel), typeof(TopBar), null);
        public TagViewModel DirectoryViewModel
        {
            get => (TagViewModel)GetValue(DirectoryViewModelProperty);
            set => SetValue(DirectoryViewModelProperty, value);
        }

        public FileRepository fileRepository = new FileRepository();
        public ObservableCollection<FileViewModel> Directories { get; set; } = [];
        public SideBarNavigation()
        {
            InitializeComponent();
            loadSidebar();
        }
        private async void loadSidebar()
        {
            try
            {
                string HOME_DIRECTORY = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
                string PALETTE_DIRECTORY = System.IO.Path.Combine(HOME_DIRECTORY, "Palette");
                var result = await fileRepository.GetFiles(PALETTE_DIRECTORY);
                foreach (var directory in result.Directories)
                {
                    Directories.Add(directory);
                }
            } 
            catch(Exception ex) 
            {
                Debug.WriteLine($"Error loading sidebar: {ex.Message}");
            }
        }

        [RelayCommand]
        private void NavigateDirectory(string path)
        {
            DirectoryViewModel.NavigateTo(
                System.IO.Path.Combine(path));
        }
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            var button = sender as Button;
            NavigateDirectory((string)button.CommandParameter);
        }
    }
}
