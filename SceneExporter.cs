using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;
using System.Text;
using UnityEditor.SearchService;
using Unity.VisualScripting;
using Unity.Mathematics;

public class MyBinaryWriter
{
    private BinaryWriter writer = null;

    public MyBinaryWriter(string filePath)
    {
        writer = new BinaryWriter(File.Open(filePath, FileMode.OpenOrCreate));
    }

    ~MyBinaryWriter()
    {
    }

    public void Close()
    {
        writer.Close();
    }

    public void WriteObjectName(Object obj)
    {
        writer.Write((obj) ? string.Copy(obj.name).Replace(" ", "_") : "null");
    }

    public void WriteObjectName(string strHeader, Object obj)
    {
        writer.Write(strHeader);
        writer.Write((obj) ? string.Copy(obj.name).Replace(" ", "_") : "null");
    }

    public void WriteString(string strToWrite)
    {
        writer.Write(strToWrite);
    }

    public void WriteString(string strToWrite, int i)
    {
        writer.Write(strToWrite);
        writer.Write(i);
    }

    public void WriteTextureName(string strHeader, Texture texture)
    {
        writer.Write(strHeader);
        if (texture)
        {
            writer.Write(string.Copy(texture.name).Replace(" ", "_"));
        }
        else
        {
            writer.Write("null");
        }
    }
    public void WriteTextureName(Texture texture)
    {
        if (texture)
        {
            writer.Write(string.Copy(texture.name).Replace(" ", "_"));
        }
        else
        {
            writer.Write("null");
        }
    }

    public void WriteInteger(int i)
    {
        writer.Write(i);
    }

    public void WriteInteger(string strHeader, int i)
    {
        writer.Write(strHeader);
        writer.Write(i);
    }

    public void WriteFloat(string strHeader, float f)
    {
        writer.Write(strHeader);
        writer.Write(f);
    }
    public void WriteFloat(float f)
    {
        writer.Write(f);
    }

    public void WriteVector(Vector2 v)
    {
        writer.Write(v.x);
        writer.Write(v.y);
    }

    public void WriteVector(Vector3 v)
    {
        writer.Write(v.x);
        writer.Write(v.y);
        writer.Write(v.z);
    }
    public void WriteVector(string strHeader, Vector3 v)
    {
        writer.Write(strHeader);
        writer.Write(v.x);
        writer.Write(v.y);
        writer.Write(v.z);
    }
    public void WriteVector(Vector4 v)
    {
        writer.Write(v.x);
        writer.Write(v.y);
        writer.Write(v.z);
        writer.Write(v.w);
    }
    public void WriteVector(Quaternion q)
    {
        writer.Write(q.x);
        writer.Write(q.y);
        writer.Write(q.z);
        writer.Write(q.w);
    }
    public void WriteColor(Color c)
    {
        writer.Write(c.r);
        writer.Write(c.g);
        writer.Write(c.b);
        writer.Write(c.a);
    }

    public void WriteColor(string strHeader, Color c)
    {
        writer.Write(strHeader);
        WriteColor(c);
    }

    public void WriteTextureCoord(Vector2 uv)
    {
        writer.Write(uv.x);
        writer.Write(1.0f - uv.y);
    }
    public void WriteVectors(string strHeader, Vector3[] vectors)
    {
        writer.Write(strHeader);
        writer.Write(vectors.Length);
        if (vectors.Length > 0) foreach (Vector3 v in vectors) WriteVector(v);
    }
    public void WriteColors(string strHeader, Color[] colors)
    {
        writer.Write(strHeader);
        writer.Write(colors.Length);
        if (colors.Length > 0) foreach (Color c in colors) WriteColor(c);
    }

