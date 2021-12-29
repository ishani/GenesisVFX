/*
    GenesisVFX for Paint.NET, part of the regenesis project
    Harry Denholm, ishani.org 2018-2021
*/

using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;

using ICSharpCode.SharpZipLib.Zip;

using PaintDotNet;
using PaintDotNet.Effects;

namespace GenesisVFX
{
    [PluginSupportInfo( typeof( PluginSupportInfo ), DisplayName = "GenesisVFX" )]
    public class GenesisEffect : Effect<GenesisEffectConfigToken>
    {
        internal static string StaticName => "Genesis VFX";
        internal static string StaticCategory => SubmenuNames.Render;
        internal static Bitmap StaticImage => GetIcon();

        internal bool        repeatEffect = false;
        internal bool  triggerFinalRender = false;


        private static readonly Tuple<int, Bitmap>[] availableIcons = new Tuple<int, Bitmap>[]
        {
            new Tuple<int, Bitmap>(96,  Properties.Resources.Plugin_16),
            new Tuple<int, Bitmap>(144, Properties.Resources.Plugin_24),
            new Tuple<int, Bitmap>(192, Properties.Resources.Plugin_32),
            new Tuple<int, Bitmap>(384, Properties.Resources.Plugin_64)
        };

        internal static Bitmap GetIcon()
        {
            var currentDpi    = UIScaleFactor.Current.Dpi;
            var currentBitmap = availableIcons.Last().Item2;

            for ( int i = 0; i < availableIcons.Length; i++ )
            {
                var icon = availableIcons[i];
                if ( icon.Item1 >= currentDpi )
                {
                    currentBitmap = icon.Item2;
                    break;
                }
            }

            return currentBitmap;
        }

        public GenesisEffect() : base(
            StaticName,
            StaticImage,
            StaticCategory,
            new EffectOptions() {
                Flags = EffectFlags.Configurable,
                RenderingSchedule = EffectRenderingSchedule.None
            } )
        {
            repeatEffect = true;
            triggerFinalRender = false;
        }

        // enumerator for walking a zip file by file record
        public static IEnumerable<(ZipInputStream stream, ZipEntry entry)> IterateZipContents( string zipFile )
        {
            if ( string.IsNullOrEmpty( zipFile ) )
                yield break;

            if ( !File.Exists( zipFile ) )
                yield break;

            using ( var inputStream = new FileStream( zipFile, FileMode.Open ) )
            {
                using ( var zipStream = new ZipInputStream( inputStream ) )
                {
                    ZipEntry zipEntry;
                    while ( (zipEntry = zipStream.GetNextEntry()) != null )
                    {
                        yield return (zipStream, zipEntry);
                    }
                }
            }
        }

        protected override void OnRender( Rectangle[] rois, int startIndex, int length )
        {
            if ( ( Token.TriggerFinalRender && triggerFinalRender ) ||
                 repeatEffect )
            {
                triggerFinalRender = false;

                var finalRender = new regenesis.RenderRunner( DstArgs.Size, DstArgs.Size, 1 );

                ExplorerForm.TokenToRenderRunner(Token, DstArgs.Size, finalRender );

                // plug in source image if requested
                if ( Token.UseBackgroundSourceImage )
                {
                    finalRender.SetBackgroundImage( SrcArgs.Surface.CreateAliasedBitmap() );
                }

                if ( Token.MaskImage != null )
                {
                    finalRender.SetMaskImage( Token.MaskImage );
                }

                // ---------------------------------------------------------------------------------------------------
                // render from a local file? 
                if ( Token.EffectFile.Contains( GfxConstants.MagicSepInFile ) )
                {
                    var fileFxPair = Token.EffectFile.Split( new string[] { GfxConstants.MagicSepInFile }, 2, StringSplitOptions.RemoveEmptyEntries );
                    var effectName = fileFxPair.Last();

                    var gfxFileToLoad = fileFxPair.First();
                    if ( !File.Exists( gfxFileToLoad ) )
                    {
                        throw new Exception( $"Could not find effect file for [{Token.EffectFile}]" );
                    }

                    var buffer = File.ReadAllBytes( gfxFileToLoad );

                    var meta = new regenesis.ResultMeta();
                    finalRender.DoRender( buffer, ref meta );

                    DstArgs.Surface.CopyFromGdipBitmap( finalRender.ImageBuffer );

                    return;
                }
                // ---------------------------------------------------------------------------------------------------
                // .. or render from a zip archive
                else
                {
                    var zipFxPair = Token.EffectFile.Split( new char[] { GfxConstants.MagicSepInZip }, 2 );
                    var effectName = zipFxPair.Last();

                    foreach ( var (stream, entry) in IterateZipContents( zipFxPair.First() ) )
                    {
                        if ( entry.Name == effectName )
                        {
                            byte[] buffer = new byte[stream.Length];
                            stream.Read( buffer, 0, (int)stream.Length );

                            var meta = new regenesis.ResultMeta();
                            finalRender.DoRender( buffer, ref meta );

                            DstArgs.Surface.CopyFromGdipBitmap( finalRender.ImageBuffer );

                            return;
                        }
                    }
                }

                throw new Exception( $"Could not find effect data\n{Token.EffectFile}" );
            }

            triggerFinalRender = false;
        }

        public override EffectConfigDialog CreateConfigDialog()
        {
            // create a directory path for user files
            var userDataPath = Services.GetService<PaintDotNet.AppModel.IUserFilesService>().UserFilesPath;
            var gvfxDataPath = Path.Combine( userDataPath, "GenesisVFX" );
            Directory.CreateDirectory( gvfxDataPath );

            repeatEffect = false;
            return new ExplorerForm( gvfxDataPath );
        }
    }
}
