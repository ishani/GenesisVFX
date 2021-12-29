/*
    GenesisVFX for Paint.NET, part of the regenesis project
    Harry Denholm, ishani.org 2018-2021
*/

using PaintDotNet;
using System;
using System.Reflection;

namespace GenesisVFX
{
    public class PluginSupportInfo : IPluginSupportInfo
    {
        public string Author      => (   (AssemblyCopyrightAttribute)base.GetType().Assembly.GetCustomAttributes( typeof(AssemblyCopyrightAttribute),   false )[0] ).Copyright;
        public string Copyright   => ( (AssemblyDescriptionAttribute)base.GetType().Assembly.GetCustomAttributes( typeof(AssemblyDescriptionAttribute), false )[0] ).Description;
        public string DisplayName => (     (AssemblyProductAttribute)base.GetType().Assembly.GetCustomAttributes( typeof(AssemblyProductAttribute),     false )[0] ).Product;

        public Version Version    => base.GetType().Assembly.GetName().Version;
        public Uri WebsiteUri     => new Uri("https://www.ishani.org/");
    }

    // rendering tasks across sessions are encoded with magic identifiers to signify whether the chosen GFX file is on-disk or in a ZIP file
    internal static class GfxConstants
    {
        public static char      MagicSepInZip   = '?';
        public static string    MagicSepInFile  = "|||";
    }
}