    public void WriteTextureCoords(string strHeader, Vector2[] uvs)
    {
        writer.Write(strHeader);
        writer.Write(uvs.Length);
        if (uvs.Length > 0) foreach (Vector2 uv in uvs) WriteTextureCoord(uv);
    }
    public void WriteIntegers(string strHeader, int n, int[] pIntegers)
    {
        writer.Write(strHeader);
        writer.Write(n);
        writer.Write(pIntegers.Length);
        if (pIntegers.Length > 0) foreach (int i in pIntegers) writer.Write(i);
    }

    public void WriteBoundingBox(string strHeader, Bounds bounds)
    {
        writer.Write(strHeader);
        WriteVector(bounds.center);
        WriteVector(bounds.extents);
    }

    public void WriteMatrix(Matrix4x4 matrix)
    {
        writer.Write(matrix.m00);
        writer.Write(matrix.m10);
        writer.Write(matrix.m20);
        writer.Write(matrix.m30);
        writer.Write(matrix.m01);
        writer.Write(matrix.m11);
        writer.Write(matrix.m21);
        writer.Write(matrix.m31);
        writer.Write(matrix.m02);
        writer.Write(matrix.m12);
        writer.Write(matrix.m22);
        writer.Write(matrix.m32);
        writer.Write(matrix.m03);
        writer.Write(matrix.m13);
        writer.Write(matrix.m23);
        writer.Write(matrix.m33);
    }


    public void WriteVector(string strHeader, Quaternion q)
    {
        writer.Write(strHeader);
        WriteVector(q);
    }

    public void WriteTransform(string strHeader, Transform current)
    {
        writer.Write(strHeader);
        WriteVector(current.localPosition);
        WriteVector(current.localEulerAngles);
        WriteVector(current.localScale);
        WriteVector(current.localRotation);

        Matrix4x4 matrix = Matrix4x4.identity;
        matrix.SetTRS(current.localPosition, current.localRotation, current.localScale);
        WriteMatrix(matrix);
    }
    public void WriteTransform(Transform current)
    {
        WriteVector(current.localPosition);
        WriteVector(current.localEulerAngles);
        WriteVector(current.localScale);
        WriteVector(current.localRotation);

        Matrix4x4 matrix = Matrix4x4.identity;
        matrix.SetTRS(current.localPosition, current.localRotation, current.localScale);
        WriteMatrix(matrix);
    }


    public void WriteMeshInfo(Mesh mesh)
    {
        WriteInteger(mesh.vertexCount);
        WriteBoundingBox("<Bounds>:", mesh.bounds);

        if ((mesh.vertices != null) && (mesh.vertices.Length > 0)) WriteVectors("<Positions>:", mesh.vertices);
        if ((mesh.colors != null) && (mesh.vertices.Length > 0)) WriteColors("<Colors>:", mesh.colors);
        if ((mesh.uv != null) && (mesh.uv.Length > 0)) WriteTextureCoords("<TextureCoords0>:", mesh.uv);
        if ((mesh.uv2 != null) && (mesh.uv2.Length > 0)) WriteTextureCoords("<TextureCoords1>:", mesh.uv2);
        if ((mesh.normals != null) && (mesh.normals.Length > 0)) WriteVectors("<Normals>:", mesh.normals);

        if ((mesh.normals.Length > 0) && (mesh.tangents.Length > 0))
        {
            Vector3[] tangents = new Vector3[mesh.tangents.Length];
            Vector3[] biTangents = new Vector3[mesh.tangents.Length];
            for (int i = 0; i < mesh.tangents.Length; i++)
            {
                tangents[i] = new Vector3(mesh.tangents[i].x, mesh.tangents[i].y, mesh.tangents[i].z);
            }

            WriteVectors("<Tangents>:", tangents);
        }

        WriteInteger("<SubMeshes>:", mesh.subMeshCount);
        if (mesh.subMeshCount > 0)
        {
            for (int i = 0; i < mesh.subMeshCount; i++)
            {
                int[] subindicies = mesh.GetTriangles(i);
                WriteIntegers("<SubMesh>:", i, subindicies);
            }
        }

        WriteString("</Mesh>");
    }

