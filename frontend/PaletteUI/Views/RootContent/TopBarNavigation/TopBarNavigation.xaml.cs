using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using PaletteUI.Core;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text.Json;
using System.Diagnostics;
using Windows.ApplicationModel.VoiceCommands;
using Windows.Foundation;
using Windows.Foundation.Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace PaletteUI.Views.RootContent
{
    public sealed partial class TopBar : UserControl
    {
        public static readonly DependencyProperty DirectoryViewModelProperty = DependencyProperty.Register(nameof(DirectoryViewModel), typeof(TagViewModel), typeof(TopBar), new PropertyMetadata(null, OnDirectoryViewModelChanged));
        private readonly HttpClient client = new HttpClient();

        private async void SearchBar_QuerySubmitted(AutoSuggestBox sender, AutoSuggestBoxQuerySubmittedEventArgs args)
        {
            if (string.IsNullOrEmpty(args.QueryText)) return;
            var response = await client.GetAsync($"http://localhost:18080/search/filename?pattern=%{args.QueryText}%");
            var json = await response.Content.ReadAsStringAsync();
            var contents = JsonSerializer.Deserialize<SearchResponse>(json);

            DirectoryViewModel.Files.Clear();
            foreach (var filePath in contents.data.files)
            {
                DirectoryViewModel.Files.Add(new FileViewModel
                {
                    Name = System.IO.Path.GetFileName(filePath),
                    Path = filePath,
                    Type = System.IO.Path.GetExtension(filePath)
                });
            }
        }
        public TagViewModel DirectoryViewModel
        {
            get => (TagViewModel)GetValue(DirectoryViewModelProperty);
            set => SetValue(DirectoryViewModelProperty, value);
        }

        private static void OnDirectoryViewModelChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var Control = (TopBar)d;
            if (e.NewValue is TagViewModel vm)
            {
                Control.CurrentPath.ItemsSource = vm.Path;
            }
        }
        private async void ImportButton_Click(object sender, RoutedEventArgs e)
        {
            var picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.FileTypeFilter.Add("*");
            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(App.MainWindow!);
            WinRT.Interop.InitializeWithWindow.Initialize(picker, hwnd);
            var file = await picker.PickSingleFileAsync();
            if (file == null) return;
            Debug.WriteLine($"Picked File: {file.Path}");
            var destination = System.IO.Path.Combine(System.IO.Path.Combine(DirectoryViewModel.Path.ToArray()), file.Name);
            Debug.WriteLine($"Destination: {destination}");
            System.IO.File.Copy(file.Path, destination, true);
            Debug.WriteLine("File copied");
            var body = JsonSerializer.Serialize(new { path = destination });
            var content = new StringContent(body, System.Text.Encoding.UTF8, "application/json");
            await client.PostAsync("http://localhost:18080/tagger/apply-rules", content);
            Debug.WriteLine("Tagger rules applied");
            await DirectoryViewModel.refreshFilesAndDirectories();
            Debug.WriteLine("Directory refreshed");
        }

        public TopBar()
        {
            InitializeComponent();
        }
        public class SearchData
        {
            public string[] files { get; set; }
        }
        public class SearchResponse
        {
            public bool success { get; set; }
            public SearchData data { get; set; }
        }

    }
}
