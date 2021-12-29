/*
    GenesisVFX for Paint.NET, part of the regenesis project
    Harry Denholm, ishani.org 2018-2021
*/

using System;
using System.Linq.Expressions;
using System.Windows.Forms;
using Microsoft.Win32;

namespace GenesisVFX
{
    #region Generic registry access

    public static class RegistryBooleanSupport
    {
        public static object GetValueFromKey( this RegistryKey reg, string keyName, object state )
        {
            if ( state.GetType() == typeof( Boolean ) )
            {
                var stateAsBool    = (Boolean)state;

                Int32 defaultState = (stateAsBool ? 1 : 0);
                defaultState       = (Int32)reg.GetValue( keyName, defaultState );

                return ( defaultState != 0 );
            }
            else
            {
                return reg.GetValue( keyName, state );
            }
        }

        public static void SetKeyFromValue( this RegistryKey reg, string keyName, object state )
        {
            if ( state.GetType() == typeof( Boolean ) )
            {
                var stateAsBool = (Boolean)state;

                Int32 saveValue = (stateAsBool ? 1 : 0);
                reg.SetValue( keyName, saveValue );
            }
            else
            {
                reg.SetValue( keyName, state );
            }
        }
    }

    internal class Win32Registry
    {
        // root reg key 
        private const string CVXRegistryKey = "Software\\Ishani\\GenesisVFX_PDN";

        public T LoadFrom<T>( string keyName, T defaultValue )
        {
            RegistryKey key = Registry.CurrentUser.OpenSubKey(CVXRegistryKey);
            if ( key == null )
            {
                key = Registry.CurrentUser.CreateSubKey( CVXRegistryKey );
            }

            T itemDefaultState = defaultValue;
            try
            {
                itemDefaultState = (T)key.GetValueFromKey( keyName, itemDefaultState );
            }
            catch ( Exception e )
            {
                MessageBox.Show( $"Error when reading setting from registry:\n\n{e.StackTrace}", "Settings Load Error", MessageBoxButtons.OK, MessageBoxIcon.Error );
            }
            finally
            {
                key.Close();
            }

            return ( ( itemDefaultState != null ) ? itemDefaultState : defaultValue );
        }

        public void SaveTo<T>( string keyName, T saveValue )
        {
            RegistryKey key = Registry.CurrentUser.OpenSubKey(CVXRegistryKey, true);
            if ( key == null )
            {
                key = Registry.CurrentUser.CreateSubKey( CVXRegistryKey );
            }

            try
            {
                key.SetKeyFromValue( keyName, saveValue );
            }
            finally
            {
                key.Close();
            }
        }
    }

    #endregion

    internal class RegistryItem<T>
    {
        private readonly             T _default_value;
        private readonly        string _name;
        private readonly Win32Registry _registry = new Win32Registry();

        public RegistryItem( Expression<Func<RegistryItem<T>>> expr, T defaultValue )
        {
            var body       = ((MemberExpression)expr.Body);
            _name          = body.Member.Name;
            _default_value = defaultValue;
        }

        // push or pull values from the registry automatically
        public T Value
        {
            get {
                // sketchy way to get name of property from inside accessors
                return _registry.LoadFrom( _name, _default_value );
            }

            set { _registry.SaveTo( _name, value ); }
        }

        // this is just sugar that allows for
        // string foo = ShowCommands;
        public static implicit operator T( RegistryItem<T> d )
        {
            return d.Value;
        }
    }

    // Wrapper around registry access to load/save settings
    internal abstract class ConfigRegistry
    {
        public static RegistryItem<string> PathToContentLibrary = new RegistryItem<string>(() => PathToContentLibrary,  string.Empty );
        public static   RegistryItem<int> DynamicResolutionMode = new RegistryItem<int>(()    => DynamicResolutionMode, 2 );
        public static RegistryItem<string>         VersionStamp = new RegistryItem<string>(() => VersionStamp,          string.Empty );
    }
}