    public void WriteSkinnedMeshInfo(SkinnedMeshRenderer skinnedMeshRenderer)
    {
        Mesh mesh = skinnedMeshRenderer.sharedMesh;

        // 뼈대 이름 추출
        Transform[] bones = skinnedMeshRenderer.bones;
        WriteInteger("<Bones>:", bones.Length);
        foreach (Transform bone in bones)
        {
            WriteObjectName(bone);
        }

        // 바인드 포즈 추출
        Matrix4x4[] bindPoses = mesh.bindposes;
        WriteInteger("<BindPoses>:", bindPoses.Length);
        foreach (Matrix4x4 bp in bindPoses)
        {
            WriteMatrix(bp);
        }

        // 가중치 정보 추출
        BoneWeight[] boneWeights = mesh.boneWeights;
        WriteInteger("<BoneWeights>:", boneWeights.Length);
        foreach (BoneWeight bw in boneWeights)
        {
            WriteInteger(bw.boneIndex0); // 뼈대 인덱스 0
            WriteFloat(bw.weight0);      // 가중치 0
            WriteInteger(bw.boneIndex1); // 뼈대 인덱스 1
            WriteFloat(bw.weight1);      // 가중치 1
            WriteInteger(bw.boneIndex2); // 뼈대 인덱스 2
            WriteFloat(bw.weight2);      // 가중치 2
            WriteInteger(bw.boneIndex3); // 뼈대 인덱스 3
            WriteFloat(bw.weight3);      // 가중치 3
        }

        WriteString("</SkinnedMesh>");
    }

    public void WriteMaterial(Material material)
    {
        WriteObjectName(material);

        string shaderName = material.shader.name;

        WriteObjectName(material.shader);

        switch (shaderName)
        {
            case "SyntyStudios/Basic_LOD_Shader":

                WriteTextureName("<AlbedoMap>:", material.GetTexture("_Albedo"));
                WriteColor("<AlbedoColor>:", material.GetColor("_AlbedoColour"));

                WriteFloat("<Smoothness>:", material.GetFloat("_Smoothness"));

                WriteFloat("<Metallic>:", material.GetFloat("_Metallic"));

                WriteTextureName("<NormalMap>:", material.GetTexture("_NormalMap"));

                break;
            case "SyntyStudios/SkyboxUnlit":
                WriteColor("<TopColor>:", material.GetColor("_ColorTop"));
                WriteColor("<BottomColor>:", material.GetColor("_ColorBottom"));
                WriteFloat("<Offset>:", material.GetFloat("_Offset"));
                WriteFloat("<Distance>:", material.GetFloat("_Distance"));

                WriteFloat("<Falloff>:", material.GetFloat("_Falloff"));

                break;
            case "SyntyStudios/Triplanar_01":
            case "SyntyStudios/Triplanar_Basic":

                WriteTextureName("<SidesMap>:", material.GetTexture("_Sides"));
                WriteTextureName("<SidesNormalMap>:", material.GetTexture("_SidesNormal"));

                WriteTextureName("<TopMap>:", material.GetTexture("_Top"));
                WriteTextureName("<TopNormalMap>:", material.GetTexture("_TopNormal"));

                WriteFloat("<FallOff>:", material.GetFloat("_FallOff"));
                WriteFloat("<Tiling>:", material.GetFloat("_Tiling"));



                break;
            case "SyntyStudios/VegitationShader":
            case "SyntyStudios/VegitationShader_Basic":

                WriteTextureName("<LeafAlbedoMap>:", material.GetTexture("_LeafTex"));
                WriteTextureName("<LeafNormalMap>:", material.GetTexture("_LeafNormalMap"));
                WriteColor("<LeafAlbedoColor>:", material.GetColor("_LeafBaseColour"));
                WriteFloat("<LeafSmoothness>:", material.GetFloat("_LeafSmoothness"));
                WriteFloat("<LeafMetallic>:", material.GetFloat("_LeafMetallic"));

                WriteTextureName("<TrunkAlbedoMap>:", material.GetTexture("_TunkTex"));
                WriteTextureName("<TrunkNormalMap>:", material.GetTexture("_TrunkNormalMap"));
                WriteColor("<TrunkAlbedoColor>:", material.GetColor("_TrunkBaseColour"));
                WriteFloat("<TrunkSmoothness>:", material.GetFloat("_TrunkSmoothness"));
                WriteFloat("<TrunkMetallic>:", material.GetFloat("_TrunkMetallic"));

                break;
            case "Universal Render Pipeline/Lit":

                WriteTextureName("<AlbedoMap>:", material.GetTexture("_BaseMap"));
                WriteColor("<AlbedoColor>:", material.GetColor("_BaseColor"));

                WriteFloat("<Smoothness>:", material.GetFloat("_Smoothness"));

                WriteTextureName("<MetallicMap>:", material.GetTexture("_MetallicGlossMap"));
                WriteFloat("<Metallic>:", material.GetFloat("_Metallic"));

                WriteTextureName("<SpecularMap>:", material.GetTexture("_SpecGlossMap"));
                WriteColor("<Specular>:", material.GetColor("_SpecColor"));

                WriteTextureName("<NormalMap>:", material.GetTexture("_BumpMap"));

                break;
            default:
                return;
        }

        WriteString("</Material>");
    }

