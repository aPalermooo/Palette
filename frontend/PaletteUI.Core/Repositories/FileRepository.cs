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
        public readonly HttpClient client = new HttpClient();
        private readonly TagRepository tagRepository = new TagRepository();

        public async Task<(List<FileViewModel> Files,List<FileViewModel> Directories)> GetFiles(string path)
        {
            var encodedPath = Uri.EscapeDataString(path);
            var response = await client.GetAsync($"http://localhost:18080/explorer/get-directory-contents?path={encodedPath}");
            response.EnsureSuccessStatusCode();
            var json = await response.Content.ReadAsStringAsync();
            var contents = JsonSerializer.Deserialize<DirectoryContents>(json);
            var files = new List<FileViewModel>();
            var directories = new List<FileViewModel>();
            foreach(var item in contents?.files ?? [])
            {
                var tags = await tagRepository.GetTags(item.path);
                files.Add(new FileViewModel { Name = item.name, Path = item.path, Type = System.IO.Path.GetExtension(item.name), Tags = tags });
            
            }
            foreach(var item in contents?.directories ?? [])
            {
                directories.Add(new FileViewModel { Name = item.name, Path = item.path, Type = "Folder", Tags = [] });
            }
            return (files, directories);
        }

    }
    public class FileItem
    {
        public string name { get; set; }
        public string path { get; set; }
    }
    public class DirectoryContents
    {
        public List<FileItem> files { get; set; }
        public List<FileItem> directories { get; set; }
    }
}
