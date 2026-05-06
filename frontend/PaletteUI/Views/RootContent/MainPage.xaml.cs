using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using PaletteUI.Core;
using System.Diagnostics;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace PaletteUI;

/// <summary>
/// An empty page that can be used on its own or navigated to within a Frame.
/// </summary>
public sealed partial class MainPage : Page
{
    public TagViewModel DirectoryViewModel { get; } = new TagViewModel();

    public MainPage()
    {
        InitializeComponent();
        Loaded += MainPage_Loaded;
    }

    private async void MainPage_Loaded(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
    {
        await DirectoryViewModel.refreshFilesAndDirectories();
    }

    protected override void OnNavigatedTo(NavigationEventArgs e)
    {
        base.OnNavigatedTo(e);

        Debug.WriteLine($"Navigated to screen w/ : {e.Parameter}");
    }
}
