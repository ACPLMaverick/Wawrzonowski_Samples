using UnityEngine;
using UnityEngine.EventSystems;
using System.Collections;
using System.Collections.Generic;

struct Uint2
{
    public uint x;
    public uint y;

    public Uint2(uint x, uint y)
    {
        this.x = x;
        this.y = y;
    }
}

public class FluidControllerGPU : Singleton<FluidControllerGPU>
{

    public uint particleCount;
    public double particleMass;
    public double particleVelocity;

    public FluidParticle[] particles;
    public FluidContainer container;
    public FluidParticle baseObject;
    public Dropper dropper;

    public GameObject initialPosition;
    public uint IDController;
    public bool canDelete;

    public bool startSimulation = false;
    public float particleOffsetX;
    public float particleOffsetY;

    private uint particleWidth;

    #region simRelated

	public float DISSIPATION = 0.9f;
    public uint JACOBI_ITERATIONS = 20;
    const uint CB_COUNT = 7;
    const int THREAD_COUNT = 32;

    public ComputeShader cShader;

    private enum KernelIDs
    {
        ADVECT,
        DIFFUSE,
        APPLY_FORCE,
        PRESSURE,
        SUBTRACT_PRESSURE,
        SWAP_TO_NEW,
        SWAP_TO_OLD,
        CLEAR
    }

    private int[] kernelIDs = new int[8];

    private bool vfInitialized = false;
    private ComputeBuffer velocityField;
    private ComputeBuffer velocityFieldNew;
    private ComputeBuffer pressureField;
    private ComputeBuffer pressureFieldNew;
    private ComputeBuffer jacobiHelper;
    private ComputeBuffer particleData;
    private ComputeBuffer dyeField;

    private ComputeBuffer[] cbArray = new ComputeBuffer[CB_COUNT];

    private Texture2D finalTexture;
    private Texture2D _DfinalTexture;
    private Vector2[] velocityFieldBuffer;
    private Vector4[] particleDataBuffer;
    private float[] pressureFieldBuffer;
    private float[] dyeFieldBuffer;

    #endregion

    #region main

    protected FluidControllerGPU() { }

    public void Start()
    {
        particleCount = 1024;
        particles = new FluidParticle[1024];
        IDController = 0;
        particleOffsetX = 0.25f;
        particleOffsetY = 0.25f;
    }

    public void FixedUpdate()
    {
        if (vfInitialized)
        {
            CalculateVectorField();
            ApplyVectorField();
        }
    }
    /*
    public void OnDisable()
    {
        velocityField.Dispose();
        velocityFieldNew.Dispose();
        pressureField.Dispose();
        pressureFieldNew.Dispose();
        jacobiHelper.Dispose();
        particleData.Dispose();
    }
    */
    #endregion

    #region sim

