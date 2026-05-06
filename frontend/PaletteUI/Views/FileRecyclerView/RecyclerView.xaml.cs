using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using PaletteUI.Core;
using PaletteUI.Views.RootContent;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text.Json;
using Windows.Foundation;
using Windows.Foundation.Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace PaletteUI.Views.FileRecyclerView
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class RecyclerView : Page
    {
        public TagViewModel DirectoryViewModel;
        private readonly HttpClient client = new HttpClient();
        public RecyclerView()
        {
            InitializeComponent();
        }

      
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            if (e.Parameter is TagViewModel tvm)
            {
                DirectoryViewModel = tvm;
                Debug.WriteLine($"Files count: {tvm.Files.Count}");
                Debug.WriteLine($"Directories count: {tvm.Directories.Count}");
                Bindings.Update();

            }
            Debug.WriteLine($"Recycler View Opened: {e.Parameter}");

        }
        private async void File_Click(object sender, RoutedEventArgs e)
        {
            var button = sender as Button;
            var file = button.Tag as FileViewModel;
            Debug.WriteLine($"File Clicked: {file?.Path}");
            if (file == null) return;
            await client.GetAsync($"http://localhost:18080/explorer/open-default?path={Uri.EscapeDataString(file.Path)}");
        }

        private void Directory_Click(object sender, RoutedEventArgs e)
        {
            var button = sender as Button;
            var directory = button?.Tag as FileViewModel;
            if (directory == null) return;
            DirectoryViewModel.NavigateTo(directory.Path);
        }
        private async void AddTag_Click(object sender, RoutedEventArgs e)
        {
            var menuItem = sender as MenuFlyoutItem;
            var file = menuItem.Tag as FileViewModel;
            var dialog = new ContentDialog
            {
                Title = "Add Tag",
                PrimaryButtonText = "Add",
                CloseButtonText = "Cancel",
                XamlRoot = this.XamlRoot
            };
            var input = new TextBox { PlaceholderText = "Enter Tag", Width = 300 };
            var panel = new StackPanel { Padding = new Thickness(0, 10, 0, 0) };
            panel.Children.Add(input);
            dialog.Content = panel;
            var result = await dialog.ShowAsync();
            if (result == ContentDialogResult.Primary && !string.IsNullOrEmpty(input.Text))
            {
                var body = JsonSerializer.Serialize(new { path = file.Path, tag = input.Text });
                var content = new StringContent(body, System.Text.Encoding.UTF8, "application/json");
                await client.PostAsync("http://localhost:18080/tagger/add-tag", content);
            }
        }
    }
}