    public void WriteTerrainData(TerrainData terrainData)
    {
        WriteObjectName(terrainData);

        WriteInteger(terrainData.heightmapResolution); 
        WriteVector(terrainData.size);

        int alphaMapCnt = terrainData.alphamapTextureCount;
        WriteInteger(alphaMapCnt);

        Texture2D[] alphaMaps = terrainData.alphamapTextures;

        int i = 0;
        foreach(Texture2D alphaMap in alphaMaps)
        {
            string name = terrainData.name + $"_splatmap_{i++}";
            WriteString(name);
        }
        TerrainLayer[] terrainLayers = terrainData.terrainLayers;
        int cnt = 0;
        WriteInteger(terrainLayers.Length);
        foreach (TerrainLayer terrainLayer in terrainLayers)
        {
            WriteTextureName(terrainLayer.diffuseTexture);
            WriteTextureName(terrainLayer.normalMapTexture);
            WriteFloat(terrainLayer.metallic);
            WriteFloat(terrainLayer.smoothness);
            cnt++;
        }
    }

    public Bounds WriteFrameInfo(Transform current)
    {
        string tag = current.gameObject.tag;
        WriteObjectName("<Frame>:", current.gameObject);
        WriteString("<Tag>:");
        WriteString(tag);
        WriteTransform("<Transform>:", current);

        SkinnedMeshRenderer skinnedMeshRenderer = current.gameObject.GetComponent<SkinnedMeshRenderer>();
        MeshFilter meshFilter = current.gameObject.GetComponent<MeshFilter>();
        MeshRenderer meshRenderer = current.gameObject.GetComponent<MeshRenderer>();

        Bounds bounds = new Bounds();

        if (meshFilter && meshRenderer)
        {
            if (skinnedMeshRenderer) {
                WriteObjectName("<SkinnedMesh>:", meshFilter.sharedMesh);
                WriteSkinnedMeshInfo(skinnedMeshRenderer);
                bounds = skinnedMeshRenderer.bounds;
            }
            else {
                WriteObjectName("<Mesh>:", meshFilter.sharedMesh);
                bounds = meshRenderer.bounds;
            }

            Material[] materials = meshRenderer.sharedMaterials;
            WriteInteger(materials.Length);
            if (materials.Length > 0)
            {
                foreach (Material mat in materials)
                {
                    WriteObjectName(mat);
                }
            }
        }

        switch (tag)
        {
            case "Light":
                break;
            case "MainCamera":
                break;
            case "Skydome":
                break;
            case "FogRing":
                break;
            case "Terrain":
                {
                    Terrain terrain = current.gameObject.GetComponent<Terrain>();
                    WriteTerrain(terrain);
                }
                break;
            default:
                break;
        }

        WriteAnimationInfo(current);

        BoxCollider collider = new();
        if (current.TryGetComponent<BoxCollider>(out collider))
        {
            WriteString("<Collider>:");
            WriteVector(collider.center);
            WriteVector(collider.size);
        }

        return bounds;
    }

