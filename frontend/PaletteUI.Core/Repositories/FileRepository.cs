using CommunityToolkit.Mvvm.ComponentModel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Http;
using System.Text.Json;
using PaletteUI.Core;

namespace PaletteUI.Core.Repositories
{
    public partial class FileRepository : ObservableObject
    {
        //Logic for connecting backend and getting all files (within a directory)
        private readonly HttpClient client = new HttpClient();

        public async Task<(List<FileViewModel> Files,List<FileViewModel> Directories)> GetFiles(string path)
        {
            var response = await client.GetAsync($"http://localhost:18080/explorer/get-directory-contents?path={path}");
            var json = await response.Content.ReadAsStringAsync();
            var contents = JsonSerializer.Deserialize<DirectoryContents>(json);
            var files = new List<FileViewModel>();
            var directories = new List<FileViewModel>();
            foreach(var item in contents.files)
            {
                files.Add(new FileViewModel { Name = item.Name, Path = item.Path });
            
            }
            foreach(var item in contents.directories)
            {
                directories.Add(new FileViewModel { Name = item.Name, Path = item.Path });
            }
            return (files, directories);
        }

    }
    public class FileItem
    {
        public string Name { get; set; }
        public string Path { get; set; }
    }
    public class DirectoryContents
    {
        public List<FileItem> files { get; set; }
        public List<FileItem> directories { get; set; }
    }
}
