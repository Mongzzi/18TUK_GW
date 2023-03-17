using UnityEngine;
using UnityEditor;

public class ConvertFBXToTXT : EditorWindow
{
    [MenuItem("Window/Convert FBX to TXT")]
    static void Init()
    {
        ConvertFBXToTXT window = (ConvertFBXToTXT)EditorWindow.GetWindow(typeof(ConvertFBXToTXT));
        window.Show();
    }

    private void OnGUI()
    {
        GUILayout.Label("Convert FBX to TXT", EditorStyles.boldLabel);

        if (GUILayout.Button("Convert"))
        {
            string fbxPath = EditorUtility.OpenFilePanel("Select FBX file", "", "fbx");
            if (!string.IsNullOrEmpty(fbxPath))
            {
                string txtPath = fbxPath.Replace(".fbx", ".txt");
                AssetDatabase.ExportPackage(fbxPath, txtPath, ExportPackageOptions.IncludeDependencies | ExportPackageOptions.Recurse);
                Debug.Log($"Successfully converted {fbxPath} to text format at {txtPath}");
            }
        }
    }
}