    public Bounds WriteFrameHierarchyInfo(Transform child, bool isPrefabable)
    {
        Bounds bounds = new Bounds();

        if (isPrefabable && PrefabUtility.IsOutermostPrefabInstanceRoot(child.gameObject)) {
            GameObject prefabSource = (GameObject)PrefabUtility.GetCorrespondingObjectFromSource(child.gameObject);
            WriteObjectName("<Prefab>:", prefabSource);
            WriteTransform(child);
        }
        else {
            bounds = WriteFrameInfo(child);
            WriteInteger("<Children>:", child.childCount);

            if (child.childCount > 0)
            {
                for (int k = 0; k < child.childCount; k++)
                {
                    bounds.Encapsulate(WriteFrameHierarchyInfo(child.GetChild(k), isPrefabable));
                }
            }

            WriteString("</Frame>");
        }

        return bounds;
    }

    public void WriteTerrain(Terrain terrain)
    {
        WriteString("<Terrain>:");
        TerrainData terrainData = terrain.terrainData;
        WriteTerrainData(terrainData);
        WriteVector(terrain.gameObject.transform.position);
    }

    public void WriteAnimationInfo(Transform current)
    {

        Animator animator = current.GetComponent<Animator>();
        if (animator != null && animator.runtimeAnimatorController != null) {
            AnimationClip[] clips = animator.runtimeAnimatorController.animationClips;
            foreach (AnimationClip clip in clips)
            {
                writer.Write(clip.name);
                break;
            }
        }
        else {
            Animation animation = current.GetComponent<Animation>();
            if (animation != null) {
                foreach (AnimationState state in animation)
                {
                    writer.Write(state.clip.name);
                    break;
                }
            }
        }
    }

    public void WriteObject(GameObject gameObject, bool isPrefabable)
    {
        Transform transform = gameObject.transform;
        Bounds combinedBounds = WriteFrameHierarchyInfo(transform, isPrefabable);

        Vector3 sphereCenter = transform.InverseTransformPoint(combinedBounds.center);
        float radius = combinedBounds.extents.magnitude;
        float maxScale = Mathf.Max(transform.lossyScale.x, transform.lossyScale.y, transform.lossyScale.z);
        radius /= maxScale;

        WriteVector(sphereCenter);
        WriteFloat(radius);

        switch (gameObject.tag)
        {
            case "Light":
                break;
            case "MainCamera":
                break;
            case "Skydome":
                break;
            case "FogRing":
                break;
            case "Terrain":
                break;
            default:
                break;
        }
    }

}

