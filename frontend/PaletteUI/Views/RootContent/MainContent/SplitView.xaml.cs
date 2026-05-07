using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using PaletteUI.Core;
using PaletteUI.Views.FileRecyclerView;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace PaletteUI.Views.RootContent.MainContent
{
    public sealed partial class SplitView : UserControl
    {

        public static readonly DependencyProperty DirectoryViewModelProperty = DependencyProperty.Register(nameof(DirectoryViewModel), typeof(TagViewModel), typeof(SplitView), new PropertyMetadata(null, OnDirectoryViewModelChanged));
        public TagViewModel DirectoryViewModel
        {
            get => (TagViewModel)GetValue(DirectoryViewModelProperty);
            set => SetValue(DirectoryViewModelProperty, value);
        }

        private static void OnDirectoryViewModelChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var Control = (SplitView)d;
            if (e.NewValue is TagViewModel vm)
            {
                Control.RecyclerFrame.Navigate(typeof(RecyclerView), vm);
            }
        }

        public SplitView()
        {
            InitializeComponent();

            RecyclerFrame.Navigate(typeof(RecyclerView), DirectoryViewModel);
        }
    }
}
