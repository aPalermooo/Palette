using CommunityToolkit.Mvvm.ComponentModel;
using PaletteUI.Core.Repositories;
using System.Collections.ObjectModel;

namespace PaletteUI.Core
{

    /*Represents Directory Path Reference that UI is searching under*/
    public partial class TagViewModel : ObservableObject
    {
        string HOME_DIRECTORY = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
        string PALETTE_DIRECTORY;
        private FileRepository fileRepository = new FileRepository();

        [ObservableProperty]
        public partial ObservableCollection<string> Path { get; set; }

        public ObservableCollection<FileViewModel> Files { get; set; } = [];

        public ObservableCollection<FileViewModel> Directories { get; set; } = [];

        /// <summary>
        /// Set Directory Reference back to Palette's Main directory
        /// </summary>
        private void SetToHome()
        {
            Path.Clear();
            foreach (var segment in PALETTE_DIRECTORY.Split(System.IO.Path.DirectorySeparatorChar))
            {
                if (!string.IsNullOrEmpty(segment))
                    Path.Add(segment);
            }
        }

        /// <summary>
        /// If Directory Reference is not contained in Palette's Main directory => Reset Directory Reference
        /// </summary>
        private void ValidatePath()
        {
            var paletteSegments = PALETTE_DIRECTORY.Split(System.IO.Path.DirectorySeparatorChar)
                .Where(s => !string.IsNullOrEmpty(s)).ToList();

            // Reset if current path doesn't start with the palette directory segments
            for (int i = 0; i < paletteSegments.Count; i++)
            {
                if (i >= Path.Count || Path[i] != paletteSegments[i])
                {
                    SetToHome();
                    return;
                }
            }
        }
        private async Task refreshFilesAndDirectories()
        {
            var fullPath = System.IO.Path.Combine(Path.ToArray());
            var result = await fileRepository.GetFiles(fullPath);
            Files.Clear();
            Directories.Clear();
            foreach (var file in result.Files)
            {
                Files.Add(file);
            }
            foreach(var directory in result.Directories)
            {
                Directories.Add(directory);
            }
        }

        public TagViewModel()
        {
            PALETTE_DIRECTORY = System.IO.Path.Combine(HOME_DIRECTORY, "Palette");
            Path = [];
            //These didn't want to be Observable Properties. IDK why
            //Files = [];
            //Directories = [];
            SetToHome();
        }

        public string PaletteDirectory => PALETTE_DIRECTORY;

        /// <summary>
        /// set Directory Reference to specific location
        /// </summary>
        /// <param name="fullPath"></param>
        public async void NavigateTo(string fullPath)
        {
            Path.Clear();
            foreach (var segment in fullPath.Split(System.IO.Path.DirectorySeparatorChar))
            {
                if (!string.IsNullOrEmpty(segment))
                    Path.Add(segment);
            }
            ValidatePath();
            await refreshFilesAndDirectories();
        }

        /// <summary>
        /// set Directory reference backwards one step
        /// </summary>
        /// <param name="fullPath"></param>
        public async void NavigateBack()
        {
            if (Path.Count > 0)
                Path.RemoveAt(Path.Count -1);
            ValidatePath();
            await refreshFilesAndDirectories();
        }

        /// <summary>
        /// Reset Directory reference back to inital reference
        /// </summary>
        public async void Reset()
        {
            SetToHome();
           await refreshFilesAndDirectories();
        }
    }
}
