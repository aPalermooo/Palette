using CommunityToolkit.Mvvm.ComponentModel;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace PaletteUI.Core.Repositories
{
    public partial class TagRepository : ObservableObject
    {

        // Logic for Contacting Backend -> Stores all tag items
        private readonly HttpClient client = new HttpClient();

        public async Task<string[]> GetTags(string path)
        {
            var response = await client.GetAsync($"http://localhost:18080/tagger/get-tags?path={path}");
            var json = await response.Content.ReadAsStringAsync();
            Debug.WriteLine($"Tags Response: {json}");
            var contents = JsonSerializer.Deserialize<TagResponse>(json);
            return contents.data.tags;
        }
    }
    public class TagData
    {
        public string path { get; set; }
        public string[] tags { get; set; }
    }
    public class TagResponse
    {
        public bool success { get; set; }
        public TagData data { get; set; }
    }
}