public class SceneObjectExporter
{
    [MenuItem("Scene/Export Scene")]
    static void ExportAllObjects()
    {
        GameObject[] allObjects = GameObject.FindObjectsOfType<GameObject>();
        Dictionary<string, GameObject> prefabSources = new Dictionary<string, GameObject>();

        string filePath = EditorUtility.SaveFilePanel("Save Scene bin", "Assets", "Scene.bin", "bin");
        if (string.IsNullOrEmpty(filePath)) return;

        MyBinaryWriter binaryWriter = new MyBinaryWriter(filePath);

        ExportResources(allObjects, binaryWriter);

        foreach (GameObject obj in allObjects)
        {
            if(!obj.activeSelf) continue;
            if (PrefabUtility.IsOutermostPrefabInstanceRoot(obj))
            {
                GameObject prefabSource = (GameObject)PrefabUtility.GetCorrespondingObjectFromSource(obj);
                if (!prefabSources.ContainsKey(prefabSource.name))
                {
                    prefabSources[prefabSource.name] = prefabSource;
                }
            }
        }

        binaryWriter.WriteInteger(prefabSources.Count);
        foreach (GameObject obj in prefabSources.Values)
        {
            binaryWriter.WriteObject(obj, false);
        }

        GameObject[] rootObjects = UnityEngine.SceneManagement.SceneManager.GetActiveScene().GetRootGameObjects();
        int activeRootCount = 0;
        foreach (GameObject root in rootObjects)
        {
            if (root.activeInHierarchy)
            {
                activeRootCount++;
            }
        }

        binaryWriter.WriteInteger(activeRootCount);
        foreach (GameObject obj in rootObjects)
        {
            if (!obj.activeSelf) continue;
            binaryWriter.WriteObject(obj, true);
        }

        binaryWriter.Close();
        Debug.Log($"씬 추출 완료");
    }

    [MenuItem("Scene/Export Objects Mesh")]
    static void ExportObjectsMesh()
    {
        // 현재 씬의 모든 GameObject 추출
        GameObject[] allObjects = GameObject.FindObjectsOfType<GameObject>();
        Dictionary<string, Mesh> sceneMeshes = new Dictionary<string, Mesh>();

        foreach (GameObject obj in allObjects)
        {
            MeshFilter meshFilter = obj.GetComponent<MeshFilter>();
            if (meshFilter != null && meshFilter.sharedMesh != null)
            {
                string meshName = meshFilter.sharedMesh.name;
                if (!sceneMeshes.ContainsKey(meshName))
                {
                    sceneMeshes[meshName] = meshFilter.sharedMesh;
                }
            }
        }

        int total = sceneMeshes.Count;
        int current = 0;
        foreach (Mesh mesh in sceneMeshes.Values)
        {
            current++;

            string filePath = "./Meshes/" + string.Copy(mesh.name).Replace(" ", "_") + ".bin";
            if (!string.IsNullOrEmpty(filePath)) {
                MyBinaryWriter writer = new(filePath);
                writer.WriteMeshInfo(mesh);

                writer.Close();
            }
            EditorUtility.DisplayProgressBar("Exporting Meshes", $"Processing {current}/{total}", (float)current / total);

        }
        EditorUtility.ClearProgressBar();
    }

    static void ExportResources(GameObject[] allObjects, MyBinaryWriter writer)
    {
        Dictionary<string, Mesh> sceneMeshes = new Dictionary<string, Mesh>();
        Dictionary<string, Material> sceneMaterials = new Dictionary<string, Material>();

        //씬 내 공유 리소스 탐색
        foreach (GameObject obj in allObjects)
        {
            MeshFilter meshFilter = obj.GetComponent<MeshFilter>();
            if (meshFilter != null && meshFilter.sharedMesh != null)
            {
                string meshName = meshFilter.sharedMesh.name;
                if (!sceneMeshes.ContainsKey(meshName))
                {
                    sceneMeshes[meshName] = meshFilter.sharedMesh;
                }
            }
            Renderer renderer = obj.GetComponent<Renderer>();
            if (renderer)
            {
                Material[] materials = renderer.sharedMaterials;
                foreach (Material material in materials)
                {
                    string matName = material.name;
                    if (!sceneMaterials.ContainsKey(matName))
                    {
                        sceneMaterials[matName] = material;
                    }
                }
            }
        }
        //리소스 추출
        writer.WriteInteger(sceneMeshes.Count);

        foreach (Mesh mesh in sceneMeshes.Values)
        {
            writer.WriteObjectName(mesh);
        }

        writer.WriteInteger(sceneMaterials.Count);
        foreach (Material material in sceneMaterials.Values)
        {
            writer.WriteMaterial(material);
        }

        Debug.Log($"리소스 추출 완료");
    }

}