    public void InitializeVectorField()
    {
        velocityField = new ComputeBuffer((int)particleCount, 2 * sizeof(float));
        velocityFieldNew = new ComputeBuffer((int)particleCount, 2 * sizeof(float));
        pressureField = new ComputeBuffer((int)particleCount, sizeof(float));
        pressureFieldNew = new ComputeBuffer((int)particleCount, sizeof(float));
        jacobiHelper = new ComputeBuffer((int)particleCount, 2 * sizeof(float));
        particleData = new ComputeBuffer((int)particleCount, 4 * sizeof(float));
        dyeField = new ComputeBuffer((int)particleCount, sizeof(float));
        cbArray[0] = velocityField;
        cbArray[1] = velocityFieldNew;
        cbArray[2] = pressureField;
        cbArray[3] = pressureFieldNew;
        cbArray[4] = jacobiHelper;
        cbArray[5] = particleData;
        cbArray[6] = dyeField;

        for (int i = 0; i < CB_COUNT; ++i)
        {
            
        }

        velocityField.SetData(new Vector2[particleCount]);
        velocityFieldNew.SetData(new Vector2[particleCount]);
        pressureField.SetData(new float[particleCount]);
        pressureFieldNew.SetData(new float[particleCount]);
        jacobiHelper.SetData(new Vector2[particleCount]);
        particleData.SetData(new Vector2[particleCount]);
        dyeField.SetData(new Vector2[particleCount]);

        velocityFieldBuffer = new Vector2[particleCount];
        particleDataBuffer = new Vector4[particleCount];
        pressureFieldBuffer = new float[particleCount];
        dyeFieldBuffer = new float[particleCount];

        finalTexture = new Texture2D((int)particleWidth, (int)particleWidth, TextureFormat.RGBAFloat, false);
        finalTexture.wrapMode = TextureWrapMode.Clamp;

        _DfinalTexture = new Texture2D((int)particleWidth, (int)particleWidth, TextureFormat.RGBAFloat, false);
        _DfinalTexture.wrapMode = TextureWrapMode.Clamp;

        MaterialPropertyBlock mp = new MaterialPropertyBlock();
        mp.AddTexture(0, finalTexture);
        container.MySprite.SetPropertyBlock(mp);

        kernelIDs[0] = cShader.FindKernel("Advect");
        kernelIDs[1] = cShader.FindKernel("Diffuse");
        kernelIDs[2] = cShader.FindKernel("ApplyForces");
        kernelIDs[3] = cShader.FindKernel("Pressure");
        kernelIDs[4] = cShader.FindKernel("SubtractPressure");
        kernelIDs[5] = cShader.FindKernel("SwapOldToNew");
        kernelIDs[6] = cShader.FindKernel("SwapNewToOld");
        kernelIDs[7] = cShader.FindKernel("Clear");

        cShader.SetBuffer(kernelIDs[(int)KernelIDs.ADVECT], "VelocityField", velocityField);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.ADVECT], "VelocityFieldNew", velocityFieldNew);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.ADVECT], "DyeField", dyeField);

        cShader.SetBuffer(kernelIDs[(int)KernelIDs.DIFFUSE], "VelocityField", velocityFieldNew);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.DIFFUSE], "VelocityFieldNew", velocityField);

        cShader.SetBuffer(kernelIDs[(int)KernelIDs.APPLY_FORCE], "VelocityField", velocityField);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.APPLY_FORCE], "VelocityFieldNew", velocityFieldNew);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.APPLY_FORCE], "DyeField", dyeField);

        cShader.SetBuffer(kernelIDs[(int)KernelIDs.PRESSURE], "VelocityField", velocityFieldNew);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.PRESSURE], "VelocityFieldNew", velocityField);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.PRESSURE], "PressureField", pressureField);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.PRESSURE], "PressureFieldNew", pressureFieldNew);

        cShader.SetBuffer(kernelIDs[(int)KernelIDs.SWAP_TO_NEW], "VelocityField", velocityField);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.SWAP_TO_NEW], "VelocityFieldNew", velocityFieldNew);

        cShader.SetBuffer(kernelIDs[(int)KernelIDs.SWAP_TO_OLD], "VelocityField", velocityFieldNew);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.SWAP_TO_OLD], "VelocityFieldNew", velocityField);

        cShader.SetBuffer(kernelIDs[(int)KernelIDs.SUBTRACT_PRESSURE], "VelocityField", velocityField);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.SUBTRACT_PRESSURE], "VelocityFieldNew", velocityFieldNew);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.SUBTRACT_PRESSURE], "PressureField", pressureField);

        cShader.SetBuffer(kernelIDs[(int)KernelIDs.CLEAR], "PressureField", pressureField);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.CLEAR], "PressureFieldNew", pressureFieldNew);

        cShader.SetFloat("DeltaTime", Time.fixedDeltaTime);
        float dx = container.containerBase / (float)particleWidth;
        cShader.SetFloat("Dx", dx);
        cShader.SetInt("Width", (int)particleWidth);
        cShader.SetFloat("ContainerElasticity", (float)container.elasticity);
        cShader.SetFloat("DropperInsertedDensity", dropper.InsertedDensity);
        cShader.SetFloat("Dissipation", DISSIPATION);
        // setup particle data to texture
        for (uint i = 0; i < particleWidth; ++i)
        {
            for (uint j = 0; j < particleWidth; ++j)
            {
                uint flatCoord = i * particleWidth + j;

                particleDataBuffer[flatCoord].x = particles[flatCoord].transform.position.x;
                particleDataBuffer[flatCoord].y = particles[flatCoord].transform.position.y;
                particleDataBuffer[flatCoord].z = (float)particles[flatCoord].viscosity;
                particleDataBuffer[flatCoord].w = (float)particles[flatCoord].mass;
            }
        }
        particleData.SetData(particleDataBuffer);

        cShader.SetBuffer(kernelIDs[(int)KernelIDs.ADVECT], "ParticleData", particleData);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.DIFFUSE], "ParticleData", particleData);
        cShader.SetBuffer(kernelIDs[(int)KernelIDs.APPLY_FORCE], "ParticleData", particleData);
        vfInitialized = true;
    }

    private void CalculateVectorField()
    {
        // setup dropper
        cShader.SetFloats("DropperPosition", new float[] { dropper.CurrentForcePosition.x, dropper.CurrentForcePosition.y });
        cShader.SetFloats("DropperDirection", new float[] { dropper.CurrentForceDirection.x, dropper.CurrentForceDirection.y });
        cShader.SetFloat("DropperRadius", dropper.Radius);
        cShader.SetFloat("DropperForceValue", dropper.ForceValue);

        if(dropper.Active)
        {
            cShader.SetFloat("DropperForceMultiplier", 1.0f);
        }
        else
        {
            cShader.SetFloat("DropperForceMultiplier", 0.0f);
        }

        ///////////////////////////////////////////////////

        cShader.Dispatch(kernelIDs[(int)KernelIDs.ADVECT], (int)particleWidth / THREAD_COUNT, (int)particleWidth / THREAD_COUNT, 1);
        cShader.Dispatch(kernelIDs[(int)KernelIDs.SWAP_TO_OLD], (int)particleWidth / THREAD_COUNT, (int)particleWidth / THREAD_COUNT, 1);

        cShader.Dispatch(kernelIDs[(int)KernelIDs.APPLY_FORCE], (int)particleWidth / THREAD_COUNT, (int)particleWidth / THREAD_COUNT, 1);
        SolveBoundaries(velocityFieldNew, pressureField, null);
        //velocityFieldNew.GetData(velocityFieldBuffer);
        //SolveBoundaries();
        //velocityFieldNew.SetData(velocityFieldBuffer);

        ComputeBuffer first = velocityFieldNew;
        ComputeBuffer second = velocityField;
        ComputeBuffer b;
        for (int i = 0; i < JACOBI_ITERATIONS; ++i )
        {
            cShader.Dispatch(kernelIDs[(int)KernelIDs.DIFFUSE], (int)particleWidth / THREAD_COUNT, (int)particleWidth / THREAD_COUNT, 1);
            b = first;
            first = second;
            second = b;
            cShader.SetBuffer(kernelIDs[(int)KernelIDs.DIFFUSE], "VelocityField", first);
            cShader.SetBuffer(kernelIDs[(int)KernelIDs.DIFFUSE], "VelocityFieldNew", second);
        }

        cShader.Dispatch(kernelIDs[(int)KernelIDs.SWAP_TO_OLD], (int)particleWidth / THREAD_COUNT, (int)particleWidth / THREAD_COUNT, 1);
        
        first = pressureField;
        second = pressureFieldNew;
        for (int i = 0; i < JACOBI_ITERATIONS; ++i)
        {
            cShader.Dispatch(kernelIDs[(int)KernelIDs.PRESSURE], (int)particleWidth / THREAD_COUNT, (int)particleWidth / THREAD_COUNT, 1);
            b = first;
            first = second;
            second = b;
            cShader.SetBuffer(kernelIDs[(int)KernelIDs.PRESSURE], "PressureField", first);
            cShader.SetBuffer(kernelIDs[(int)KernelIDs.PRESSURE], "PressureFieldNew", second);
        }

        SolveBoundaries(velocityField, pressureField, null);

        cShader.Dispatch(kernelIDs[(int)KernelIDs.SUBTRACT_PRESSURE], (int)particleWidth / THREAD_COUNT, (int)particleWidth / THREAD_COUNT, 1);
 
        SolveBoundaries(velocityFieldNew, pressureField, dyeField);

        cShader.Dispatch(kernelIDs[(int)KernelIDs.SWAP_TO_OLD], (int)particleWidth / THREAD_COUNT, (int)particleWidth / THREAD_COUNT, 1);
        cShader.Dispatch(kernelIDs[(int)KernelIDs.CLEAR], (int)particleWidth / THREAD_COUNT, (int)particleWidth / THREAD_COUNT, 1);

        ////////////////////////////////////////////////////////////////

        for (uint i = 0; i < particleWidth; ++i)
        {
            for (uint j = 0; j < particleWidth; ++j)
            {
                // applying texture data
                uint id = i * particleWidth + j;
                //float length = velocityFieldBuffer[id].x * velocityFieldBuffer[id].x + velocityFieldBuffer[id].y * velocityFieldBuffer[id].y;
                finalTexture.SetPixel((int)j, (int)i, new Color(dropper.DyeRValue * dyeFieldBuffer[id],
				                                                dropper.DyeGValue * dyeFieldBuffer[id],
				                                                dropper.DyeBValue * dyeFieldBuffer[id], 1.0f));
                //finalTexture.SetPixel((int)j, (int)i, new Color(velocityFieldBuffer[id].x, velocityFieldBuffer[id].y, 0.0f, 1.0f));
                //_DfinalTexture.SetPixel((int)j, (int)i, new Color(_DpressureFieldBuffer[id], _DpressureFieldBuffer[id], _DpressureFieldBuffer[id], 1.0f));
                
                // signing borders
                if(i == 0 || j == 0 || i == particleWidth - 1 || j == particleWidth - 1)
                    finalTexture.SetPixel((int)j, (int)i, new Color(0.0f, 0.0f, 1.0f, 1.0f));
            }
        }
        finalTexture.Apply();
        _DfinalTexture.Apply();
       
        //ApplyVectorField();
    }
    /*
    private void ApplyTextureData(Texture2D tex, ComputeBuffer field, Vector2[] buffer)
    {
        field.GetData(buffer);
        for (uint i = 0; i < particleWidth; ++i )
        {
            for(uint j = 0; j < particleWidth; ++j)
            {
                uint id = i * particleWidth + j;
                tex.SetPixel((int)j, (int)i, new Color(buffer[id].x, buffer[id].y, 0.0f, 1.0f));
            }
        }
            
        tex.Apply();
    }
    */

    private void SolveBoundaries(ComputeBuffer vBuffer, ComputeBuffer pBuffer, ComputeBuffer dBuffer)
    {
        vBuffer.GetData(velocityFieldBuffer);
        pBuffer.GetData(pressureFieldBuffer);
        if (dBuffer != null) dBuffer.GetData(dyeFieldBuffer);

        for(uint i = 0; i < particleWidth; ++i)
        {
            for(uint j = 0; j < particleWidth; ++j)
            {
                uint id = i * particleWidth + j;
                /*
                if(i == 0 && j == 0)
                {
                    // bl corner
                    velocityFieldBuffer[id] = -velocityFieldBuffer[(i + 1) * particleWidth + (j + 1)];
                    _DpressureFieldBuffer[id] = _DpressureFieldBuffer[(i + 1) * particleWidth + (j + 1)];
                }
                else if(i == 0 && j == particleWidth - 1)
                {
                    // br corner
                    velocityFieldBuffer[id] = -velocityFieldBuffer[(i + 1) * particleWidth + (j - 1)];
                    _DpressureFieldBuffer[id] = _DpressureFieldBuffer[(i + 1) * particleWidth + (j - 1)];
                }
                else if(i == particleWidth - 1 && j == particleWidth - 1)
                {
                    // tr corner
                    velocityFieldBuffer[id] = -velocityFieldBuffer[(i - 1) * particleWidth + (j - 1)];
                    _DpressureFieldBuffer[id] = _DpressureFieldBuffer[(i - 1) * particleWidth + (j - 1)];
                }
                else if(i == particleWidth - 1 && j == 0)
                {
                    // tl corner
                    velocityFieldBuffer[id] = -velocityFieldBuffer[(i - 1) * particleWidth + (j + 1)];
                    _DpressureFieldBuffer[id] = _DpressureFieldBuffer[(i - 1) * particleWidth + (j + 1)];
                }

                else*/ if(i == 0)
                {
                    // bottom
                    velocityFieldBuffer[id] = -velocityFieldBuffer[(i + 1) * particleWidth + (j + 1)];
                    pressureFieldBuffer[id] = pressureFieldBuffer[(i + 1) * particleWidth + (j)];
                    if (dBuffer != null) dyeFieldBuffer[id] = 0.0f;
                }
                else if(i == particleWidth - 1)
                {
                    // top
                    velocityFieldBuffer[id] = -velocityFieldBuffer[(i - 1) * particleWidth + (j)];
                    pressureFieldBuffer[id] = pressureFieldBuffer[(i - 1) * particleWidth + (j)];
                    if (dBuffer != null) dyeFieldBuffer[id] = 0.0f;
                }
                else if(j == 0)
                {
                    // left
                    velocityFieldBuffer[id] = -velocityFieldBuffer[(i) * particleWidth + (j + 1)];
                    pressureFieldBuffer[id] = pressureFieldBuffer[(i) * particleWidth + (j + 1)];
                    if (dBuffer != null) dyeFieldBuffer[id] = 0.0f;
                }
                else if(j == particleWidth - 1)
                {
                    // right
                    velocityFieldBuffer[id] = -velocityFieldBuffer[(i) * particleWidth + (j - 1)];
                    pressureFieldBuffer[id] = pressureFieldBuffer[(i) * particleWidth + (j - 1)];
                    if (dBuffer != null) dyeFieldBuffer[id] = 0.0f;
                }

                /*
                if (i == 0 || j == 0 || i == particleWidth - 1 || j == particleWidth - 1)
                {
                    velocityFieldBuffer[id] = Vector2.zero;
                    _DpressureFieldBuffer[id] = 0.0f;
                }
                */
            }
        }

        vBuffer.SetData(velocityFieldBuffer);
        pBuffer.SetData(pressureFieldBuffer);
        if (dBuffer != null) dBuffer.SetData(dyeFieldBuffer);
    }

    private void ApplyVectorField()
    {
        for (uint i = 0; i < particleCount; ++i)
        {
            // compute particle position in relation to container space
            Vector2 rPos = Vector2.zero;

            rPos.x = particles[i].transform.position.x - particles[0].transform.position.x;
            rPos.y = particles[i].transform.position.y - particles[0].transform.position.y;

            Uint2 tl, tr, br, bl;
            uint x = (uint)Mathf.Floor(rPos.x);
            uint y = (uint)Mathf.Floor(rPos.y);
            float xRatio = rPos.x - (float)x;
            float yRatio = rPos.y - (float)y;
            float xRatioOpp = 1.0f - xRatio;
            float yRatioOpp = 1.0f - yRatio;
            bl = new Uint2(x, y);
            tl = new Uint2(x, (uint)Mathf.Min(y + 1, particleWidth - 1));
            tr = new Uint2((uint)Mathf.Min(x + 1), (uint)Mathf.Min(y + 1));
            br = new Uint2((uint)Mathf.Min(x + 1), y);

            Vector2 velocity = Vector2.zero;
            /*
            velocity = (velocityFieldBuffer[Flatten2DCoords(bl.x, bl.y, particleWidth)] * xRatioOpp + velocityFieldBuffer[Flatten2DCoords(br.x, br.y, particleWidth)] * xRatio) * yRatioOpp +
                (velocityFieldBuffer[Flatten2DCoords(tl.x, tl.y, particleWidth)] * xRatioOpp + velocityFieldBuffer[Flatten2DCoords(tr.x, tr.y, particleWidth)] * xRatio) * yRatio;
            */
            velocity = velocityFieldBuffer[i] * 10.0f;
            Vector2 nPos = velocity * Time.fixedDeltaTime;

            particles[i].transform.position =
                new Vector3(
                        particles[i].transform.position.x + nPos.x,
                        particles[i].transform.position.y + nPos.y,
                        particles[i].transform.position.z
                    );

            float length = velocity.x * velocity.x + velocity.y * velocity.y;
            particles[i].GetComponent<SpriteRenderer>().color = new Color(length, length, length);
        }
    }

    private void SwapTextureContent(ref Texture2D first, ref Texture2D second)
    {

    }

    private void SwapColorContent(ref Color[] first, ref Color[] second, uint length)
    {
        Color buffer;
        for (uint i = 0; i < length; ++i)
        {
            buffer = second[i];
            second[i] = first[i];
            first[i] = buffer;
        }
    }

    private void SwapColor(ref Color[] first, ref Color[] second)
    {
        Color[] temp = first;
        first = second;
        second = temp;
    }

    private uint Flatten2DCoords(uint i, uint j, uint width)
    {
        return i * width + j;
    }

    private uint Flatten2DCoords(Vector2 coord, uint width)
    {
        return (uint)coord.x * width + (uint)coord.y;
    }

    private Vector2 Square1DCoords(uint coord, uint width)
    {
        Vector2 ret = Vector2.zero;

        ret.x = coord / width;
        ret.y = coord % width;

        return ret;
    }

    #endregion

    #region creation

    public void CreateParticles()
    {
        DestroyParticles();
        particles = new FluidParticle[particleCount];

        float x = initialPosition.transform.position.x;
        float y = initialPosition.transform.position.y;

        particleWidth = (uint)Mathf.Sqrt(particleCount);

        for (uint i = 0; i < particleWidth; ++i)
        {
            for (uint j = 0; j < particleWidth; ++j)
            {
                particles[j + particleWidth * i] = (FluidParticle)Instantiate(baseObject, new Vector2(x, y), Quaternion.identity);
                particles[j + particleWidth * i].viscosity = baseObject.viscosity;
                particles[j + particleWidth * i].position = particles[j + particleWidth * i].transform.position;
                particles[j + particleWidth * i].radius = container.containerBase / (float)particleWidth / 2.0f;
                CalculatePosition(ref x, ref y, particleCount, false);
            }
            CalculatePosition(ref x, ref y, particleCount, true);
        }

        //startSimulation = true;
    }

    public void CalculatePosition(ref float inputX, ref float inputY, uint count, bool moveUp)
    {
        switch (count)
        {
            case 1024:
                particleOffsetX = 0.285f;
                particleOffsetY = 0.285f;
                break;
            case 4096:
                particleOffsetX = 0.143f;
                particleOffsetY = 0.143f;
                break;
            case 16384:
                particleOffsetX = 0.072f;
                particleOffsetY = 0.072f;
                break;
        }

        inputX += particleOffsetX;
        if (moveUp)
        {
            inputX = initialPosition.transform.position.x;
            inputY += particleOffsetY;
        }
    }

    public void DestroyParticles()
    {
        if (particles[0] != null)
        {
            for (int i = 0; i < particles.Length; i++)
            {
                Destroy(particles[i].gameObject);
            }
        }
    }

    #endregion
}
