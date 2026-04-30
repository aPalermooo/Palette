using CommunityToolkit.Mvvm.ComponentModel;

namespace PaletteUI.Core
{
    public partial class FileViewModel : ObservableObject
    {
        [ObservableProperty]
        public partial string Path { get; set; }
        [ObservableProperty]
        public partial string Name {  get; set; }
        [ObservableProperty]
        public partial string[] Tags { get; set; }
    }
}
