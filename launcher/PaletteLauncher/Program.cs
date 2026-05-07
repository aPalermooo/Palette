using System;
using System.IO;
using System.Linq;
using System.Diagnostics;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;

namespace PaletteLauncher;

internal static class Program
{
    private const string PalettePackageName = "a8f14be3-5f61-455f-9bd2-a32a27e5e2d0";
    private const string BackendHealthUrl = "http://localhost:18080/";

    [STAThread]
    private static async Task Main()
    {
        try
        {
            Log("Starting launcher.");
            string repoRoot = FindRepoRoot(AppContext.BaseDirectory);
            Log($"Repo root: {repoRoot}");
            string backendExe = ResolveBackendExe(repoRoot);
            string? frontendExe = ResolveFrontendExe(repoRoot);
            Log($"Backend: {backendExe}");
            Log($"Frontend: {frontendExe ?? "package fallback"}");

            StartBackendIfNeeded(backendExe);
            await WaitForBackendAsync();
            Log("Backend is online.");

            if (frontendExe is not null)
            {
                if (TryLaunchFrontend(frontendExe))
                {
                    Log("Launched local frontend.");
                    return;
                }

                Log("Local frontend launch failed; falling back to packaged frontend.");
            }

            string packageFamilyName = GetPackageFamilyName();
            TryEnableLoopback(packageFamilyName);
            LaunchPackagedFrontend(packageFamilyName);
            Log("Launched packaged frontend.");
        }
        catch (Exception ex)
        {
            Log(ex.ToString());
            throw;
        }
    }

    private static string FindRepoRoot(string start)
    {
        DirectoryInfo? directory = new(start);

        while (directory is not null)
        {
            if (File.Exists(Path.Combine(directory.FullName, "CMakeLists.txt")) &&
                Directory.Exists(Path.Combine(directory.FullName, "frontend")) &&
                Directory.Exists(Path.Combine(directory.FullName, "backend")))
            {
                return directory.FullName;
            }

            directory = directory.Parent;
        }

        string desktopPalette = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.DesktopDirectory),
            "Palette");

        if (Directory.Exists(desktopPalette))
        {
            return desktopPalette;
        }

        throw new InvalidOperationException("Could not locate the Palette project folder.");
    }

    private static string ResolveBackendExe(string repoRoot)
    {
        string[] candidates =
        [
            Path.Combine(repoRoot, "out", "build", "x64-Release", "Release", "Palette.exe"),
            Path.Combine(repoRoot, "out", "build", "x64-Release", "Palette.exe"),
            Path.Combine(repoRoot, "out", "build", "x64-Debug", "Palette.exe")
        ];

        string? backendExe = candidates.FirstOrDefault(File.Exists);
        if (backendExe is null)
        {
            throw new FileNotFoundException("Could not find the Palette backend executable.", candidates[0]);
        }

        return backendExe;
    }

    private static string? ResolveFrontendExe(string repoRoot)
    {
        string[] candidates =
        [
            Path.Combine(repoRoot, "frontend", "PaletteUI", "bin", "x64", "Release", "net8.0-windows10.0.19041.0", "win-x64", "PaletteUI.WinUI.exe"),
            Path.Combine(repoRoot, "frontend", "PaletteUI", "bin", "x64", "Debug", "net8.0-windows10.0.19041.0", "win-x64", "PaletteUI.WinUI.exe")
        ];

        return candidates.FirstOrDefault(File.Exists);
    }

    private static void StartBackendIfNeeded(string backendExe)
    {
        if (Process.GetProcessesByName("Palette").Any())
        {
            return;
        }

        ProcessStartInfo startInfo = new()
        {
            FileName = backendExe,
            WorkingDirectory = Path.GetDirectoryName(backendExe) ?? Environment.CurrentDirectory,
            UseShellExecute = false,
            CreateNoWindow = true,
            WindowStyle = ProcessWindowStyle.Hidden
        };

        Process.Start(startInfo);
    }

    private static async Task WaitForBackendAsync()
    {
        using HttpClient client = new()
        {
            Timeout = TimeSpan.FromSeconds(1)
        };

        Exception? lastError = null;

        for (int attempt = 0; attempt < 40; attempt++)
        {
            try
            {
                using HttpResponseMessage response = await client.GetAsync(BackendHealthUrl);
                if (response.IsSuccessStatusCode)
                {
                    return;
                }
            }
            catch (Exception ex)
            {
                lastError = ex;
            }

            await Task.Delay(250);
        }

        throw new TimeoutException("Palette backend did not become ready on localhost:18080.", lastError);
    }

    private static string GetPackageFamilyName()
    {
        using Process process = RunProcess(
            "powershell.exe",
            $"-NoProfile -ExecutionPolicy Bypass -Command \"(Get-AppxPackage -Name '{PalettePackageName}').PackageFamilyName\"");

        string output = process.StandardOutput.ReadToEnd().Trim();
        string error = process.StandardError.ReadToEnd().Trim();
        process.WaitForExit();

        if (process.ExitCode != 0 || string.IsNullOrWhiteSpace(output))
        {
            throw new InvalidOperationException(
                "Could not find the installed Palette frontend package. Run Install-Palette.bat first." +
                (string.IsNullOrWhiteSpace(error) ? string.Empty : Environment.NewLine + error));
        }

        return output;
    }

    private static void TryEnableLoopback(string packageFamilyName)
    {
        using Process process = RunProcess(
            "CheckNetIsolation.exe",
            $"LoopbackExempt -a -n={packageFamilyName}");

        process.WaitForExit();
    }

    private static bool TryLaunchFrontend(string frontendExe)
    {
        try
        {
            ProcessStartInfo startInfo = new()
            {
                FileName = frontendExe,
                WorkingDirectory = Path.GetDirectoryName(frontendExe) ?? Environment.CurrentDirectory,
                UseShellExecute = false,
                CreateNoWindow = false
            };

            Process process = Process.Start(startInfo) ??
                              throw new InvalidOperationException("Failed to start local Palette frontend.");

            Thread.Sleep(1500);
            if (process.HasExited)
            {
                Log($"Local frontend exited immediately with code {process.ExitCode}.");
                return false;
            }

            return true;
        }
        catch (Exception ex)
        {
            Log(ex.ToString());
            return false;
        }
    }

    private static void LaunchPackagedFrontend(string packageFamilyName)
    {
        ProcessStartInfo startInfo = new()
        {
            FileName = $"shell:AppsFolder\\{packageFamilyName}!App",
            UseShellExecute = true
        };

        Process.Start(startInfo);
    }

    private static Process RunProcess(string fileName, string arguments)
    {
        ProcessStartInfo startInfo = new()
        {
            FileName = fileName,
            Arguments = arguments,
            UseShellExecute = false,
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            CreateNoWindow = true,
            WindowStyle = ProcessWindowStyle.Hidden
        };

        return Process.Start(startInfo) ??
               throw new InvalidOperationException($"Failed to start {fileName}.");
    }

    private static void Log(string message)
    {
        string directory = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
            "Palette");
        Directory.CreateDirectory(directory);

        File.AppendAllText(
            Path.Combine(directory, "launcher.log"),
            $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] {message}{Environment.NewLine}");
    }
}
